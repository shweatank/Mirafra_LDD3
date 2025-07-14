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
	{ 0xb51d50, "cdev_add" },
	{ 0x33c47e0e, "class_create" },
	{ 0xf641a686, "device_create" },
	{ 0x49cd25ed, "alloc_workqueue" },
	{ 0xd8d16d3a, "input_register_handler" },
	{ 0x122c3a7e, "_printk" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x9ec6ca96, "ktime_get_real_ts64" },
	{ 0x4c03a563, "random_kmalloc_seed" },
	{ 0x29ac0981, "kmalloc_caches" },
	{ 0xdc327b3c, "kmalloc_trace" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xe2964344, "__wake_up" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x9166fc03, "__flush_workqueue" },
	{ 0x8c03d20c, "destroy_workqueue" },
	{ 0xc21bdf9, "device_destroy" },
	{ 0xfa5cdaeb, "class_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xc5442e07, "cdev_del" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0x36155840, "input_register_handle" },
	{ 0xa92cf7e8, "input_open_device" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0x6626afca, "down" },
	{ 0xcf2a6966, "up" },
	{ 0xa916b694, "strnlen" },
	{ 0x754d539c, "strlen" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0x69acdf38, "memcpy" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x798cf223, "input_unregister_handle" },
	{ 0x861a4456, "input_close_device" },
	{ 0x37a0cba, "kfree" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xa304a8e0, "cdev_init" },
	{ 0xb2b23fc2, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("input:b*v*p*e*-e*k*r*a*m*l*s*f*w*");

MODULE_INFO(srcversion, "70B2C9092D492DAC1221E53");
