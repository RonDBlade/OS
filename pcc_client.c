#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>



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
	unsigned int temp,amount_read,total_read=0,C=0,temp2=0,temp3=0,amount_sent,total_sent=0;
	int randfd,sockfd;
	char* temparr;
	char temparr2[1024];
	struct sockaddr_in serv_addr;
	/*creating tcp connection code here*/
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
	while(total_read<temp){/*read from the /dev/urandom*/
		amount_read=read(randfd,mymessage+total_read,temp-total_read);
		if(amount_read<0){
			printf("ERROR in read(): %s\n",strerror(errno));
			exit(1);
		}
		total_read+=amount_read;
	}
	printf("%s\n",mymessage);
	while(total_sent<temp){/*write to server and read the answer*/
		if(temp-total_sent<1024)/*control how much we send so we won't miss data,as server reads 1024 each time*/
			temp2=temp-total_sent;/*set how much we will send*/
		else
			temp2=1024;
		printf("we want to send %d right now\n",temp2);
		if(temp<1024){
			memset(temparr2,0,1024);
			memcpy(temparr2,&mymessage[total_sent],temp2);
			printf("the string we are going to send is %s should be identical to the first thing we printed\n",temparr2);
			amount_sent=write(sockfd,temparr2,1024);
		}
		else{
			amount_sent=write(sockfd,mymessage+total_sent,1024);/*writes some of the chars to the server*/
		}
		if(amount_sent<0){/*error occured*/
			printf("ERROR in write(): %s\n",strerror(errno));
			exit(1);
		}
		else if(amount_sent==0)/*shouldn't happen*/
			break;
		printf("we sent to the server %d\n",amount_sent);
		total_sent+=amount_sent;/*increase the total of how much we sent to server,offset increase as well*/
		temparr=(char*)calloc(5,sizeof(char));/*we send max 1024,so answer will take 4 bits+1 for \0*/
		temp3=0;
		while(1){/*from what we did sent right now,find how many were printable*/
			temp2=read(sockfd,temparr+temp3,4-temp3);/*read info from server*/
			if(temp2<0){
				printf("ERROR in read(): %s\n",strerror(errno));
				exit(1);
			}
			else if(temp2==0)/*server finished writing info*/
				break;
			temp3+=temp2;/*offset increase+how much to read decrease*/
			printf("we read %d\n",temp2);
			if(temp3==4)
				break;
		}
		temp2=string_to_int(temparr);/*convert the string the server sent to int and increase counter*/
		printf("the answer the server sent us for this chunk is %d\n",temp2);
		C+=temp2;
		free(temparr);
	}
	printf("# of printable characters: %u\n", C);
	exit(0);
}
