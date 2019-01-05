#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

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
	int listenfd,connfd,onfile;
	unsigned int temp,amount_read,temp2=0,i,count=0,amount_sent,length,temp3=0;
	struct sockaddr_in serv_addr;
	char temparr[1024];
	char* str;
	unsigned int pcc_total[95];/*init counter struct*/
	memset(pcc_total,0,95);
	memset(temparr,0,1024);
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
	while(1){
		connfd=accept(listenfd,NULL,NULL);/*establish the connection*/
		if(connfd<0){
			printf("ERROR in accept(): %s\n",strerror(errno));
			exit(1);
		}
		onfile=1;
		printf("connection accepted\n");
		while(onfile){
			count=0;/*counts each time how many chars that were sent to the server in each iteration were printable chars*/
			while(1){/*reading from client until we read everything*/
				amount_read=read(connfd,temparr+temp2,1-temp2);/*limit how much we read each time to 1024*/
				if(amount_read<0){
					printf("ERROR in read(): %s\n",strerror(errno));
					exit(1);
				}
				else if(amount_read==0){
					onfile=0;
					break;
				}
				temp2+=amount_read;
				if(temp2==1)
					break;
				printf("%d\n",temp2);
			}
			if(onfile==0)
				break;
			printf("finished reading\n");
			for(i=0;i<temp2;i++){
				if(temparr[i]>=32 && temparr[i]<=126){/*found printable,increase needed counters*/
					count++;
					pcc_total[temparr[i]-32]++;
				}
			}
			printf("preparing to send answer\n");
			length=snprintf(NULL,0,"%d",count);/*coverting the int to a string*/
			str=(char*)calloc(length+1,sizeof(char));/*from stackoverflow "how to convert an int to string in c*/
			snprintf(str,length+1,"%d",count);
			while(1){
				amount_sent=write(connfd,str+temp3,length-temp3);
				if(amount_sent<0){
					printf("ERROR in write(): %s\n",strerror(errno));
					exit(1);
				}
				temp3+=amount_sent;
				if(temp3==length)/*we sent the result of this chunk of chars to the client*/
					break;
			}
			printf("sent answer\n");
			temp2=0;
			free(str);
			printf("%d\n",count);
		}
	}
	exit(0);
}
