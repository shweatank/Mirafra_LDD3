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
	{ 0x14d1e1b0, "i2c_unregister_device" },
	{ 0x223b9bdb, "v4l2_async_unregister_subdev" },
	{ 0xf0a96f95, "v4l2_ctrl_handler_free" },
	{ 0x60b678d1, "i2c_register_driver" },
	{ 0x476b165a, "sized_strscpy" },
	{ 0xfeaa9dab, "__v4l2_ctrl_modify_range" },
	{ 0x75b7a3fe, "i2c_transfer" },
	{ 0xfb077e23, "_dev_warn" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x464601cd, "i2c_del_driver" },
	{ 0x7c9a7371, "clk_prepare" },
	{ 0xb6e6d99d, "clk_disable" },
	{ 0xb077e70a, "clk_unprepare" },
	{ 0x815588a6, "clk_enable" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0xfa61d21, "devm_kmalloc" },
	{ 0x728af5b0, "i2c_new_dummy_device" },
	{ 0x1a283f39, "_dev_info" },
	{ 0x1c8c9f72, "v4l2_i2c_subdev_init" },
	{ 0x183d6ce5, "v4l2_ctrl_handler_init_class" },
	{ 0xdae0d8b, "v4l2_ctrl_new_std" },
	{ 0xe92de946, "v4l2_ctrl_new_std_menu_items" },
	{ 0x61491381, "v4l2_ctrl_new_int_menu" },
	{ 0x7b396170, "v4l2_ctrl_handler_setup" },
	{ 0x76345776, "media_entity_pads_init" },
	{ 0x85d4d69e, "__v4l2_async_register_subdev" },
	{ 0xf810f451, "_dev_err" },
	{ 0xdc43cf22, "param_ops_int" },
	{ 0x39ff040a, "module_layout" },
};

MODULE_INFO(depends, "v4l2-async,videodev,mc");

MODULE_ALIAS("of:N*T*Comnivision,inno_mipi_ov7251");
MODULE_ALIAS("of:N*T*Comnivision,inno_mipi_ov7251C*");
MODULE_ALIAS("i2c:inno_mipi_ov7251");

MODULE_INFO(srcversion, "8B1480D628F7996C6B7E2C0");
