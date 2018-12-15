#ifndef MESSAGE_SLOT_H
#define MESSAGE_SLOT_H

#include <linux/ioctl.h>

#define MSG_SLOT_CHANNEL _IOW(242,0,unsigned long)/*doesnt matter what the 1st argument 
	sent will contain,only a convention*/

#endif
