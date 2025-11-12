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
	{ 0xed6af888, "__platform_driver_register" },
	{ 0x122c3a7e, "_printk" },
	{ 0xa223bac6, "platform_driver_unregister" },
	{ 0x838be74d, "platform_get_resource" },
	{ 0x51013a01, "devm_ioremap_resource" },
	{ 0x39ff040a, "module_layout" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("of:N*T*Ccustom,uart_demo");
MODULE_ALIAS("of:N*T*Ccustom,uart_demoC*");

MODULE_INFO(srcversion, "01A29FC14F536418CC434ED");
