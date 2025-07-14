#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/pagemap.h>  // for simple_statfs
#include <linux/kernel.h>

#define SIMPLEFS_MAGIC 0x20240710

static struct dentry *simplefs_mount(struct file_system_type *fs_type,
                                     int flags, const char *dev_name,
                                     void *data);

static void simplefs_kill_sb(struct super_block *sb)
{
    pr_info("SimpleFS: unmounted\n");
    kill_litter_super(sb);
}

static const struct super_operations simplefs_super_ops = {
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

static int simplefs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct inode *inode;

    sb->s_magic = SIMPLEFS_MAGIC;
    sb->s_op = &simplefs_super_ops;

    inode = new_inode(sb);
    if (!inode)
        return -ENOMEM;

    inode->i_ino = 1;
    inode->i_sb = sb;
    inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);
    inode->i_mode = S_IFDIR | 0755;
    inode->i_op = &simple_dir_inode_operations;
    inode->i_fop = &simple_dir_operations;

    sb->s_root = d_make_root(inode);
    if (!sb->s_root)
        return -ENOMEM;

    return 0;
}

static struct dentry *simplefs_mount(struct file_system_type *fs_type,
                                     int flags, const char *dev_name,
                                     void *data)
{
    return mount_nodev(fs_type, flags, data, simplefs_fill_super);
}

static struct file_system_type simplefs_type = {
    .owner = THIS_MODULE,
    .name = "simplefs",
    .mount = simplefs_mount,
    .kill_sb = simplefs_kill_sb,
    .fs_flags = FS_USERNS_MOUNT,
};

static int __init simplefs_init(void)
{
    pr_info("SimpleFS: loading\n");
    return register_filesystem(&simplefs_type);
}

static void __exit simplefs_exit(void)
{
    pr_info("SimpleFS: unloading\n");
    unregister_filesystem(&simplefs_type);
}

module_init(simplefs_init);
module_exit(simplefs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sumanth");
MODULE_DESCRIPTION("Simple filesystem avoiding simple_super_operations error");

