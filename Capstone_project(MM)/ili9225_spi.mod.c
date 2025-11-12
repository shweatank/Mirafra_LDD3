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
	{ 0xafbeba1c, "__spi_register_driver" },
	{ 0x122c3a7e, "_printk" },
	{ 0xe095e43a, "device_destroy" },
	{ 0x4a41ecb3, "class_destroy" },
	{ 0x607587f4, "cdev_del" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xd51bf3d7, "driver_unregister" },
	{ 0xdcb764ad, "memset" },
	{ 0x9c771906, "spi_sync" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xdc6ffbdc, "gpiod_set_value_cansleep" },
	{ 0x52c5c991, "__kmalloc_noprof" },
	{ 0x37a0cba, "kfree" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0xf9a482f9, "msleep" },
	{ 0xc3055d20, "usleep_range_state" },
	{ 0xfa61d21, "devm_kmalloc" },
	{ 0xe2e7af6c, "device_property_read_u32_array" },
	{ 0x41e0584a, "spi_setup" },
	{ 0x1a283f39, "_dev_info" },
	{ 0x832481e, "devm_gpiod_get" },
	{ 0xcb944b71, "gpiod_set_value" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x5d9d9fd4, "cdev_init" },
	{ 0xcc335c1c, "cdev_add" },
	{ 0xf311fc60, "class_create" },
	{ 0x93ab9e33, "device_create" },
	{ 0xf810f451, "_dev_err" },
	{ 0x39ff040a, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Cilitek,ili9225");
MODULE_ALIAS("of:N*T*Cilitek,ili9225C*");

MODULE_INFO(srcversion, "EAAF0D375E640E4AA54947B");
