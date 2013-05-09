#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x68d372d2, "module_layout" },
	{ 0x8d15f646, "debugfs_remove_recursive" },
	{ 0x79e675fb, "debugfs_create_x64" },
	{ 0xfe8fb7c8, "debugfs_create_u64" },
	{ 0xdb372a34, "debugfs_create_file" },
	{ 0xf7f53ca0, "debugfs_create_dir" },
	{ 0x44366cfc, "simple_write_to_buffer" },
	{ 0x528c709d, "simple_read_from_buffer" },
	{ 0x50eedeb8, "printk" },
	{ 0xb4390f9a, "mcount" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "7761B00B4EC8A0566D52B49");
