#include<stdlib.h>

void exit (int status){

}

void _start(uint64_t argc, char* argv [], char* envp[]) {
  	// call main() and exit() here
//	char* argv[5];
//	char* envp[5];
//	argv[4]=NULL;
//	envp[4]=NULL; 
//	int status;
	main(argc,argv,envp);
//	exit(status);	
}
