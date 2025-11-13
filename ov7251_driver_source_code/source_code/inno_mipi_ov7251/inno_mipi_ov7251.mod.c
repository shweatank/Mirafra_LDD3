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
	{ 0xb063ac3a, "i2c_unregister_device" },
	{ 0x3d3a8372, "v4l2_async_unregister_subdev" },
	{ 0x263f3602, "v4l2_ctrl_handler_free" },
	{ 0xb90015ae, "i2c_register_driver" },
	{ 0x476b165a, "sized_strscpy" },
	{ 0xe6657608, "__v4l2_ctrl_modify_range" },
	{ 0x70384f24, "i2c_transfer" },
	{ 0xc5d5e114, "_dev_warn" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0xcb8314a8, "i2c_del_driver" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x7c9a7371, "clk_prepare" },
	{ 0xb6e6d99d, "clk_disable" },
	{ 0xb077e70a, "clk_unprepare" },
	{ 0x815588a6, "clk_enable" },
	{ 0x36a78de3, "devm_kmalloc" },
	{ 0xc8d7f03e, "i2c_new_dummy_device" },
	{ 0x3bb3b979, "_dev_info" },
	{ 0x4ce71d0c, "v4l2_i2c_subdev_init" },
	{ 0x9aaf43cd, "v4l2_ctrl_handler_init_class" },
	{ 0x75c4b665, "v4l2_ctrl_new_std" },
	{ 0x133d5f87, "v4l2_ctrl_new_std_menu_items" },
	{ 0x9e781eac, "v4l2_ctrl_new_int_menu" },
	{ 0xc38b7ac0, "v4l2_ctrl_handler_setup" },
	{ 0xbbd8f39f, "_dev_err" },
	{ 0xa3732c8f, "media_entity_pads_init" },
	{ 0x28a60742, "__v4l2_async_register_subdev" },
	{ 0xfe548aae, "param_ops_int" },
	{ 0x474e54d2, "module_layout" },
};

MODULE_INFO(depends, "v4l2-async,videodev,mc");

MODULE_ALIAS("of:N*T*Comnivision,inno_mipi_ov7251");
MODULE_ALIAS("of:N*T*Comnivision,inno_mipi_ov7251C*");
MODULE_ALIAS("i2c:inno_mipi_ov7251");

MODULE_INFO(srcversion, "8297D82514E5C2169A57E42");
