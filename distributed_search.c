#include <stdio.h>
#include <pthreah.h>
#include <string.h>


int string_to_int(char* thread_count){/*translate the argument which specifies how many threads we want to use to an int*/
	int count=0,i;
	for(i=0;i<strlen(thread_count);i++){
		count*=10;
		count+=thread_count[i];
	}
	return count;
}

int main(int argc,char** argv){
	int num_threads;



	num_threads=string_to_int(argv[3]);







	exit(0);
}
