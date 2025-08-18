#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>

#define DEVICE_NAME "ramblock"
#define RAMBLOCK_SECTOR_SIZE 512
#define RAMBLOCK_NUM_SECTORS 2048 // 1MB

static int major = 0;
static u8 *ramblock_data = NULL;

static struct gendisk *ramblock_disk = NULL;
static struct request_queue *ramblock_queue = NULL;
static DEFINE_SPINLOCK(ramblock_lock);

static blk_status_t ramblock_request(struct blk_mq_hw_ctx *hctx,
                                     const struct blk_mq_queue_data *bd)
{
    struct request *req = bd->rq;
    blk_status_t status = BLK_STS_OK;

    blk_mq_start_request(req);

    if (blk_rq_is_passthrough(req)) {
        pr_notice("ramblock: skip non-fs request\n");
        status = BLK_STS_IOERR;
        goto done;
    }

    sector_t sector = blk_rq_pos(req);
    unsigned int nsectors = blk_rq_sectors(req);
    struct bio_vec bvec;
    struct req_iterator iter;
    loff_t offset = sector * RAMBLOCK_SECTOR_SIZE;

    rq_for_each_segment(bvec, req, iter) {
        void *buffer = page_address(bvec.bv_page) + bvec.bv_offset;
        unsigned int len = bvec.bv_len;

        if ((offset + len) > (RAMBLOCK_NUM_SECTORS * RAMBLOCK_SECTOR_SIZE)) {
            pr_err("ramblock: write overflow\n");
            status = BLK_STS_IOERR;
            break;
        }

        if (rq_data_dir(req) == WRITE)
            memcpy(ramblock_data + offset, buffer, len);
        else
            memcpy(buffer, ramblock_data + offset, len);

        offset += len;
    }

done:
    blk_mq_end_request(req, status);
    return status;
}

static struct blk_mq_ops ramblock_mq_ops = {
    .queue_rq = ramblock_request,
};

static struct blk_mq_tag_set tag_set;

static int __init ramblock_init(void)
{
    major = register_blkdev(0, DEVICE_NAME);
    if (major <= 0) {
        pr_err("ramblock: unable to register block device\n");
        return -EBUSY;
    }

    ramblock_data = vmalloc(RAMBLOCK_NUM_SECTORS * RAMBLOCK_SECTOR_SIZE);
    if (!ramblock_data) {
        unregister_blkdev(major, DEVICE_NAME);
        return -ENOMEM;
    }

    ramblock_queue = blk_mq_init_sq_queue(&tag_set, &ramblock_mq_ops, 128, BLK_MQ_F_SHOULD_MERGE);
    if (IS_ERR(ramblock_queue)) {
        vfree(ramblock_data);
        unregister_blkdev(major, DEVICE_NAME);
        return PTR_ERR(ramblock_queue);
    }

    blk_queue_logical_block_size(ramblock_queue, RAMBLOCK_SECTOR_SIZE);

    ramblock_disk = alloc_disk(1);
    if (!ramblock_disk) {
        blk_cleanup_queue(ramblock_queue);
        vfree(ramblock_data);
        unregister_blkdev(major, DEVICE_NAME);
        return -ENOMEM;
    }

    ramblock_disk->major = major;
    ramblock_disk->first_minor = 0;
    ramblock_disk->fops = NULL; // not used with blk-mq
    ramblock_disk->queue = ramblock_queue;
    snprintf(ramblock_disk->disk_name, 32, DEVICE_NAME);
    set_capacity(ramblock_disk, RAMBLOCK_NUM_SECTORS);

    add_disk(ramblock_disk);

    pr_info("ramblock: loaded (major %d)\n", major);
    return 0;
}

static void __exit ramblock_exit(void)
{
    del_gendisk(ramblock_disk);
    put_disk(ramblock_disk);
    blk_cleanup_queue(ramblock_queue);
    vfree(ramblock_data);
    unregister_blkdev(major, DEVICE_NAME);
    pr_info("ramblock: unloaded\n");
}

module_init(ramblock_init);
module_exit(ramblock_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant Soni");
MODULE_DESCRIPTION("RAM-based block device for Raspberry Pi 4B - Kernel 6.12");
