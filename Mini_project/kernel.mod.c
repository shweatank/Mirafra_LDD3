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
	"\x28\x00\x00\x00\xb3\x1c\xa2\x87"
	"__ubsan_handle_out_of_bounds\0\0\0\0"
	"\x14\x00\x00\x00\x06\x72\xb3\xec"
	"filp_open\0\0\0"
	"\x14\x00\x00\x00\x41\x79\xd4\xc4"
	"kernel_read\0"
	"\x14\x00\x00\x00\x8c\x68\x41\x0b"
	"filp_close\0\0"
	"\x10\x00\x00\x00\xe6\x6e\xab\xbc"
	"sscanf\0\0"
	"\x10\x00\x00\x00\x7e\x3a\x2c\x12"
	"_printk\0"
	"\x1c\x00\x00\x00\xcb\xf6\xfd\xf0"
	"__stack_chk_fail\0\0\0\0"
	"\x10\x00\x00\x00\xc5\x8f\x57\xfb"
	"memset\0\0"
	"\x14\x00\x00\x00\xc2\xde\x7b\x8e"
	"proc_create\0"
	"\x18\x00\x00\x00\x5a\xc8\x64\x23"
	"tasklet_init\0\0\0\0"
	"\x18\x00\x00\x00\xed\x25\xcd\x49"
	"alloc_workqueue\0"
	"\x20\x00\x00\x00\x57\x39\x1d\x04"
	"kthread_create_on_node\0\0"
	"\x18\x00\x00\x00\x2c\xd0\xf6\x8e"
	"wake_up_process\0"
	"\x20\x00\x00\x00\x8e\x83\xd5\x92"
	"request_threaded_irq\0\0\0\0"
	"\x18\x00\x00\x00\xa6\xb9\xbd\x68"
	"kthread_stop\0\0\0\0"
	"\x1c\x00\x00\x00\x0c\xd2\x03\x8c"
	"destroy_workqueue\0\0\0"
	"\x14\x00\x00\x00\xbb\x98\x3b\x46"
	"proc_remove\0"
	"\x14\x00\x00\x00\x3b\x4a\x51\xc1"
	"free_irq\0\0\0\0"
	"\x18\x00\x00\x00\x4e\xc7\xa3\x0e"
	"tasklet_kill\0\0\0\0"
	"\x1c\x00\x00\x00\x03\xfc\x66\x91"
	"__flush_workqueue\0\0\0"
	"\x1c\x00\x00\x00\x96\xca\xc6\x9e"
	"ktime_get_real_ts64\0"
	"\x2c\x00\x00\x00\xc6\xfa\xb1\x54"
	"__ubsan_handle_load_invalid_value\0\0\0"
	"\x1c\x00\x00\x00\xac\xb8\x2a\x9d"
	"__tasklet_schedule\0\0"
	"\x14\x00\x00\x00\xdd\xe1\x2e\xb4"
	"seq_read\0\0\0\0"
	"\x14\x00\x00\x00\x76\x02\xcd\x2b"
	"seq_lseek\0\0\0"
	"\x18\x00\x00\x00\x63\xd6\x6d\x73"
	"single_release\0\0"
	"\x14\x00\x00\x00\xbb\x6d\xfb\xbd"
	"__fentry__\0\0"
	"\x10\x00\x00\x00\xf9\x82\xa4\xf9"
	"msleep\0\0"
	"\x1c\x00\x00\x00\x6e\x64\xf7\xb3"
	"kthread_should_stop\0"
	"\x1c\x00\x00\x00\xca\x39\x82\x5b"
	"__x86_return_thunk\0\0"
	"\x18\x00\x00\x00\x36\xf2\xb6\xc5"
	"queue_work_on\0\0\0"
	"\x14\x00\x00\x00\x9f\x4a\x26\x31"
	"single_open\0"
	"\x14\x00\x00\x00\xac\x44\xc1\x7a"
	"seq_printf\0\0"
	"\x24\x00\x00\x00\x70\xce\x5c\xd3"
	"_raw_spin_unlock_irqrestore\0"
	"\x20\x00\x00\x00\x0b\x05\xdb\x34"
	"_raw_spin_lock_irqsave\0\0"
	"\x18\x00\x00\x00\x76\xf2\x0f\x5e"
	"module_layout\0\0\0"
	"\x00\x00\x00\x00\x00\x00\x00\x00";

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "8FEDCE01D789A2060636D58");
