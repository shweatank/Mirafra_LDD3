#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/pagemap.h>
#include <linux/time64.h>
#include <linux/string.h>
#include <linux/mount.h>   // for nop_mnt_idmap

#define SIMPLEFS_MAGIC 0x10032013
#define SIMPLEFS_FILE_NAME "simple.txt"
#define SIMPLEFS_FILE_CONTENT "Hello from kernel file system!\n"

static const struct super_operations simple_super_ops = { };

static ssize_t simplefs_read(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    return simple_read_from_buffer(buf, len, ppos,
                                    SIMPLEFS_FILE_CONTENT,
                                    strlen(SIMPLEFS_FILE_CONTENT));
}

static const struct file_operations simplefs_file_ops = {
    .read = simplefs_read,
    .llseek = default_llseek,
};

static const struct inode_operations simple_dir_inode_ops = { };

static int simplefs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct inode *root_inode, *file_inode;
    struct dentry *file_dentry;
    struct timespec64 ts;

    sb->s_magic = SIMPLEFS_MAGIC;
    sb->s_op = &simple_super_ops;

    root_inode = new_inode(sb);
    if (!root_inode)
        return -ENOMEM;

    root_inode->i_ino = 1;
    root_inode->i_mode = S_IFDIR | 0755;
    inode_init_owner(&nop_mnt_idmap, root_inode, NULL, root_inode->i_mode);
    root_inode->i_op = &simple_dir_inode_ops;

    ts = current_time(root_inode);
    inode_set_ctime(root_inode, ts.tv_sec, ts.tv_nsec);

    sb->s_root = d_make_root(root_inode);
    if (!sb->s_root)
        return -ENOMEM;

    file_inode = new_inode(sb);
    if (!file_inode)
        return -ENOMEM;

    file_inode->i_ino = 2;
    file_inode->i_mode = S_IFREG | 0444;
    inode_init_owner(&nop_mnt_idmap, file_inode, root_inode, file_inode->i_mode);
    file_inode->i_fop = &simplefs_file_ops;

    ts = current_time(file_inode);
    inode_set_ctime(file_inode, ts.tv_sec, ts.tv_nsec);

    file_dentry = d_alloc_name(sb->s_root, SIMPLEFS_FILE_NAME);
    if (!file_dentry)
        return -ENOMEM;

    d_add(file_dentry, file_inode);
    return 0;
}

static struct dentry *simplefs_mount(struct file_system_type *type,
                                     int flags, const char *dev_name,
                                     void *data)
{
    return mount_nodev(type, flags, data, simplefs_fill_super);
}

static void simplefs_kill_super(struct super_block *sb)
{
    kill_litter_super(sb);
}

static struct file_system_type simplefs_type = {
    .owner = THIS_MODULE,
    .name = "simplefs",
    .mount = simplefs_mount,
    .kill_sb = simplefs_kill_super,
};

static int __init simplefs_init(void)
{
    return register_filesystem(&simplefs_type);
}

static void __exit simplefs_exit(void)
{
    unregister_filesystem(&simplefs_type);
}

module_init(simplefs_init);
module_exit(simplefs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant @ TechDhaba, Updated by ChatGPT");
MODULE_DESCRIPTION("Minimal Simple Filesystem for Linux Kernel 6.8+");

