#include "message_slot.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int string_to_int(char* number){
	int i,sum=0;
	for(i=0;i<strlen(number);i++){
		sum*=10;
		sum+=number[i]-'0';
	}
	return sum;
}

int main(int argc,char** argv){
	int fd,val,channel;
	if(argc!=4){/*should be exactly 4*/
		printf("Not the right amount of arguments passed for the sender\n");
		return -1;
	}
	fd=open(argv[1],O_RDWR);
	if(fd<0){
		printf("Error opening file: %s\n",strerror(errno));
		return -1;
	}
	channel=string_to_int(argv[2]);
	val=ioctl(fd,MSG_SLOT_CHANNEL,channel);
	if(val<0){
		printf("Error changing channel: %s\n",strerror(errno));
		return -1;
	}
	val=write(fd,argv[3],strlen(argv[3]));
	if(val<0){
		printf("Error writing to channel: %s\n",strerror(errno));
		return -1;
	}
	close(fd);
	printf("write successful to %s channel %d\n",argv[1],channel);
	return 0;
}
