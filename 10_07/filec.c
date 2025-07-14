#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/pagemap.h>
#include <linux/time64.h>
#include <linux/string.h>
#include <linux/slab.h>

#define SIMPLEFS_MAGIC 0x10032013
#define SIMPLEFS_FILE_NAME "simple.txt"
#define SIMPLEFS_FILE_CONTENT "Hello from kernel file system!\n"

// Read handler
static ssize_t simplefs_read(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    return simple_read_from_buffer(buf, len, ppos, SIMPLEFS_FILE_CONTENT, strlen(SIMPLEFS_FILE_CONTENT));
}

static const struct file_operations simplefs_file_ops = {
    .read = simplefs_read,
    .llseek = default_llseek,
};

// Inode creation helper
static struct inode *simplefs_make_inode(struct super_block *sb, int mode)
{
    struct inode *inode = new_inode(sb);
    if (inode) {
        inode->i_ino = get_next_ino();
        inode->i_sb = sb;
        inode->i_mode = mode;

        struct timespec64 now = current_time(inode);
        inode->__i_ctime = now;
        inode->__i_mtime = now;
        inode->__i_atime = now;
    }
    return inode;
}

// Superblock operations
static const struct super_operations simple_super_ops = {
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

// Fill superblock
static int simplefs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct inode *root_inode, *file_inode;
    struct dentry *file_dentry;

    sb->s_magic = SIMPLEFS_MAGIC;
    sb->s_op = &simple_super_ops;

    root_inode = simplefs_make_inode(sb, S_IFDIR | 0755);
    if (!root_inode)
        return -ENOMEM;
    root_inode->i_op = &simple_dir_inode_operations;
    sb->s_root = d_make_root(root_inode);
    if (!sb->s_root)
        return -ENOMEM;

    file_inode = simplefs_make_inode(sb, S_IFREG | 0444);
    if (!file_inode)
        return -ENOMEM;
    file_inode->i_fop = &simplefs_file_ops;

    file_dentry = d_alloc_name(sb->s_root, SIMPLEFS_FILE_NAME);
    if (!file_dentry)
        return -ENOMEM;

    d_add(file_dentry, file_inode);
    return 0;
}

// Mount handler
static struct dentry *simplefs_mount(struct file_system_type *type, int flags,
                                     const char *dev_name, void *data)
{
    return mount_nodev(type, flags, data, simplefs_fill_super);
}

// Kill superblock
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
MODULE_AUTHOR("rev");
MODULE_DESCRIPTION("Minimal simplefs for Linux 6.8+");

