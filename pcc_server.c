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
	int listenfd,connfd;
	unsigned int temp;
	struct sockaddr_in serv_addr;
	unsigned int pcc_total[95];
	memset(pcc_total,0,95);
	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0){
		printf("ERROR in socker(): %s\n",strerror(errno));
		exit(1);
	}
	temp=string_to_int(argv[1]);
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(temp);
	if(bind(listenfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr))!=0){
		printf("ERROR in bind(): %s\n",strerror(errno));
		exit(1);
	}
	if(listen(listenfd,10)!=0){
		printf("ERROR in listen(): %s\n",strerror(errno));
		exit(1);
	}
	while(1){
		connfd=accept(listenfd,NULL,NULL);
		if(connfd<0){
			printf("ERROR in accept(): %s\n",strerror(errno));
			exit(1);
		}
		
		
	}
	exit(0);
}
