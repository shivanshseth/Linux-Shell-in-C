#ifndef GLOBALS_H  /* Include guard */
#define GLOBALS_H
#define BUF_INP 100000
#define BUF_PWD 1000
#define BUF_HSTNM 1000
#define MAX_CMDS 100
#define MAX_ARGS 100
#define MAX_JOBS 10000
#define HIST_LIM 20

extern char home[BUF_PWD];
extern char wd[BUF_PWD];
extern char lwd[BUF_PWD];

struct Job {
  int jid;
  int pid;
  char pstatus[20];
  char pname[1000];
  int ppid;
};
extern int shellpgid;
extern int fgpgid;
extern char* fgpname;
extern struct Job* jobs[MAX_JOBS];
extern int* jtop;
extern int shell_stdin;
extern int shell_stdout;
#endif