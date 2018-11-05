#define _XOPEN_SOURCE 500 /*added according to stackoverflow
"pread and pwrite not defined",gives warning without this*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

void concat(char* first,char* second){
        while(*first!='\0'){
                first++;
}
        while(*second!='\0'){
                *first=*second;
                first++;
                second++;
        }
        *first = '\0';
}


int main(int argc,char** argv){
	int temp;
	char* addto;
	int fd=0;
	int offset=0;
	char* buffer=(char*)malloc(strlen(argv[1])+1);
	if (buffer == NULL) {
		printf("Error: malloc has failed (memory allocation error)\n");
		return 1;
	}
	char* dir=getenv("HW1DIR");
	char* file=getenv("HW1TF");
	if((!dir) || (!file)){ /*from stackoverflow
"how to check if enviroment variable is set from c program"*/
	printf("Error: Atleast one of the enviroment variables needed is not defined\n");
	return 1;}
	addto=(char*)malloc(strlen(dir)+strlen(file)+1);
	if (addto == NULL) {
		printf("Error: malloc has failed (memory allocation error)\n");
		return 1;
	}
	concat(addto,dir);
	concat(addto,"/");
	concat(addto,file);
	fd=open(addto,O_RDONLY);
	if(fd<0){
	printf("Error opening file: %s\n",strerror(errno));
	return 1;
}

	while((temp=pread(fd,buffer,strlen(argv[1]),offset))){
		if(temp<0){
		printf("Error reading from file: %s\n",strerror(errno));
		return 1;
	}
		if(!(strcmp(argv[1],buffer))){
		fwrite(argv[2],1,strlen(argv[2]),stdout);
		offset+=strlen(argv[1]);
	}
		else{
		fwrite(buffer,1,1,stdout);
		offset++;
	}
}
	close(fd);
	free(addto);
	return 0;
}
