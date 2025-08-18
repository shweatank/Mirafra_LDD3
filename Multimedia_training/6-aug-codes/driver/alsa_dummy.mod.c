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
	{ 0x86411b0, "snd_pcm_new" },
	{ 0xb04f2a1a, "snd_pcm_set_ops" },
	{ 0x38f5b11f, "snd_pcm_lib_preallocate_pages_for_all" },
	{ 0xd0cb4600, "snd_card_register" },
	{ 0xee2ce009, "snd_pcm_lib_ioctl" },
	{ 0xbdfb6dbb, "__fentry__" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x7ae99c74, "snd_card_free" },
	{ 0x122c3a7e, "_printk" },
	{ 0x90bdaeb3, "snd_pcm_lib_free_pages" },
	{ 0x2ce89da9, "snd_pcm_lib_malloc_pages" },
	{ 0xb20b98ab, "snd_card_new" },
	{ 0xc6227e48, "module_layout" },
};

MODULE_INFO(depends, "snd-pcm,snd");


MODULE_INFO(srcversion, "311BB16EBE5661125A3F3DB");
