#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
void stringshift(char* buffer,char* tempchar){
	int i;
	for(i=0;i<strlen(argv[1]);i++){
	buffer[i]=buffer[i+1];
	}
	buffer[strlen(argv[1])]=tempchar[0];
}

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
	int temp2;
	char* addto;
	int fd=0;
	int offset=0;
	char* buffer=(char*)calloc(strlen(argv[1])+1,1);
	char* tempchar=(char*)calloc(2,1);
	if (buffer == NULL) {
		printf("Error: calloc has failed (memory allocation error)\n");
		return 1;
	}
	char* dir=getenv("HW1DIR");
	char* file=getenv("HW1TF");
	if((!dir) || (!file)){ /*from stackoverflow
"how to check if enviroment variable is set from c program"*/
		printf("Error: Atleast one of the enviroment variables needed is not defined\n");
		free(buffer);
		return 1;
	}
	addto=(char*)calloc(strlen(dir)+strlen(file)+2,1);
	if (addto == NULL) {
		printf("Error: calloc has failed (memory allocation error)\n");
		free(buffer);
		return 1;
	}
	concat(addto,dir);
	concat(addto,"/");
	concat(addto,file);
	fd=open(addto,O_RDONLY);
	if(fd<0){
		printf("Error opening file: %s\n",strerror(errno));
		free(addto);
		free(buffer);
		return 1;
}
	do{
		if(temp2)
			temp=read(fd,buffer,strlen(argv[1]));
		else
			temp=read(fd,tempchar,1);
		if(temp<0){
			printf("Error reading from file: %s\n",strerror(errno));
			free(addto);
			free(buffer);
			return 1;
	}
		if(!temp2)
			stringshift(buffer,tempchar);
		if(!(strcmp(argv[1],buffer))){
			fwrite(argv[2],1,strlen(argv[2]),stdout);
			temp2=1;
	}
		else{
			fwrite(buffer,1,1,stdout);
			temp2=0;
		}
	while((temp=pread(fd,buffer,strlen(argv[1]),offset))){
		if(temp<0){
		printf("Error reading from file: %s\n",strerror(errno));
		free(addto);
		free(buffer);
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
	free(buffer);
	return 0;
}
