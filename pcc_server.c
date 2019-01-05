#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>


int keepgoing=1;

unsigned int string_to_int(char* thread_count){/*translate the argument which specifies how many threads we want to use to an int*/
	int i;
	unsigned int count=0;
	for(i=0;i<strlen(thread_count);i++){
		count*=10;
		count+=thread_count[i]-'0';
	}
	return count;
}

void printprintable(unsigned int* array){
	int i,c=32;
	for(i=0;i<95;i++){
		printf("char '%c' : %u times\n",c,array[i]);
		c++;
	}
}

void intHandler(){
	keepgoing=0;
}

int main(int argc,char** argv){
	int listenfd,connfd,onfile;
	unsigned int temp,amount_read,i,count=0,amount_sent,length,temp3=0;
	struct sockaddr_in serv_addr;
	struct sigaction sigact;
	char temparr[1024],temparr2[5];
	char* str;
	unsigned int pcc_total[95];/*init counter struct*/
	memset(pcc_total,0,95*sizeof(unsigned int));
	memset(temparr,0,1024*sizeof(char));
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0){
		printf("ERROR in socker(): %s\n",strerror(errno));
		exit(1);
	}
	temp=string_to_int(argv[1]);
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;/*set server parameters*/
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(temp);
	if(bind(listenfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr))!=0){
		printf("ERROR in bind(): %s\n",strerror(errno));
		exit(1);
	}
	if(listen(listenfd,10)!=0){/*listen for request to connect to the server*/
		printf("ERROR in listen(): %s\n",strerror(errno));
		exit(1);
	}
	sigact.sa_handler=intHandler;
	if(sigaction(SIGINT,&sigact,NULL)<0){
		printf("ERROR in sigaction(): %s\n",strerror(errno));
		exit(1);
	}
	while(keepgoing){
		printf("waiting for connection\n");
		connfd=accept(listenfd,NULL,NULL);/*establish the connection*/
		if(connfd<0){
			if(errno==EINTR)
				break;
			printf("ERROR in accept(): %s\n",strerror(errno));
			exit(1);
		}
		onfile=1;
		printf("connection accepted\n");
		while(onfile){
			count=0;/*counts each time how many chars that were sent to the server in each iteration were printable chars*/
			/*reading from client until we read everything*/
			amount_read=read(connfd,temparr,1024);/*limit how much we read each time to 1024*/
			if(amount_read<0){
				printf("ERROR in read(): %s\n",strerror(errno));
				exit(1);
			}
			else if(amount_read==0){
				onfile=0;
				break;
			}
			printf("%d\n",amount_read);
			printf("finished reading\n");
			for(i=0;i<amount_read;i++){
				if(temparr[i]>=32 && temparr[i]<=126){/*found printable,increase needed counters*/
					count++;
					pcc_total[temparr[i]-32]++;
				}
			}
			printf("preparing to send answer\n");
			length=snprintf(NULL,0,"%d",count);/*coverting the int to a string*/
			str=(char*)calloc(length+1,sizeof(char));/*from stackoverflow "how to convert an int to string in c*/
			snprintf(str,length+1,"%d",count);
			printf("the answer is %s\n",str);
			memset(temparr2,0,5);/*since we receive up to 1024 chars,we will need 4 bits to send the asnwer*/
			memcpy(temparr2,str,length+1);/*last place in temparr will always be \0*/
			printf("the the string we are going to send is %s\n",temparr2);
			while(1){
				amount_sent=write(connfd,temparr2+temp3,4-temp3);
				if(amount_sent<0){
					printf("ERROR in write(): %s\n",strerror(errno));
					exit(1);
				}
				temp3+=amount_sent;
				if(temp3==4)/*we sent the result of this chunk of chars to the client*/
					break;
			}
			temp3=0;
			printf("sent answer\n");
			free(str);
			printf("%d\n",count);
		}
		close(connfd);
	}
	printprintable(pcc_total);
	exit(0);
}
