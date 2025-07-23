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
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x128d9757, "_dev_info" },
	{ 0x122c3a7e, "_printk" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xa728d6a5, "devm_kmalloc" },
	{ 0x703a6e9e, "devm_ioremap" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xa304a8e0, "cdev_init" },
	{ 0xb51d50, "cdev_add" },
	{ 0x33c47e0e, "class_create" },
	{ 0xf641a686, "device_create" },
	{ 0xf1be6b0e, "_dev_err" },
	{ 0x2cd5d0e1, "platform_driver_unregister" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x9e07cb93, "__platform_driver_register" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xc21bdf9, "device_destroy" },
	{ 0xfa5cdaeb, "class_destroy" },
	{ 0xc5442e07, "cdev_del" },
	{ 0xb2b23fc2, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Crpi,uart-driver");
MODULE_ALIAS("of:N*T*Crpi,uart-driverC*");

MODULE_INFO(srcversion, "FD69F9CAA4A1AA6C53013BB");
