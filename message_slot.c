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

MODULE_LICENSE("GPL");

#define DEVICE_RANGE_NAME "message_slot"
#define DEVICE_NAME "message_slot"
#define DEVICE_FILE_NAME "message_slot"

static int majorNumber;

/*
static int device_open(struct inode* inode,struct file* file){

}


static ssize_t device_read(struct file* file,char __user* buffer,
			size_t length,loff_t* offset){


}

static ssize_t device_write(struct file* file, const char __user* buffer,
			size_t length,loff_t* offset){


}

static long device_ioctl(struct file* file,unsigned int ioctl_command_id,unsigned long param){

}

*/
struct file_operations Fops={
	/*.read = device_read,
	.write = device_write,
	.open = device_open,
	.unlocked_ioctl = device_ioctl,
*/
};


static int __init slot_init(void){
	majorNumber = register_chrdev(0,DEVICE_RANGE_NAME,&Fops);/*change nop to &Fops*/
	if(majorNumber<0){/*register_chrdev failed*/
		printk(KERN_ALERT "%s registration failed for %d\n",DEVICE_FILE_NAME,majorNumber);
		return majorNumber;
	}
	printk(KERN_INFO "message_slot: registered major number %d\n",majorNumber);


	return 0;
}

static void __exit slot_cleanup(void){
	unregister_chrdev(majorNumber,DEVICE_RANGE_NAME);
	printk(KERN_ALERT "cleaning up the driver i think lmao\n");
}

module_init(slot_init);
module_exit(slot_cleanup);
