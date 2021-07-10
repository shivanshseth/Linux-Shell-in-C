#include "headers.h"
#include "utils.h"
#include "builtins.h"
#include "globals.h"
#include "nightswatch.h"
#include "history.h"


struct Job* jobs[MAX_JOBS];
int bck  = 0;
char* fgpname = NULL;
int fgpgid  = 0;
void zhandler(int signum) {
  if(fgpgid > 0) {
    setpgid(fgpgid, fgpgid);
    kill(fgpgid, signum);  
  }
}
void chandler(int signum) {
  if(fgpgid > 0)
    kill(fgpgid, signum);  
}
void bg_exit(int signum) {
  int status;
  int pid;
  for(int i = 1; i < *jtop; i++) { 
    if(jobs[i] != NULL) {
      if(waitpid(jobs[i]->pid, &status, WNOHANG) > 0)
      if(WEXITSTATUS(status)==0) { 
        fprintf(stderr, "\n%s with pid %d exited normally with exit status %d", jobs[i]->pname, jobs[i]->pid, WEXITSTATUS(status));
        del_job(i);      
      }
      else if(WIFSIGNALED(status)==0) {
        fprintf(stderr, "\n%s with pid %d terminated due to signal %d", jobs[i]->pname, jobs[i]->pid, WTERMSIG(status));
        del_job(i);      
      }
      fflush(stderr);
    }
  }
  return;
}

int exec_cmd(int argc, char* argv[], char* cmd) {
  bck = 0;

  // Redirection
  int t = 0;
  char redir_symbols[][3] = {">", ">>", "<"};
  int fd;
  int stdin_bak = dup(0);
  int stdout_bak = dup(1);
  int rt = -1;
  int ind = -1;
  for(int i = argc-1; i >= 0; i--) {
    if (strcmp(argv[i], ">")==0) {
      rt = 0;
      ind = i;
      fd = open(argv[ind+1], O_TRUNC | O_WRONLY | O_CREAT);
      if (fd < -1)
      {
        perror(argv[ind+1]);
        return 1;
      }
      dup2(fd, 1);
      argv[ind] = NULL;
      close(fd);
    }
    else if (strcmp(argv[i], ">>")==0) {
      rt = 1;
      ind = i;
      fd = open(argv[ind+1], O_WRONLY | O_CREAT | O_APPEND);
      if (fd < -1)
      {
        perror(argv[ind+1]);
        return 1;
      }
      dup2(fd, 1);
      argv[ind] = NULL;
      close(fd);
    }
    else if (strcmp(argv[i], "<")==0) {
      rt = 2;
      ind = i;
      fd = open(argv[ind+1], O_RDONLY);
      if (fd < -1)
      {
        perror(argv[ind+1]);
        return 1;
      }
      dup2(fd, 0);
      argv[ind] = NULL;
      close(fd);
    }
  }
  int llen = strlen(argv[argc-1]);
  if(argv[argc-1] != NULL && argv[argc-1][llen-1] == '&') {
    bck = 1;
    if (strcmp(argv[argc-1], "&") == 0)
      argv[argc-1] = NULL;
    else
      argv[argc-1][llen-1] = '\0';
  }
  if(ind > -1)
    argc = ind;
  if (strcmp("echo", argv[0]) == 0) echo(argc, argv);
  else if(strcmp("pwd", argv[0]) == 0) pwd(argc, argv);
  else if(strcmp("cd", argv[0]) == 0) cd(argc, argv);
  else if(strcmp("ls", argv[0]) == 0) ls(argc, argv);
  else if(strcmp("pinfo", argv[0]) == 0) pinfo(argc, argv);
  else if(strcmp("nightswatch", argv[0]) == 0) nightswatch(argc, argv);
  else if(strcmp("history", argv[0]) == 0) history(argc, argv);
  else if(strcmp("setenv", argv[0]) == 0) setenv_fn(argc, argv);
  else if(strcmp("unsetenv", argv[0]) == 0) unsetenv_fn(argc, argv);
  else if(strcmp("quit", argv[0]) == 0) quit(argc, argv);
  else if(strcmp("jobs", argv[0]) == 0) jobs_fn(argc, argv);
  else if(strcmp("kjob", argv[0]) == 0) kjob(argc, argv);
  else if(strcmp("fg", argv[0]) == 0) fg(argc, argv);
  else if(strcmp("bg", argv[0]) == 0) bg(argc, argv);
  else if(strcmp("overkill", argv[0]) == 0) overkill(argc, argv);
  else if(strcmp("getenv", argv[0]) == 0) {
    char* n;
    if ((n = (char*)getenv(argv[1])) != NULL)
      printf("%s\n", n);
    else 
      printf("NULL\n");
  }
  else {
    int pid;
    if ((pid = fork()) < 0) {
      perror("fork");
      return 1;
    }

    if (pid > 0) {
      if (!bck) {
        int status;
        fgpgid = pid;
        fgpname = cmd;
        waitpid(pid, &status, WUNTRACED); 
        fgpgid = 0;
        if(WIFSTOPPED(status)){
          add_job(cmd, pid);
          dprintf(shell_stdout, "[%d]\n", *jtop-1);
        }
      } else {
        add_job(cmd, pid);
        dprintf(shell_stdout, "[%d]\n", *jtop-1);
        signal(SIGCHLD, bg_exit); 
        return 0;
      }  
    }
    if (pid == 0) {
      if(bck) 
        setpgid(0, 0);       
      if(execvp(argv[0], argv) < 0) {
        perror(argv[0]);
      }

      exit(EXIT_FAILURE);
    }
  }
  dup2(stdout_bak, 1);
  dup2(stdin_bak, 0);
}

void exec_pipe(char* cmd) {
  int stdin_bak = dup(0);
  int stdout_bak = dup(1);
  int ind = -1;
  int pfds[3];
  char *argv[MAX_ARGS];
  int argc;
  for(int i = strlen(cmd)-1; i >= 0; i--) {
    if (cmd[i] == '|') {
      ind = i;
      break;
    }
  }
  if (ind > -1) {
    // printf("LOL%s %d\n", cmd, ind);
    if (pipe(pfds) == -1) {
      perror("pipe");
      return;
    }
    if (fork() == 0) {
      // printf("ls %d\n", getpid());
      // child/read-end
      dup2(pfds[0], 0);
      close(pfds[0]);
      close(pfds[1]);
      // if (d = read(0, inp, d) > 0)
      //   printf("%s %d", inp, d);
      // else printf("waiting for write\n");
      exec_pipe(cmd + ind + 1);
      exit(1);
    } 
    
    if (fork() == 0) {
      // printf("ls2 %d\n", getpid());
      // write-end
      char new_cmd[ind+1];
      strncpy(new_cmd, cmd, ind);
      new_cmd[ind] = '\0';
      dup2(pfds[1], 1); 
      close(pfds[1]);
      close(pfds[0]);
      exec_pipe(new_cmd);
      exit(1);
    }
    close(pfds[1]);
    close(pfds[0]);
    wait(0);
    wait(0);
    return;
  }

  argc = tokenize(cmd, argv, " \t");
  argv[argc] = NULL;
  exec_cmd(argc, argv, cmd);
  return;
}

void exec() {
  size_t inp_size = BUF_INP;
  char *inp;
  int n_cmds = 0;
  inp = (char *) malloc(BUF_INP * sizeof(char));
  char *cmds[MAX_CMDS];
  int bck = 0;
  int fd;
  int stdin_bak = dup(0);
  int stdout_bak = dup(1);
  // getting input
  fflush(stdin);
  int len = getline(&inp, &inp_size, stdin);
  if(len == -1) 
    exit(1);
  inp[len-1] = '\0'; // remove \n from the input
  add_to_history(inp);  
  // getting all cmds
  n_cmds = tokenize(inp, cmds, ";");

  // tokenizing and executing each command
  for(int i=0; i < n_cmds; i++) {
    exec_pipe(cmds[i]);
  }
  free(inp);
  inp = NULL;
  dup2(stdin_bak, 0);
  dup2(stdout_bak, 1);
}

