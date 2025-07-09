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
	{ 0x33c47e0e, "class_create" },
	{ 0xf641a686, "device_create" },
	{ 0x122c3a7e, "_printk" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xc5442e07, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0xa916b694, "strnlen" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x1000e51, "schedule" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x92540fbf, "finish_wait" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xc21bdf9, "device_destroy" },
	{ 0xfa5cdaeb, "class_destroy" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xe2964344, "__wake_up" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xa304a8e0, "cdev_init" },
	{ 0xb51d50, "cdev_add" },
	{ 0xb2b23fc2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "78919EBD19BA2CF0714BE56");
