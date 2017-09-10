#include <linux/module.h>

static int __init insert_hello(void)
{
printk(KERN_INFO "Hello World\n");
return 0;
}

static void __exit remove_hello(void)
{
printk(KERN_DEBUG "Hello World removed\n");
}


module_init(insert_hello)
module_exit(remove_hello)

MODULE_LICENSE("GPL");


