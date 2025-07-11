#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/pagemap.h>
#include <linux/string.h>

#define HELLOFS_MAGIC 0x13131313

static struct inode *hello_make_inode(struct super_block *sb, int mode);

static const char *hello_str = "Hello, TechDhaba!\n";
static const char *hello_name = "hello.txt";

/* File operations */
static ssize_t hello_read_file(struct file *filp, char __user *buf,
                               size_t count, loff_t *offset)
{
    return simple_read_from_buffer(buf, count, offset, hello_str, strlen(hello_str));
}

static struct file_operations hello_file_ops = {
    .read = hello_read_file,
    .llseek = default_llseek,
};

/* Inode operations for file */
static struct inode_operations hello_inode_ops = {
    .getattr = simple_getattr,
};

/* Directory inode operations */
static int hello_iterate(struct file *filp, struct dir_context *ctx)
{
    if (ctx->pos > 0)
        return 0;

    dir_emit_dots(filp, ctx);
    dir_emit(ctx, hello_name, strlen(hello_name),
             1, DT_REG);
    ctx->pos = 1;
    return 0;
}

static struct file_operations hello_dir_ops = {
    .iterate_shared = hello_iterate,
    .llseek = default_llseek,
};

/* Superblock operations */
static struct super_operations hello_s_ops = {
    .statfs = simple_statfs,
    .drop_inode = generic_delete_inode,
};

/* Fill superblock */
static int hello_fill_super(struct super_block *sb, void *data, int silent)
{
    struct inode *root;
    sb->s_magic = HELLOFS_MAGIC;
    sb->s_op = &hello_s_ops;

    root = hello_make_inode(sb, S_IFDIR | 0755);
    if (!root)
        return -ENOMEM;

    root->i_fop = &hello_dir_ops;
    sb->s_root = d_make_root(root);
    if (!sb->s_root)
        return -ENOMEM;

    // Create hello.txt inode
    struct inode *file_inode = hello_make_inode(sb, S_IFREG | 0444);
    if (!file_inode)
        return -ENOMEM;

    file_inode->i_fop = &hello_file_ops;

    struct dentry *dentry = d_alloc_name(sb->s_root, hello_name);
    if (!dentry)
        return -ENOMEM;

    d_add(dentry, file_inode);

    return 0;
}

/* Create inode */
static struct inode *hello_make_inode(struct super_block *sb, int mode)
{
    struct inode *inode = new_inode(sb);
    if (!inode)
        return NULL;

    inode->i_mode = mode;
    inode->i_uid = current_uid();
    inode->i_gid = current_gid();
    inode->i_blocks = 0;
    inode->i_atime = inode->i_mtime = inode->i_ctime = current_time(inode);

    if (S_ISDIR(mode))
        inode->i_op = &simple_dir_inode_operations;

    return inode;
}

/* Mount */
static struct dentry *hello_mount(struct file_system_type *type, int flags,
                                  const char *dev, void *data)
{
    return mount_nodev(type, flags, data, hello_fill_super);
}

static void hello_kill_sb(struct super_block *sb)
{
    kill_litter_super(sb);
}

static struct file_system_type hello_fs_type = {
    .owner = THIS_MODULE,
    .name = "hello_fs",
    .mount = hello_mount,
    .kill_sb = hello_kill_sb,
};

/* Module init and exit */
static int __init hello_init(void)
{
    return register_filesystem(&hello_fs_type);
}

static void __exit hello_exit(void)
{
    unregister_filesystem(&hello_fs_type);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Simple Hello File System in Kernel Space");
