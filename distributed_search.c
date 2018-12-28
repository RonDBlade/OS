#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>

typedef struct queueStruct{
	int size,front,rear;
	char* array;
}queueStruct;

queueStruct* initQueue(){
	queueStruct* queue=(queueStruct*) malloc(sizeof(queueStruct));
	queue->size=0;
	queue->front=0;
	queue->rear=0;
	queue->array=(char*)malloc(0);
	return queue;
}

void enqueue(queueStruct* queue,char* item){/*inserts new item to the queue*/
	int i;
	for(i=0;i<NAME_MAX;i++){/*insert item to the last place in the queue*/
		if(i<strlen(item))
			queue->array[(queue->rear)+i]=item[i];
		else
			queue->array[(queue->rear)+i]='\0';
	}
	queue->rear=(queue->rear)+NAME_MAX;
	queue->size=(queue->size)+1;
}

void dequeue(queueStruct* queue,char* item){/*pops out the first item in the queue,saves in item*/
	int i=0;
	for(i=0;i<NAME_MAX;i++){
		item[i]=queue->array[(queue->front)+1];
	}
	queue->front=(queue->front)+NAME_MAX;
	queue->size=(queue->size)-1;
}

static int count;
static char* name;
static queueStruct* current_dirs;/*max size of name of dir is NAME_MAX (from limits.h)*/

int string_to_int(char* thread_count){/*translate the argument which specifies how many threads we want to use to an int*/
	int count=0,i;
	for(i=0;i<strlen(thread_count);i++){
		count*=10;
		count+=thread_count[i];
	}
	return count;
}

void* thread_func(void* thread_param){
	DIR *d;
	struct dirent *dir;
	char dirname[128];
	dequeue(current_dirs,dirname);
	d=opendir((const char*)dirname);
	exit(0);
}

int main(int argc,char** argv){
	int num_threads,i,rc;
	name=argv[2];
	current_dirs=initQueue();
	num_threads=string_to_int(argv[3]);
	pthread_t thread[num_threads];
	for(i=0;i<num_threads;i++){
		rc=pthread_create(&thread[i],NULL,thread_func,NULL);
		if(rc){
			printf("ERROR in pthread_create(): %s\n",strerror(rc));
			exit(1);
		}


	}






	exit(0);
}
