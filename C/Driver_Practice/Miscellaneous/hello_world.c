#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/moduleparam.h>
#include<linux/init.h>


MODULE_AUTHOR("Jackal");
MODULE_LICENSE("GPL");

static char* mystring= " ";
static int   mynum= 0;

module_param(mystring, charp, S_IRUGO);
module_param(mynum, int, S_IRUGO);

static int __init init(void)
{
	printk(KERN_ALERT "mystring is %s, mynum is %d\n", mystring, mynum);
	return 0;
}

static void __exit cleanup(void)
{
	printk(KERN_ALERT "Good bye World\n");
}

module_init(init);
module_exit(cleanup);
