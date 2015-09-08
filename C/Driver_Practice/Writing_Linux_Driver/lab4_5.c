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

#define MYDEV_NAME "mycdrv"
#define ramdisk_size (size_t) (16*PAGE_SIZE)

static dev_t first;
static unsigned int count = 1;
static int my_major = 700, my_minor = 0;
static struct cdev *my_cdev;
static struct class* foo_class;
static struct device *foo_device;

static int mycdrv_open(struct inode *inode, struct file *file)
{
	pr_info(" OPENING device: %s:\n\n", MYDEV_NAME);
	pr_info(" The current reference number is: %lu\n", module_refcount(THIS_MODULE));
	pr_info( "Major: %d, Minor: %d \n", imajor(inode), iminor(inode));

	if (!file->private_data)
		file->private_data= kmalloc(ramdisk_size, GFP_KERNEL);
	if (!file->private_data)
		return -ENOMEM;
	
	return 0;
}

static int mycdrv_release(struct inode *inode, struct file *file)
{
	pr_info(" CLOSING device: %s:\n\n", MYDEV_NAME);
	if (file->private_data) {
		memset( file->private_data, 0, 2);
		kfree(file->private_data);
	}

	return 0;
}

static ssize_t
mycdrv_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	int nbytes;
	char *data= (char*) file->private_data;
	if ((lbuf + *ppos) > ramdisk_size) {
		pr_info("trying to read past end of device,"
			"aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_to_user(buf, data + *ppos, lbuf);
	*ppos += nbytes;
	pr_info("\n READING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}

static ssize_t
mycdrv_write(struct file *file, const char __user * buf, size_t lbuf,
	     loff_t * ppos)
{
	int nbytes;
	char *data= (char*) file->private_data;
	if ((lbuf + *ppos) > ramdisk_size) {
		pr_info("trying to read past end of device,"
			"aborting because this is just a stub!\n");
		return 0;
	}
	nbytes = lbuf - copy_from_user(data + *ppos, buf, lbuf);
	*ppos += nbytes;
	pr_info("\n WRITING function, nbytes=%d, pos=%d\n", nbytes, (int)*ppos);
	return nbytes;
}

static loff_t mycdrv_llseek(struct file *filp, loff_t off, int whence)
{
	loff_t pos;

	switch (whence) {
		case SEEK_SET:
			pos= off;
			break;
		case SEEK_CUR:
			pos= filp->f_pos + off;
			break;
		case SEEK_END:
			pos= ramdisk_size + off;
			break;
		default:
			return -EINVAL;
	}

	if (pos < 0) return -EINVAL;
	pos= (pos > ramdisk_size) ? ramdisk_size : pos;
	pr_info("Seeking to %ld\n", (long)pos);
	filp->f_pos= pos;
	return pos;
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
