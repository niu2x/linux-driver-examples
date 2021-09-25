#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

static dev_t devno;
#define dev_count 256

#define device_capacity 4096
struct device_t {
	struct cdev cdev;
	char data[device_capacity];
	int64_t size;
	struct semaphore sem;
};

static struct device_t device;

static int memory_device_open(struct inode *inode, struct file *filp) {
	struct device_t *dev = container_of(inode->i_cdev, struct device_t, cdev);
	filp->private_data = dev;
	if( (filp->f_flags & O_ACCMODE) == O_WRONLY ){
		// if(down_interruptible(&(dev->sem))) {
		// 	return -ERESTARTSYS;
		// }
		dev->size = 0;
		// up(&(dev->sem));
	}
	return 0;
}

static int memory_device_release(struct inode *inode, struct file *filp){
	return 0;
}

static ssize_t memory_device_read(struct file *filp, char __user *buff, size_t count, loff_t *offp) {
	struct device_t *dev = filp->private_data;
	int err;
	
	int64_t rest = dev->size - *offp;

	// if(down_interruptible(&(dev->sem))) {
	// 	return -ERESTARTSYS;
	// }


	if(count > rest)
		count = rest;

	if(count > 0){
		err = copy_to_user(buff, dev->data + *offp, count);
		if(!err){
			*offp += count;
			return count;
		}
	}

	// up(&(dev->sem));
	return 0;
}

static ssize_t memory_device_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp) {
	
	struct device_t *dev = filp->private_data;
	int err = 0;
	
	if(dev->size >= device_capacity)
		return - ERESTARTSYS;



	int64_t rest;
	// size_t ori_count = count;

	rest = device_capacity - *offp;

	// if(down_interruptible(&(dev->sem))) {
	// 	return -ERESTARTSYS;
	// }

	if(count > rest)
		count = rest;

	// if(count > 0){
	err = copy_from_user(dev->data, buff, count);
	if(err){
		return - ERESTARTSYS;
	}

	dev->size += count;
	*offp += count;
	// 	if(err > 0)
			// *offp += err;
	// 	if(*offp > dev->size)
	// 		dev->size = *offp;
	// 	if(err >= 0)
	// 		return err;
	// 	else{
	// 		return ori_count;
	// 	}
	// }
	// else{
	// 	return ori_count;
	// }

	// up(&(dev->sem));

	return count;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = memory_device_read,
	.write = memory_device_write,
	.open = memory_device_open,
	.release = memory_device_release,
};

static int __init setup(void) {
	int err = 0;
	err = alloc_chrdev_region(&devno, 0, dev_count, "memory-device");
	if(err){
		printk("memory-device: setup alloc_chrdev_region failed\n");
		return err;
	}

	cdev_init(&device.cdev, &fops);
	device.cdev.owner = THIS_MODULE;
	device.size = 0;
	err = cdev_add(&device.cdev, devno, 1);
	if(err){
		unregister_chrdev_region(devno, dev_count);
		printk("memory-device: setup cdev_add failed\n");
		return err;
	}
	return 0;
}

static void __exit cleanup(void) {
	cdev_del(&device.cdev);
	unregister_chrdev_region(devno, dev_count);
}

module_init(setup);
module_exit(cleanup);