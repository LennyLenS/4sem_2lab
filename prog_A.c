#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t child_pid;

void start(int sig) {
    printf("Start timer in prog A\n");
    sleep(12);
    int wstatus;
    if (waitpid(child_pid, &wstatus, WNOHANG) == 0) {
      kill(child_pid, SIGTERM);
    }
}
 

int main(){
	int return_code = 0;
	child_pid = fork();
	if(child_pid < 0){
		printf("Error. Fork failed\n");
		return_code = 1;
	}else if(child_pid == 0){
		char *args[] = {"./prog_B.o", NULL};
		execv(args[0], args);
	}else{
		signal(SIGUSR1, start);
	}

	int status;
	wait(&status);

	return return_code;
}