#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  char command[256];
  printf("Enter command for prog_B: ");
  fgets(command, sizeof(command), stdin);

  int cmd_counter = 0;
  char* args[16][256];
  args[cmd_counter][0] = strtok(command, "|");

  while (args[cmd_counter][0] != NULL) {
    cmd_counter++;
    args[cmd_counter][0] = strtok(NULL, "|");
  }

  for (int i = 0; i < cmd_counter; i++) {
    char* arg = strtok(args[i][0], " \n");
    int arg_counter = 0;
    while (arg != NULL) {
      args[i][arg_counter] = arg;
      arg_counter++;
      arg = strtok(NULL, " \n");
    }
  }

  int pipe1[2] = {-1, -1};
  int pipe2[2];

  for (int i = 0; i < cmd_counter; ++i) {
    if (i != cmd_counter - 1) {
      pipe(pipe2);
    }

    pid_t child_pid = fork();

    if (child_pid < 0) {
      printf("Error. Fork failed\n");
      exit(EXIT_FAILURE);
    } else if (child_pid == 0) {
      if (pipe1[0] != -1) {
        close(pipe1[1]);
        dup2(pipe1[0], 0);
      }

      if (i != cmd_counter - 1) {
        close(pipe2[0]);
        dup2(pipe2[1], 1);
      }

      execvp(args[i][0], args[i]);
    } else {
      close(pipe1[0]);
      close(pipe1[1]);
      pipe1[0] = pipe2[0];
      pipe1[1] = pipe2[1];

      int wstatus;
      wait(&wstatus);

      if (wstatus != 0) {
        exit(EXIT_FAILURE);
      }
    }
  }

  return 0;
}