#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

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



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x60b678d1, "i2c_register_driver" },
	{ 0xe095e43a, "device_destroy" },
	{ 0x4a41ecb3, "class_destroy" },
	{ 0x607587f4, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x1a283f39, "_dev_info" },
	{ 0x464601cd, "i2c_del_driver" },
	{ 0xdcb764ad, "memset" },
	{ 0x4829a47e, "memcpy" },
	{ 0xbc9ac584, "i2c_transfer_buffer_flags" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x5d9d9fd4, "cdev_init" },
	{ 0xcc335c1c, "cdev_add" },
	{ 0xf311fc60, "class_create" },
	{ 0x93ab9e33, "device_create" },
	{ 0x39ff040a, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cpavan,ssd1306-fb");
MODULE_ALIAS("of:N*T*Cpavan,ssd1306-fbC*");

MODULE_INFO(srcversion, "434C3EB76D05C1CDE6BA1D4");
