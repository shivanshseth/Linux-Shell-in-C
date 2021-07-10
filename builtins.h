#ifndef BUILTINS_H  /* Include guard */
#define BUILTINS_H


void echo(int argc, char* argv[]);
void pwd(int argc, char* argv[]);
void cd(int argc, char* argv[]);
void ls(int argc, char* argv[]);
void pinfo(int argc, char* argv[]);
void setenv_fn(int argc, char* argv[]);
void unsetenv_fn(int argc, char* argv[]);
void quit(int argc, char* argv[]);
void jobs_fn(int argc, char* argv[]);
void kjob(int argc, char* argv[]);
void fg(int argc, char* argv[]);
void bg(int argc, char* argv[]);
void overkill(int argc, char* argv[]);
#endif