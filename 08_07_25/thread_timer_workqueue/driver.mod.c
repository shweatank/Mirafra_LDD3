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
	{ 0x15ba50a6, "jiffies" },
	{ 0x122c3a7e, "_printk" },
	{ 0xc38c83b8, "mod_timer" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0xe2964344, "__wake_up" },
	{ 0x82ee90dc, "timer_delete_sync" },
	{ 0x68bdb9a6, "kthread_stop" },
	{ 0x9fa7184a, "cancel_delayed_work_sync" },
	{ 0x8c03d20c, "destroy_workqueue" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0xe2c17b5d, "__SCT__might_resched" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0x1000e51, "schedule" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x92540fbf, "finish_wait" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x54b1fac6, "__ubsan_handle_load_invalid_value" },
	{ 0x49cd25ed, "alloc_workqueue" },
	{ 0xffeedf6a, "delayed_work_timer_fn" },
	{ 0xc6f46339, "init_timer_key" },
	{ 0x41d3957, "kthread_create_on_node" },
	{ 0x8ef6d02c, "wake_up_process" },
	{ 0xb2fcb56d, "queue_delayed_work_on" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xb2b23fc2, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "00879149819556915F21C8D");
