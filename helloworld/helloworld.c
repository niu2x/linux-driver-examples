#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

static char *name = "world";
static int greet_count = 1;

module_param(name, charp, S_IRUGO);
module_param(greet_count, int, S_IRUGO);

static int __init setup(void) {
	int c = greet_count;
	while(c--)
		printk("hello %s\n", name);

	return 0;
}
static void __exit cleanup(void) {
	int c = greet_count;
	while(c--)
		printk("goodbye %s\n", name);
}

module_init(setup);
module_exit(cleanup);