#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include <pthread.h>


void overflow_routine(int layer) {
	char cache[1024];
	printf("layer: %d  %p  %p\n", layer, &layer, cache);
	layer++;
	overflow_routine(layer);
}


void *reader_routine(void *arg) {
	size_t stack_size = 0;
	void *stack_addr = NULL;
	char name[64] = {0};
	pthread_attr_t attr;
	if(arg) {
		if(pthread_setname_np(pthread_self(), (char*)arg))
			printf("error: pthread_setnmae_np: %m\n");

	}
	if(pthread_getattr_np(pthread_self(), &attr)) {
		printf("error: pthread_getattr_np: %m\n");
		return NULL;
	}
	if(pthread_attr_getstack(&attr, &stack_addr, &stack_size)) {
		printf("error: pthread_attr_getstack: %m\n");
		return NULL;
	}
	if(pthread_getname_np(pthread_self(), name, sizeof(name))) {
		printf("error: pthread_getname_np:%m\n");
	}
	printf("id: %ld   name: %s  stackaddr: %p  stacksize: %ld start...\n",
		 pthread_self(), name, stack_addr, stack_size); 
	overflow_routine(0);
	usleep(2000000);
	printf("id: %ld   name: %s  stackaddr: %p  stacksize: %ld end...\n",
		 pthread_self(), name, stack_addr, stack_size); 
	return NULL;
}

int main(int argc, char *argv[]) {
	pthread_t t1, t2;
	if(pthread_create(&t1, NULL, reader_routine, "t1")) {
		printf("error: pthread_create: %m\n");
		return 1;
	}
	//if(pthread_create(&t2, NULL, reader_routine, "t2")) {
	//	printf("error: pthread_create: %m\n");
	//	return 1;
	//}
	pthread_join(t1, NULL);
	//pthread_join(t2, NULL);
	return 0;
}
