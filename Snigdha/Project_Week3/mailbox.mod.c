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
	{ 0xa916b694, "strnlen" },
	{ 0x6b10bee1, "_copy_to_user" },
	{ 0x3213f038, "mutex_unlock" },
	{ 0xcf2a6966, "up" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x1000e51, "schedule" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x92540fbf, "finish_wait" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xa304a8e0, "cdev_init" },
	{ 0xb51d50, "cdev_add" },
	{ 0x33c47e0e, "class_create" },
	{ 0xf641a686, "device_create" },
	{ 0x8e7bdec2, "proc_create" },
	{ 0xf94be466, "kernel_kobj" },
	{ 0x64699b6a, "kobject_create_and_add" },
	{ 0x951843ea, "sysfs_create_file_ns" },
	{ 0x49cd25ed, "alloc_workqueue" },
	{ 0x9166fc03, "__flush_workqueue" },
	{ 0x8c03d20c, "destroy_workqueue" },
	{ 0xc21bdf9, "device_destroy" },
	{ 0xfa5cdaeb, "class_destroy" },
	{ 0xc5442e07, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x30eb19e, "remove_proc_entry" },
	{ 0x6465cd4e, "sysfs_remove_file_ns" },
	{ 0x73d79c6c, "kobject_put" },
	{ 0x148653, "vsnprintf" },
	{ 0xf9a482f9, "msleep" },
	{ 0x656e4a6e, "snprintf" },
	{ 0xe2964344, "__wake_up" },
	{ 0xb42ee1dd, "seq_read" },
	{ 0x736dd663, "single_release" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x31264a9f, "single_open" },
	{ 0x7ac144ac, "seq_printf" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x122c3a7e, "_printk" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xc5b6f236, "queue_work_on" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0x6626afca, "down" },
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0x4dfa8d4b, "mutex_lock" },
	{ 0xb2b23fc2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "7B5AAE5CC15EC15C6F0B293");
