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
#include <linux/errno.h>
#include "message_slot.h"


MODULE_LICENSE("GPL");

#define DEVICE_RANGE_NAME "message_slot"/*module name,/def/filename is created with mknod,
		needs to be a different filename for each message_slot we create*/
/*i thought we need to #define DEVICE_FILE_NAME in the code too,but it is done in the bash,
different filename for each message_slot*/
#define MAX_LEN 128
#define MAX_MINORS 256

static int majorNumber;
static int current_minor;
static char **slots;/*the message slots of the device*/
static unsigned long channel_num;/*ioctl sets what channel we want to read/write from/to*/
static int channel_amount[MAX_MINORS];/*total amount of open channels for each minor*/
static int** channel_list;/*show where channels are located in the slots double array
eg: [i] contains 30 means channel 30 is in [i*128]-[i*128+127] in slots*/
static unsigned long channel_location;/*where the channel starts on the slots array*/
static int initiated=0;/*to see if we allocated mem at all*/
static int initiated_minor[MAX_MINORS];/*to see if we allocated mem for specific minor */


static int device_open(struct inode* inode,struct file* file){/*creates device.if needed,
		creates data structure for thespecific message slot that is being opened*/
	printk("invoking device open\n");
	current_minor=iminor(inode);
	if(initiated==0){
		initiated=1;
		channel_list=(int**)kmalloc(MAX_MINORS*sizeof(int*),GFP_KERNEL);/*initialize list of initialized channels for each minor*/
		if(channel_list==NULL)
			return -1;
		slots=(char**)kmalloc(MAX_MINORS*MAX_LEN*sizeof(char),GFP_KERNEL);/*128*sizeof(char)=max size of string in the slot
					initialize space for channels to be initialized to,for each minor*/
		if(slots==NULL)
			return -1;
	}
	return 0; /*success*/
}

int find_size(char* array,unsigned long start){/*return length of string in channel currently*/
	int i,size=0;
	for(i=0;i<MAX_LEN;i++){
		if(array[start+i]!='\0')
			size++;
	}
	printk("size is %d\n",size);
	return size;
}

static ssize_t device_read(struct file* file,char __user* buffer,
			size_t length,loff_t* offset){/*read data fro message slot to user*/
	int i,size;
	printk("invoking device read\n");
	if(!channel_num)
		return -EINVAL;
	if(slots[current_minor][channel_location*MAX_LEN]=='\0')/*channel has no message*/
		return -EWOULDBLOCK;
	size=find_size(slots[current_minor],channel_location*MAX_LEN);
	if(size>length)/*shouldn't happen,in usercode we set length to 128*/
		return -ENOSPC;
	for(i=0;i<size;i++){
		if(put_user(slots[current_minor][i+channel_location*MAX_LEN],&buffer[i])!=0)
			return -EFAULT;
		printk("%c\n",slots[current_minor][i+channel_location*MAX_LEN]);
	}
	return 0;
}

static ssize_t device_write(struct file* file, const char __user* buffer,
			size_t length,loff_t* offset){/*write data to message slot from user*/
	int i;
	printk("invoking device write\n");
	if(!channel_num)/*shouldn't happen,we do ioctl every time*/
		return -EINVAL;
	if(length==0 || length>MAX_LEN)/*length isn't correct*/
		return -EMSGSIZE;
	for(i=0;i<length;i++){
		if(get_user(slots[current_minor][i+channel_location*MAX_LEN],&buffer[i])!=0)/*error happened in get_user*/
			return -EFAULT;
		printk("%c\n",slots[current_minor][i+channel_location*MAX_LEN]);
	}
	if(length<MAX_LEN)
		slots[current_minor][channel_location*MAX_LEN+length]='\0';
	return length;
}

int find_channel(unsigned long channel_id){
	int i;
	for(i=0;i<channel_amount[current_minor];i++){
		if(channel_list[current_minor][i]==channel_id)
			return i;/*is an open channel on the message slot,return location*/
	}
	return -1;/*not an open channel on the message slot*/
}

void init_mem(char* array,int start,int end){/*zero-out the added memory*/
	int i;
	for(i=start;i<end;i++){
		array[i]='\0';
	}
}

static long device_ioctl(struct file* file,unsigned int ioctl_command_id,unsigned long channel_id){
	/*set what channel we want to talk talk to*/
	printk("invoking device ioctl\n");
	if(MSG_SLOT_CHANNEL!=ioctl_command_id)
		return -EINVAL;
	if(channel_id==0)
		return -EINVAL;
	printk("changing channel\n");
	channel_num=channel_id;
	channel_location=find_channel(channel_num);
	if(channel_location==-1){/*channel wasn't found,need to dynamically allocate
			more space for the channels*/
		channel_amount[current_minor]++;
		channel_location=(channel_amount[current_minor]-1);
		if(channel_amount[current_minor]==1){
			initiated_minor[current_minor]=1;
			slots[current_minor]=(char*)kmalloc(channel_amount[current_minor]*MAX_LEN*sizeof(char),GFP_KERNEL);
			if(slots[current_minor]==NULL)
				return -1;
			channel_list[current_minor]=(int*)kmalloc(channel_amount[current_minor]*sizeof(int),GFP_KERNEL);
			if(channel_list[current_minor]==NULL)
				return -1;
		}
		else{
			slots[current_minor]=(char*)krealloc(slots[current_minor],channel_amount[current_minor]*MAX_LEN*sizeof(char),GFP_KERNEL);
			if(slots[current_minor]==NULL)
				return -1;
			channel_list[current_minor]=(int*)krealloc(channel_list[current_minor],channel_amount[current_minor]*sizeof(int),GFP_KERNEL);
			if(channel_list[current_minor]==NULL)
				return -1;
		}
		channel_list[current_minor][channel_amount[current_minor]-1]=channel_num;
		init_mem(slots[current_minor],(channel_amount[current_minor]-1)*MAX_LEN*sizeof(char),channel_amount[current_minor]*MAX_LEN*sizeof(char));
	}
	printk("channel num is %ld,channel location is %ld\n",channel_num,channel_location);
	return 0;
}

static int device_release(struct inode* inode,struct file* file){/*for close i think?*/
	printk("invoking device release\n");
	return 0; /*success*/
}

struct file_operations Fops={
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.unlocked_ioctl = device_ioctl,
	.release = device_release,

};


static int __init slot_init(void){/*create the module*/
	majorNumber = register_chrdev(0,DEVICE_RANGE_NAME,&Fops);/*change nop to &Fops*/
	if(majorNumber<0){/*register_chrdev failed*/
		printk(KERN_ALERT "%s registration failed for %d\n",DEVICE_RANGE_NAME,majorNumber);
		return majorNumber;
	}
	printk(KERN_INFO "message_slot: registered major number %d\n",majorNumber);


	return 0;
}

static void __exit slot_cleanup(void){/*remove the module*/
	int i;
	unregister_chrdev(majorNumber,DEVICE_RANGE_NAME);
	for(i=0;i<MAX_MINORS;i++){
		if(initiated_minor[i]){/*if we allocated space for a minor,we need to free it.*/
			kfree(channel_list[i]);
			kfree(slots[i]);
		}
	}
	kfree(channel_list);
	kfree(slots);
	printk("removing module\n");
}

module_init(slot_init);
module_exit(slot_cleanup);