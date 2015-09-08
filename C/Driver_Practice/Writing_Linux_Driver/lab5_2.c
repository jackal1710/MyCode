/* **************** LDD:2.0 s_04/sample_driver.c **************** */
/*
 * The code herein is: Copyright Jerry Cooperstein, 2012
 *
 * This Copyright is retained for the purpose of protecting free
 * redistribution of source.
 *
 *     URL:    http://www.coopj.com
 *     email:  coop@coopj.com
 *
 * The primary maintainer for this code is Jerry Cooperstein
 * The CONTRIBUTORS file (distributed with this
 * file) lists those known to have contributed to the source.
 *
 * This code is distributed under Version 2 of the GNU General Public
 * License, which you should have received with the source.
 *
 */
/* 
Sample Character Driver 
@*/

#include <linux/module.h>	/* for modules */
#include <linux/fs.h>		/* file_operations */
#include <linux/uaccess.h>	/* copy_(to,from)_user */
#include <linux/init.h>		/* module_init, module_exit */
#include <linux/slab.h>		/* kmalloc */
#include <linux/cdev.h>		/* cdev utilities */
#include <linux/device.h>
#include <linux/list.h>

#define MYDEV_NAME "mycdrv"

static dev_t first;
static unsigned int my_minor= 0;
static unsigned int count = 1;
static struct cdev *my_cdev;
static struct class* foo_class;
static struct device *foo_device;

static int mycdrv_open(struct inode *inode, struct file *file)
{
	struct module *mod;
	struct list_head *list;
	mod= THIS_MODULE;
	list= &mod->list;
	list= list->prev;
	pr_info(" OPENING device: %s\n", MYDEV_NAME);
	pr_info( "Major: %d, Minor: %d \n", imajor(inode), iminor(inode));

	list_for_each_entry(mod, list, list) {
		pr_info("Tants value of %s: %d\n", mod->name, mod->taints);
	}

	return 0;
}

static int mycdrv_release(struct inode *inode, struct file *file)
{
	pr_info(" CLOSING device: %s:\n\n", MYDEV_NAME);

	return 0;
}

static ssize_t
mycdrv_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	return 0;
}

static ssize_t
mycdrv_write(struct file *file, const char __user * buf, size_t lbuf,
	     loff_t * ppos)
{
	return 0;
}

static loff_t mycdrv_llseek(struct file *filp, loff_t off, int whence)
{
	return 0;
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_read,
	.write = mycdrv_write,
	.open = mycdrv_open,
	.release = mycdrv_release,
	.llseek= mycdrv_llseek,
};

 

static void my_exit(void)
{
	device_destroy(foo_class, first);
	class_destroy(foo_class);
	cdev_del(my_cdev);
	unregister_chrdev_region(first, count);
	pr_info("\ndevice unregistered\n");
}

static int __init my_init(void)
{
	if (alloc_chrdev_region(&first, my_minor, count, MYDEV_NAME))
		pr_info("Cannot obtain major number\n");
	
	my_cdev = cdev_alloc();
	if (!my_cdev)
		unregister_chrdev_region(first, count);

	cdev_init(my_cdev, &mycdrv_fops);
	if(cdev_add(my_cdev, first, count)) {
		my_exit();
		return -1;
	}
	pr_info("\nSucceeded in registering character device %s\n", MYDEV_NAME);

	foo_class= class_create(THIS_MODULE, "myclass");
	foo_device= device_create(foo_class, NULL, first, NULL, "%s", MYDEV_NAME);
	return 0;
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_LICENSE("GPL v2");
