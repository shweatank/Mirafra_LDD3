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



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x5ccbea15, "new_inode" },
	{ 0xfe7ca19f, "current_time" },
	{ 0x39d543d7, "simple_dir_inode_operations" },
	{ 0xb0dd434c, "d_make_root" },
	{ 0x92c03852, "d_alloc_name" },
	{ 0x4831a32e, "d_add" },
	{ 0x619cb7dd, "simple_read_from_buffer" },
	{ 0x4ccb4364, "unregister_filesystem" },
	{ 0x4969f180, "default_llseek" },
	{ 0x7b99f0d5, "generic_delete_inode" },
	{ 0x9285afbb, "simple_statfs" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xdd04682f, "register_filesystem" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x19a594a3, "kill_litter_super" },
	{ 0x3046f818, "mount_nodev" },
	{ 0xb2b23fc2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "EC59A67A1351EDE4D561803");
