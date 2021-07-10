#include "headers.h"
#include "utils.h"
#include "prompt.h"
#include "globals.h"

char hostname[BUF_HSTNM];
char wd[BUF_PWD];

void prompt() {
  struct passwd *pswd;


  // getting username
  pswd = getpwuid(getuid());
  if (pswd == NULL) {
    perror("username");
    return;
  }  

  // getting hostname
  if (gethostname(hostname, sizeof(hostname)) < 0) {
    perror("hostname");
    return;
  }

  // getting current dir
  if (getcwd(wd, sizeof(wd)) == NULL) {
    perror("pwd");
    return;
  }
  fflush(stdin);
  fflush(stdout);
  dprintf(shell_stdout, "<\033[1;32m%s@%s\033[0m:\033[1;36m%s\033[0m> ", pswd->pw_name, hostname, rel_path(wd));
}