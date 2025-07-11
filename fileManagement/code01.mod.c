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
	{ 0x1999a631, "kill_litter_super" },
	{ 0x5b8239ca, "__x86_return_thunk" },
	{ 0x96b2515e, "mount_nodev" },
	{ 0x619cb7dd, "simple_read_from_buffer" },
	{ 0xe0280b67, "new_inode" },
	{ 0xc5bbbf60, "current_task" },
	{ 0x6e672ee6, "current_time" },
	{ 0x38699e1b, "simple_dir_inode_operations" },
	{ 0x71cbab8b, "d_make_root" },
	{ 0x7a6aa7e1, "d_alloc_name" },
	{ 0x794f41bf, "d_add" },
	{ 0x65487097, "__x86_indirect_thunk_rax" },
	{ 0xba8fbd64, "_raw_spin_lock" },
	{ 0xb5b54b34, "_raw_spin_unlock" },
	{ 0x55385e2e, "__x86_indirect_thunk_r14" },
	{ 0x20a4e878, "register_filesystem" },
	{ 0xac30ae97, "unregister_filesystem" },
	{ 0xb7aa7543, "generic_delete_inode" },
	{ 0x1e3f1b95, "simple_statfs" },
	{ 0x71eada32, "default_llseek" },
	{ 0x541a6db8, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C1D53E2F14328DC7B9BB13D");
