#ifndef UTILS_H  /* Include guard */
#define UTILS_H

char* proc_path();
char* rel_path();
void HideStdinKeystrokes();
void ShowStdinKeystrokes();
int tokenize(char* str, char* tokens[], char* sp);
int add_job(char* name, int pid);
void update_job_status(int jid);
void del_job(int jid);
#endif