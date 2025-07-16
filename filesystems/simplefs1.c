#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/pagemap.h>
#include <linux/time.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/mount.h> // for nop_mnt_idmap

#define SIMPLEFS_MAGIC 0x10032013
#define SIMPLEFS_FILE_NAME "simple.txt"
#define SIMPLEFS_FILE_CONTENT "Hello from kernel file system!\n"

static const struct super_operations simplefs_super_ops;
static const struct inode_operations simplefs_dir_inode_ops;

// File read function
static ssize_t simplefs_read(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    const char *content = SIMPLEFS_FILE_CONTENT;
    size_t content_len = strlen(content);
    return simple_read_from_buffer(buf, len, ppos, content, content_len);
}

static const struct file_operations simplefs_file_ops = {
    .owner = THIS_MODULE,
    .read = simplefs_read,
    .llseek = default_llseek,
};

// Superblock fill function
static int simplefs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct inode *root_inode, *file_inode;
    struct dentry *file_dentry;
    struct timespec64 now;

    sb->s_magic = SIMPLEFS_MAGIC;
    sb->s_op = &simplefs_super_ops;

    ktime_get_real_ts64(&now);

    // Create root inode
    root_inode = new_inode(sb);
    if (!root_inode)
        return -ENOMEM;

    root_inode->i_ino = 1;
    inode_init_owner(&nop_mnt_idmap, root_inode, NULL, S_IFDIR | 0755);
    root_inode->i_atime = root_inode->i_mtime = root_inode->i_ctime = now;
    root_inode->i_op = &simplefs_dir_inode_ops;
    sb->s_root = d_make_root(root_inode);
    if (!sb->s_root)
        return -ENOMEM;

    // Create simple.txt file inode
    file_inode = new_inode(sb);
    if (!file_inode)
        return -ENOMEM;

    file_inode->i_ino = 2;
    inode_init_owner(&nop_mnt_idmap, file_inode, root_inode, S_IFREG | 0444);
    file_inode->i_atime = file_inode->i_mtime = file_inode->i_ctime = now;
    file_inode->i_fop = &simplefs_file_ops;

    file_dentry = d_alloc_name(sb->s_root, SIMPLEFS_FILE_NAME);
    if (!file_dentry)
        return -ENOMEM;

    d_add(file_dentry, file_inode);
    return 0;
}

// Mount function
static struct dentry *simplefs_mount(struct file_system_type *type, int flags,
                                     const char *dev_name, void *data)
{
    return mount_nodev(type, flags, data, simplefs_fill_super);
}

// Superblock cleanup
static void simplefs_kill_super(struct super_block *sb)
{
    kill_litter_super(sb);
}

// Superblock operations
static const struct super_operations simplefs_super_ops = {
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

// Inode operations for directory
static const struct inode_operations simplefs_dir_inode_ops = {
    .lookup = simple_lookup,
};

// Filesystem type registration
static struct file_system_type simplefs_type = {
    .owner = THIS_MODULE,
    .name = "simplefs",
    .mount = simplefs_mount,
    .kill_sb = simplefs_kill_super,
    .fs_flags = FS_USERNS_MOUNT,
};

static int __init simplefs_init(void)
{
    pr_info("SimpleFS: registering...\n");
    return register_filesystem(&simplefs_type);
}

static void __exit simplefs_exit(void)
{
    pr_info("SimpleFS: unregistering...\n");
    unregister_filesystem(&simplefs_type);
}

module_init(simplefs_init);
module_exit(simplefs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant @ TechDhaba, fixed for 6.8+ by ChatGPT");
MODULE_DESCRIPTION("SimpleFS - Minimal Filesystem Kernel 6.8+ Compatible");

