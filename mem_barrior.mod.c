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
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x14\x00\x00\x00\x4b\x8d\xfa\x4d"
	"mutex_lock\0\0"
	"\x1c\x00\x00\x00\x48\x9f\xdb\x88"
	"__check_object_size\0"
	"\x18\x00\x00\x00\xc2\x9c\xc4\x13"
	"_copy_from_user\0"
	"\x14\x00\x00\x00\x2f\x7a\x25\xa6"
	"complete\0\0\0\0"
	"\x18\x00\x00\x00\x38\xf0\x13\x32"
	"mutex_unlock\0\0\0\0"
	"\x18\x00\x00\x00\xc6\x4f\x62\x2a"
	"kthread_stop\0\0\0\0"
	"\x18\x00\x00\x00\x5f\xc1\x06\x3c"
	"device_destroy\0\0"
	"\x18\x00\x00\x00\xff\x34\xfc\x01"
	"class_destroy\0\0\0"
	"\x14\x00\x00\x00\x1a\x27\x1c\xf4"
	"cdev_del\0\0\0\0"
	"\x24\x00\x00\x00\x33\xb3\x91\x60"
	"unregister_chrdev_region\0\0\0\0"
	"\x10\x00\x00\x00\xba\x0c\x7a\x03"
	"kfree\0\0\0"
	"\x1c\x00\x00\x00\x63\xa5\x03\x4c"
	"random_kmalloc_seed\0"
	"\x18\x00\x00\x00\x44\x99\xa9\x37"
	"kmalloc_caches\0\0"
	"\x18\x00\x00\x00\x04\x4f\xe1\x22"
	"kmalloc_trace\0\0\0"
	"\x1c\x00\x00\x00\x2b\x2f\xec\xe3"
	"alloc_chrdev_region\0"
	"\x14\x00\x00\x00\x6b\x47\x85\x31"
	"cdev_init\0\0\0"
	"\x14\x00\x00\x00\x46\xf5\xab\x0a"
	"cdev_add\0\0\0\0"
	"\x18\x00\x00\x00\xa1\xa6\x4f\xc7"
	"class_create\0\0\0\0"
	"\x18\x00\x00\x00\x3a\x46\xee\xf4"
	"device_create\0\0\0"
	"\x20\x00\x00\x00\xba\x90\x58\x47"
	"kthread_create_on_node\0\0"
	"\x18\x00\x00\x00\x20\x3a\x9a\x04"
	"wake_up_process\0"
	"\x1c\x00\x00\x00\xcb\xf6\xfd\xf0"
	"__stack_chk_fail\0\0\0\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x10\x00\x00\x00\xf9\x82\xa4\xf9"
	"msleep\0\0"
	"\x1c\x00\x00\x00\x6e\x64\xf7\xb3"
	"kthread_should_stop\0"
	"\x2c\x00\x00\x00\xe2\xcc\x3b\x2e"
	"wait_for_completion_interruptible\0\0\0"
	"\x18\x00\x00\x00\x65\x83\x85\xfa"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "833B454955CF6F9E47FCAC3");
