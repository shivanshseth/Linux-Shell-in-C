#include "headers.h"
#include "utils.h"
#include "globals.h"


void HideStdinKeystrokes()
{
    struct termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    /* we want to disable echo */
    tty.c_lflag &= ~ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

void ShowStdinKeystrokes()
{
   struct termios tty;

    tcgetattr(STDIN_FILENO, &tty);

    /* we want to reenable echo */
    tty.c_lflag |= ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

char* proc_path(char* path) {
  if (strcmp(path, "~") == 0)
    return home;
  else return path;
}

char* rel_path(char *path) {
  int i, j;
  if (strncmp(path, home, strlen(home)) == 0) {
    char* newpath = malloc(BUF_PWD);

    newpath[0] = '~';
    j = 1;
    for( i = strlen(home); i < strlen(path); i++) {
      newpath[j++] = path[i];
    } 
    newpath[j] = '\0';
    return newpath;
  }
  return path;
} 

int tokenize(char* str, char* tokens[], char* sp) {
  int argc = 0;
  char *token = strtok(str, sp);
  while(token != NULL) {
    tokens[argc++] = token;
    token = strtok(NULL, sp);
  }
  return argc;
}

int add_job(char* name, int pid) {
  char stat;
  char pf_path[BUF_PWD];
  char tmp[BUF_INP];
  FILE* fd;
  struct Job* j = (struct Job*) malloc(sizeof(struct Job));
  strcpy(j->pname, name);

  j->pid = pid;
  j->ppid = getpid();
  sprintf(pf_path, "/proc/%d/stat", pid);
  fd = fopen (pf_path,"r");
  if(fd == NULL) {
    perror(pf_path);
    return 1;
  }
  fscanf(fd, "%d %s %c", &pid, tmp, &stat);
  fclose(fd);
  char running_vals[] = {'R', 'S'};
  char stopped_vals[] = {'D', 'Z', 'T', 't', 'X'};
  for(int i = 0; i < 2; i++) {
    if (stat == running_vals[i]) {
      strcpy(j->pstatus, "Running");
      break;
    }
  } 
  for(int i = 0; i < 5; i++) {
    if (stat == stopped_vals[i]) {
     strcpy(j->pstatus, "Stopped");
      break;
    }
  }
  j->jid = *jtop;
  jobs[(*jtop)++] = j;
  return 0;
}

void update_job_status(int jid) {
  char stat;
  char pf_path[BUF_PWD];
  char tmp[BUF_INP];
  FILE* fd;
  int pid = jobs[jid]->pid;
  sprintf(pf_path, "/proc/%d/stat", pid);
  fd = fopen (pf_path,"r");
  if(fd == NULL) {
    perror(pf_path);
    free(jobs[jid]);
    jobs[jid] = NULL;
    return;
  }
  fscanf(fd, "%d %s %c", &pid, tmp, &stat);
  fclose(fd);
  char running_vals[] = {'R', 'S'};
  char stopped_vals[] = {'D', 'Z', 'T', 't', 'X'};
  for(int i = 0; i < 2; i++) {
    if (stat == running_vals[i]) {
      strcpy(jobs[jid]->pstatus, "Running");
      break;
    }
  } 
  for(int i = 0; i < 5; i++) {
    if (stat == stopped_vals[i]) {
     strcpy(jobs[jid]->pstatus, "Stopped");
      break;
    }
  }
}

void del_job(int jid) {
  if (jobs[jid] != NULL) {
    free(jobs[jid]);
    jobs[jid] = NULL;
  }
}