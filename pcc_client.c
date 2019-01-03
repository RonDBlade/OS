#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

unsigned int string_to_int(char* thread_count){/*translate the argument which specifies how many threads we want to use to an int*/
	int i;
	unsigned int count=0;
	for(i=0;i<strlen(thread_count);i++){
		count*=10;
		count+=thread_count[i]-'0';
	}
	return count;
}

int main(int argc,char** argv){
	unsigned int temp;
	int ret,randfd,sockfd;
	struct sockaddr_in serv_addr;
	/*create tcp connection code here*/
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0){/*create the socket for the transmissions*/
		printf("ERROR in socket(): %s\n",strerror(errno));
		exit(1);
	}
	temp=string_to_int(argv[2]);/*the port we want to connect to*/
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;/*to tell the server that we are doing TCP*/
	serv_addr.sin_port = htons(temp);/*to tell the server what port we are gonna connect to*/
	serv_addr.sin_addr.s_addr= inet_addr(argv[1]);/*the address of the server*/
	if(connect(sockfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr))<0){
		printf("ERROR in connect(): %s\n",strerror(errno));
		exit(1);
	}

	temp=string_to_int(argv[3]);/*the number of chars we want to read from /dev/urandom*/
	char mymessage[temp+1];/*1 more for '\0'*/
	memset(mymessage,0,temp+1);
	char* address="/dev/urandom";
	/*address="testfile"; file for testing reading,delete when finished*/
	randfd=open(address,O_RDONLY);
	if(randfd<0){
		printf("ERROR in open(): %s\n",strerror(errno));
		exit(1);
	}
	ret=read(randfd,mymessage,temp);
	if(ret<0){
		printf("ERROR in read(): %s\n",strerror(errno));
		exit(1);
	}
	printf("%s\n",mymessage);
	exit(0);
}
