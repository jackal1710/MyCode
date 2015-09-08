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
#include <linux/interrupt.h>
#include <linux/user.h>
#include <linux/cdev.h>

#define MYDEV_NAME "mycdrv"
#define IRQ_NUM_MAX 50	
#define SIZE_BYTE (IRQ_NUM_MAX * sizeof(int))


static int numr= 1, dev_id= 1710;
static dev_t dev_first;
static char *ramdisk;
static struct cdev *my_cdev;
static struct class *foo_class;
static struct device *foo_device;


static irqreturn_t fool_irqhandler(int irq, void* dev_id)
{
	signed int* irq_list= (signed int*) ramdisk;
	irq_list[irq]++;

	return IRQ_NONE;
}

static void get_irq(void)
{
	int i;
	signed int *irq_list= (signed int*) ramdisk;

	for(i=0; i< IRQ_NUM_MAX; ++i) {
		irq_list[i]= -1;
		if (!request_irq(i, fool_irqhandler, IRQF_SHARED, "foolirq", (void*) &dev_id)) {
			irq_list[i]= 0;
			pr_info("Success register IRQ LINE %d\n", i);
		}
	}
}

static void release_irq(void)
{
	int i;
	signed int *irq_list= (signed int*) ramdisk;

	for(i=0; i< IRQ_NUM_MAX; ++i) {
		pr_info("IRQ Line %d: %d\n", i, irq_list[i]);
		if (irq_list[i] != -1) {
			free_irq(i, &dev_id);
			pr_info("Release IRQ LINE %d\n", i);
		}
	}		
}
static ssize_t mydev_read(struct file *filp, char __user *buf, size_t count, loff_t *offp)
{
	if (count> SIZE_BYTE)
		count= SIZE_BYTE;
	if (copy_to_user(buf, ramdisk, count))
			return -EFAULT;
	
	return count;
}

static const struct file_operations my_fops ={
	.owner= THIS_MODULE,
	.read= mydev_read,
};

static void my_exit(void)
{
	//Release the IRQ line
	release_irq();

	//Clean up char device
	if (foo_class) {
		device_destroy(foo_class, dev_first);
		class_destroy(foo_class);
	}
	if (my_cdev)
		cdev_del(my_cdev);
	unregister_chrdev_region(dev_first, numr);
	if (ramdisk)
		kfree(ramdisk);

	pr_info("\ndevice unregistered\n");
}

static int __init my_init(void)
{
	//Obtain major number
	if (alloc_chrdev_region(&dev_first, 0, numr, MYDEV_NAME)) {
		pr_info("Cannot obtain major number\n");
		return -EFAULT;
	}
	pr_info("Success registering %s with MAJOR %d\n", MYDEV_NAME, MAJOR(dev_first)); 

	//Initialize device structure
	ramdisk= kmalloc(SIZE_BYTE, GFP_KERNEL);
	if (!ramdisk) {
		pr_info("Cannot allocate memory");
		unregister_chrdev_region(dev_first, numr);
		return -ENOMEM;
	}

	//Regitering cdev and make a device node
	my_cdev= cdev_alloc();
	cdev_init(my_cdev, &my_fops);
	my_cdev->owner= THIS_MODULE;
	cdev_add(my_cdev, dev_first, numr);
	foo_class= class_create(THIS_MODULE, MYDEV_NAME);
	foo_device= device_create(foo_class, NULL, dev_first, NULL, "%s", MYDEV_NAME);
	pr_info("\nSucceeded in registering device %s\n", MYDEV_NAME);

	//Register IRQ line
	get_irq();

	return 0;
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Jerry Cooperstein");
MODULE_LICENSE("GPL v2");
