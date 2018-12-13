#undef __KERNEL__
#define __KERNEL__

#undef MODULE
#define MODULE

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/slab.h>

#define DEVICE_RANGE_NAME "message_slot"
#define DEVICE_NAME "message_slot"

static int majorNumber;

static int device_open(struct inode* inode,struct file* file){

}


static ssize_t device_read(struct file* file,char __user* buffer,
			size_t length,loff_t* offset){


}

static ssize_t device_write(struct file* file, const char __user* buffer,
			size_t length,loff_t* offset){


}

static long device_ioctl(struct file* file,unsigned int fd,unsigned long request){

}


struct file_operations Fops={
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.ioctl = device_ioctl,

};


static int __init slot_init(void){
	majorNumber = register_chrdev(0,DEVICE_RANGE_NAME,&Fops);
	if(majorNumber<0){
		printk(KERN_ALERT "%s registration failed for %d\n",DEVICE_FILE_NAME,majorNmber);
		return major;
	}
}

static void __exit slot_cleanup(void){
	unregister_chrdev(major,DEVICE_RANGE_NAME);
}

module_init(slot_init);
module_exit(slot_cleanup);
