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
#include <linux/init.h>		/* module_init, module_exit */
#include <linux/slab.h>		/* kmalloc */
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/kmod.h>

#define MYDEV_NAME "mycdrv"

static int numr= 1;
static dev_t dev_first;
static struct cdev *my_cdev;
static struct class *foo_class;

static int mydev_open(struct inode* inode, struct file* filp)
{
	pr_info("Open device %s\n", MYDEV_NAME);
	if(request_module("lab9_3b.ko")) 
		pr_info("Cannot load module lab9_3b\n");
	else
		pr_info("Success loading module lab9_3b\n");
	
	return 0;
}
static ssize_t mydev_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	return count;
}

static const struct file_operations my_fops ={
	.owner= THIS_MODULE,
	.read= mydev_read,
	.open= mydev_open,
};

static void mydev_exit(void)
{
	//Clean up char device
	if (foo_class) {
		device_destroy(foo_class, dev_first);
		class_destroy(foo_class);
	}
	if (my_cdev)
		cdev_del(my_cdev);
	unregister_chrdev_region(dev_first, numr);
	pr_info("\ndevice unregistered\n");
}

static int __init mydev_init(void)
{
	//Obtain major number
	if (alloc_chrdev_region(&dev_first, 0, numr, MYDEV_NAME)) {
		pr_info("Cannot obtain major number\n");
		return -EFAULT;
	}
	pr_info("Success registering %s with MAJOR %d\n", MYDEV_NAME, MAJOR(dev_first)); 

	//Initialize device structure

	//Regitering cdev and make a device node
	my_cdev= cdev_alloc();
	if (!my_cdev) {
		mydev_exit();
		return -ENOMEM;
	}
	cdev_init(my_cdev, &my_fops);
	my_cdev->owner= THIS_MODULE;
	if(cdev_add(my_cdev, dev_first, numr)) {
		mydev_exit();
		return -EFAULT;
	}
	foo_class= class_create(THIS_MODULE, MYDEV_NAME);
	device_create(foo_class, NULL, dev_first, NULL, "%s", MYDEV_NAME);
	pr_info("\nSucceeded in registering device %s\n", MYDEV_NAME);

	return 0;
}

module_init(mydev_init);
module_exit(mydev_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_LICENSE("GPL v2");
