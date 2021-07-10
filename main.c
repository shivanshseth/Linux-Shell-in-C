#include "headers.h"
#include "prompt.h"
#include "utils.h"
#include "globals.h"
#include "exec.h"

char home[BUF_PWD];
int shellpgid;
int fgpid;
int* jtop;
int shell_stdin;
int shell_stdout;

int main(int argc, char* argv[]) {
  shell_stdin = dup(STDIN_FILENO);
  shell_stdout = dup(STDOUT_FILENO);
  jtop = malloc(sizeof(int));
  *jtop = 1;
  signal(SIGTSTP, zhandler);
  signal(SIGINT, chandler);
  fgpid = 0;
  shellpgid = getpgid(getpid());
  getcwd(home, sizeof(home));
  char *cmds[100];
  while(1) {
    prompt();
    exec();
  }
}