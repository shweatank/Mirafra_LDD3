#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

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
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0xc5bbbf60, "current_task" },
	{ 0x92997ed8, "_printk" },
	{ 0xb3f7646e, "kthread_should_stop" },
	{ 0xf9a482f9, "msleep" },
	{ 0x37a0cba, "kfree" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x5f540977, "kmalloc_caches" },
	{ 0xfa55b3ee, "kmem_cache_alloc_trace" },
	{ 0xac13d953, "kthread_create_on_node" },
	{ 0x87a21cb3, "__ubsan_handle_out_of_bounds" },
	{ 0xd9b11890, "wake_up_process" },
	{ 0xf709cb89, "kthread_stop" },
	{ 0x541a6db8, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "14B02603F4284E7A76D15F4");
