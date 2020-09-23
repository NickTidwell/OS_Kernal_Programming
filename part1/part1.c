#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main ()
{
  char * argv[2];
  argv[0] = "ls";
  argv[1] = "-l";
  pid_t pid;
  pid = fork();
  if (pid == 0) {
    execv(argv[0], argv);
  }
  else {
    waitpid(pid, NULL, 0);
  }
  char * arg[2];
  arg[0] = "echo";
  arg[1] = "hello";
  pid = fork();
  if (pid == 0) {
    execv(arg[0], arg);
  }
  else {
    waitpid(pid, NULL, 0);
  }
  sleep(1);
  return 0;
}
