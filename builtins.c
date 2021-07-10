#include "headers.h"
#include "globals.h"
#include "utils.h"
char lwd[BUF_PWD];
void echo(int argc, char* argv[]) {
  for(int i=1; i<argc; i++) {
    printf("%s", argv[i]);
    if(i == argc-1) printf("\n");
    else printf(" ");
  }
}

void pwd(int argc, char* argv[]) {
  if (argc > 1) {
    write(3, "pwd: too many arguments\n", 26);
    return;
  }
  getcwd(wd, sizeof(wd));
  printf("%s\n", wd);
}

int cd(int argc, char* argv[]) {
  char tmp[BUF_PWD];
  getcwd(tmp, sizeof(tmp));
  if(argc > 2) {
    printf("Invalid args %d\n", argc);
    return -1;
  }
  if(argc == 1) {
    if(chdir(proc_path(home)) < 0)
      perror(home);
    return 0;
  }
  if(strcmp(argv[1], "-") == 0) {
    if(strcmp(lwd, "") == 0)
      strcpy(lwd, "~");
    strcpy(argv[1], lwd);
    printf("%s\n", rel_path(lwd));
  }
  if(chdir(proc_path(argv[1])) < 0) {
    perror(argv[1]);
    return 1;
  }
  strcpy(lwd, tmp);
  getcwd(wd, sizeof(wd));
  return 0;
}

int long_desc(char* path) {
	struct stat st;
	if(stat(path, &st) < 0) {
		perror(path);
		return 0;
	}
  struct passwd* usr;
  struct group *grp;
  usr = getpwuid(st.st_uid);
  grp = getgrgid(st.st_gid);
  printf((S_ISDIR(st.st_mode)? "d": "-"));
  int modes[] = {S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};
  char rwx[] = {'r', 'w', 'x'};
  for(int i = 0; i < 9; i++) {
    printf("%c", (st.st_mode & modes[i])? rwx[i%3]: '-');
  }
  printf(" %3ld %9s %9s %9ld %.12s %s\n", st.st_nlink, usr->pw_name, grp->gr_name, st.st_size, ctime(&st.st_mtime) +4, path);
}

int ls_cwd(char* path, int f_all, int f_long) {
  DIR* dir;
  struct dirent *sd;
  dir = opendir(".");
  if(dir == NULL) {
    perror(path);
  } else {
    while((sd = readdir(dir)) != NULL) {
      if(f_all || sd->d_name[0] != '.') {
        if(f_long) long_desc(sd->d_name);
        else printf("%s\n", sd->d_name);
      }
    }
    // if(!f_long) printf("\n");
  }
}

int ls(int argc, char* argv[]) {
  int f_long = 0, f_all = 0, opt;
  DIR* dir;
  struct dirent *sd;
  optind = 0;
  while((opt = getopt(argc, argv, ":la")) != -1) {
    switch (opt)
    {
    case 'l':
      f_long = 1;
      break;
    case 'a':
      f_all = 1;
      break;
    default:
      printf("Invalid option(s). \nOptions: \n\t-l: Long output\n\t-a: Display all files");
      break;
    }
  }
  if(optind >= argc) {
    ls_cwd(wd, f_all, f_long);
  }
  for(; optind < argc; optind++) {
    if(chdir(proc_path(argv[optind])) < 0)
      perror(argv[optind]);
    else {
      printf("%s: \n", argv[optind]);
      ls_cwd(argv[optind], f_all, f_long);
      printf("\n");
      if(chdir(proc_path(wd)) < 0){
        perror(wd);
      }
    }
  }
} 

void pinfo(int argc, char* argv[]) {
  int pid = 0;
  char status[2];
  char exec_path[BUF_PWD];
  char pf_path[BUF_PWD];
  FILE* fd;
  char tmp[BUF_INP];
  int mem = 0;
  if(argc < 2) 
    pid = getpid();
  else 
    pid =atoi(argv[1]);

  printf("pid -- %d\n", pid);
  sprintf(pf_path, "/proc/%d/stat", pid);

  fd = fopen (pf_path,"r");
  if(fd == NULL) {
    perror(pf_path);
    return;
  }
  fscanf(fd, "%d %s %s", &pid, tmp, status);
  fclose(fd);
  printf("Process Status -- %s\n", status);
  sprintf(pf_path, "/proc/%d/statm", pid);

  fd = fopen (pf_path,"r");
  if(fd == NULL) {
    perror(pf_path);
    return;
  }
  fscanf(fd, "%d", &mem);
  fclose(fd);

  printf("memory -- %d\n", mem);

  sprintf(pf_path, "/proc/%d/exe", pid);
  int len;
  if((len = readlink(pf_path, exec_path, BUF_PWD)) < 0) {
    perror(pf_path);
    return;
  }
  exec_path[len] = '\0';
  printf("Executable Path -- %s\n", rel_path(exec_path));
  return;
}

void setenv_fn(int argc, char* argv[]) {
  if (argc > 3 || argc < 2) {
    char err[] = "Usage: setenv var [value]\n";
    write(STDERR_FILENO, err, strlen(err));
    return;
  }
  if (argc == 2) 
    setenv(argv[1], "", 1);
  else if (argc == 3) 
    setenv(argv[1], argv[2], 1); 
}
void unsetenv_fn(int argc, char* argv[]) {
  if (argc != 2) {
    char err[] = "Usage: unsetenv var\n";
    write(STDERR_FILENO, err, strlen(err));
    return;
  }
  unsetenv(argv[1]);
  return;
}

void jobs_fn(int argc, char* argv[]) {
  if (argc > 1) {
    write(3, "jobs: too many arguments\n", 25);
    return;
  }
  for (int i = 1; i < *jtop; i++) {
    if (jobs[i] != NULL)
      printf("[%d] %s %s [%d]\n", jobs[i]->jid, jobs[i]->pstatus, jobs[i]->pname, jobs[i]->pid);
  }
}

void kjob(int argc, char* argv[]) {
  if (argc != 3) {
    write(3, "usage: kjob <job number> <signal number>\n", 43);
    return;
  }
  int jid = atoi(argv[1]);
  int signum = atoi(argv[2]);
  if(jid <= 0 || jid >= *jtop || jobs[jid] == NULL) {
    write(3, "kjob: Invalid job number\n", 27);
    return;
  }
  if(signum <= 0 || signum > 31) {
    write(3, "kjob: Invalid signal number\n", 30);
    return;
  }
  assert(jobs[jid]->jid == jid);
  if (kill(jobs[jid]->pid, signum) == -1) {
    perror("kjob");
    return;
  }
  update_job_status(jid);
}

void fg(int argc, char* argv[]) {
  if (argc != 2) {
    write(3, "usage: fg <job number>\n", 24);
    return;
  }
  int jid = atoi(argv[1]);
  if(jid <= 0 || jid >= *jtop || jobs[jid] == NULL) {
    write(3, "fg: Invalid job number\n", 24);
    return;
  }
  signal(SIGTTOU, SIG_IGN);
  // signal(SIGCHLD, SIG_IGN);
  if (tcsetpgrp(1, jobs[jid]->pid) == -1)
    perror("tcsetpgrp");
  if (kill(jobs[jid]->pid, SIGCONT) == -1) {
    perror(jobs[jid]->pname);
    return;
  }
  update_job_status(jid);
  struct Job j = *jobs[jid];
  del_job(jid);
  int status;
  fgpgid = j.pid;
  fgpname = j.pname;
  waitpid(j.pid, &status, WUNTRACED);
  if(WIFSTOPPED(status)){
    add_job(j.pname, j.pid);
    printf("[%d]\n", *jtop-1);
  }
  if (tcsetpgrp(1, shellpgid) == -1)
    perror("tcsetpgrp");
  fgpgid = 0;
  signal(SIGTTOU, SIG_DFL);
}

void bg(int argc, char* argv[]) {
  if (argc != 2) {
    write(3, "usage: bg <job number>\n", 25);
    return;
  }
  int jid = atoi(argv[1]);
  if(jid <= 0 || jid >= *jtop || jobs[jid] == NULL) {
    write(3, "bg: Invalid job number\n", 25);
    return;
  }
  if (kill(jobs[jid]->pid, SIGCONT) == -1) {
    perror(jobs[jid]->pname);
    return;
  }
  update_job_status(jid);
}

void overkill(int argc, char* argv[]) {
  if (argc != 1) {
    write(3, "usage: overkill\n", 25);
    return;
  }
  for (int i = 0; i < *jtop; i++) {
    if(jobs[i] == NULL)
      continue;
    if (kill(jobs[i]->pid, SIGKILL) == -1) {
      perror(jobs[i]->pname);
      return;
    }
    del_job(i);
  }
}

void quit(int argc, char* argv[]) {
  if (argc != 1) {
    write(3, "usage: quit\n", 13);
    return;
  }
  exit(0);
}