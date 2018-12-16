#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <linux/ioctl.h>

#define MAJOR_NUM 244
#define MSG_SLOT_CHANNEL _IOW(MAJOR_NUM,0,unsigned long)/*doesnt matter what the 1st argument 
	sent will contain,only a convention*/

#endif
