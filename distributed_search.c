#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <limits.h>
#include <errno.h>

typedef struct queueStruct{/*queue struct and operations from geeksforgeeks.org*/
	int size,front,rear;
	char* array;
}queueStruct;

queueStruct* initQueue(){
	queueStruct* queue=(queueStruct*) malloc(sizeof(queueStruct));
	if(queue==NULL){
		printf("malloc failed: %s\n",strerror(errno));
		exit(1);
	}
	queue->size=0;
	queue->front=0;
	queue->rear=0;
	queue->array=(char*)malloc(0);
	return queue;
}

void enqueue(queueStruct* queue,char* item){/*inserts new item to the queue*/
	int i;
	queue->array=(char*)realloc(queue->array,sizeof(char)*(queue->size)+1);
	/*add check if it fails*/
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

static long count=0;
static char* name;
static queueStruct* current_dirs;/*max size of name of dir is NAME_MAX (from limits.h)*/
pthread_mutex_t lock;

int string_to_int(char* thread_count){/*translate the argument which specifies how many threads we want to use to an int*/
	long count=0,i;
	for(i=0;i<strlen(thread_count);i++){
		count*=10;
		count+=thread_count[i]-'0';
	}
	return count;
}

void* thread_func(void* thread_param){/*what each thread does*/
	/*listing items in directory from stackoverflow "how to list files in a directory"*/
	int rc;
	long num=(long)thread_param;
	DIR *d;
	struct dirent *dir;
	char dirname[128];
	dequeue(current_dirs,dirname);
	d=opendir((const char*)dirname);
	if(d){
		while((dir=readdir(d))!=NULL){
			if((dir->d_name)!="." || (dir->d_name)!="..")
				continue;
			if(dir->d_type==DT_DIR)
				enqueue(current_dirs,dir->d_name);
			if(dir->d_name==name)
				rc=pthread_mutex_lock(&lock);
				if(rc){
					printf("ERROR in pthread_mutex_lock(): %s\n",strerror(rc));
					exit(1);
				}
				count++;
				rc=pthread_mutex_unlock(&lock);
				if(rc){
					printf("ERROR in pthread_mutex_unlock(): %s\n",strerror(rc));
					exit(1);
				}
		}
	}
	closedir(d);



	pthread_exit(NULL);
}

int main(int argc,char** argv){
	long num_threads,i;
	int rc;
	void* status;
	name=argv[2];
	current_dirs=initQueue();
	enqueue(current_dirs,arv[1]);
	rc=pthread_mutex_init(&lock,NULL);
	if(rc){
		printf("ERROR in pthread_mutex_init(): %s\n",strerror(rc));
		exit(1);
	}
	num_threads=string_to_int(argv[3]);
	printf("number of threads in total is %ld\n",num_threads);
	pthread_t thread[num_threads];
	for(i=0;i<num_threads;i++){
		rc=pthread_create(&thread[i],NULL,thread_func,(void*)i);
		if(rc){
			printf("ERROR in pthread_create(): %s\n",strerror(rc));
			exit(1);
		}
	}
	for(i=0;i<num_threads;i++){
		rc=pthread_join(thread[i],&status);
		if(rc){
			printf("ERROR in pthread_join(): %s\n",strerror(rc));
			exit(1);
		}
		printf("Main: completed join with thread %ld having a status of %ld\n",i,(long)status);
	}
	printf("amount of threads was %ld\n",count);
	free(current_dirs->array);
	free(current_dirs);
	exit(0);
}
