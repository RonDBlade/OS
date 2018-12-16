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
	int fd,val,channel,i;
	char buffer[129];
	for(i=0;i<129;i++){/*zero out the output array*/
		buffer[i]='\0';
	}
	if(argc!=3){/*should be exactly 3,2 for actual args,1 for program name*/
		printf("Not the right amount of arguments passed for the reader\n");
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
	val=read(fd,buffer,128);
	if(val<0){
		printf("Error reading from channel: %s\n",strerror(errno));
		return -1;
	}
	close(fd);
	printf("Read successful from %s channel %d\n",argv[1],channel);
	printf("message was: %s\n",buffer);
	return 0;
}
