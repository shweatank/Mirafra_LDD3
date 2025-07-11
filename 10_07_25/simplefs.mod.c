#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const char ____versions[]
__used __section("__versions") =
	"\x14\x00\x00\x00\x15\xea\xcb\x5c"
	"new_inode\0\0\0"
	"\x18\x00\x00\x00\x9f\xa1\x7c\xfe"
	"current_time\0\0\0\0"
	"\x24\x00\x00\x00\xd7\x43\xd5\x39"
	"simple_dir_inode_operations\0"
	"\x14\x00\x00\x00\x4c\x43\xdd\xb0"
	"d_make_root\0"
	"\x18\x00\x00\x00\x52\x38\xc0\x92"
	"d_alloc_name\0\0\0\0"
	"\x10\x00\x00\x00\x2e\xa3\x31\x48"
	"d_add\0\0\0"
	"\x20\x00\x00\x00\xdd\xb7\x9c\x61"
	"simple_read_from_buffer\0"
	"\x20\x00\x00\x00\x64\x43\xcb\x4c"
	"unregister_filesystem\0\0\0"
	"\x20\x00\x00\x00\xd5\xf0\x99\x7b"
	"generic_delete_inode\0\0\0\0"
	"\x18\x00\x00\x00\xbb\xaf\x85\x92"
	"simple_statfs\0\0\0"
	"\x18\x00\x00\x00\x80\xf1\x69\x49"
	"default_llseek\0\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x1c\x00\x00\x00\x2f\x68\x04\xdd"
	"register_filesystem\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x1c\x00\x00\x00\xa3\x94\xa5\x19"
	"kill_litter_super\0\0\0"
	"\x14\x00\x00\x00\x18\xf8\x46\x30"
	"mount_nodev\0"
	"\x18\x00\x00\x00\x76\xf2\x0f\x5e"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "92064829405CA3811FAFA8F");
