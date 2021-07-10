#include "headers.h"
#include "utils.h"
#include "globals.h"
FILE* fd;;
char p[BUF_PWD];

void add_to_history(char* cmd) { 
  char* inp;
  size_t inp_size = BUF_INP;
  inp = (char *) malloc(BUF_INP * sizeof(char));
  strcpy(p, home);
  strcat(p, "/.history_cshell");
  // printf("%s\n", p);
	fd = fopen(p, "a+");
	if (fd == NULL) {
		perror(p);
		return;
	}
  rewind(fd);
  int c = 0;
  while(getline(&inp, &inp_size, fd) != -1)
    c++;
  if (c >= HIST_LIM) {
    char p2[BUF_PWD];
    strcpy(p2, home);
    strcat(p2, "/.tmp_history");
    FILE *fd2 = fopen(p2, "w+");
    if (fd2 == NULL) {
		  perror(p2);
		  return;
	  }
    rewind(fd);
    for(int i =0; i < c; i++) {
      getline(&inp, &inp_size, fd);
      if (i > c - HIST_LIM)
        fprintf(fd2, "%s", inp);
    }
    fprintf(fd2, "%s\n", cmd);
    fclose(fd);
    fclose(fd2);
    remove(p);
    rename(p2, p);
  } 
  else {
    fprintf(fd, "%s\n", cmd);
    fclose(fd);
  }
  free(inp);
  inp=NULL;
  return;
}

void history(int argc, char* argv[]) {
  char* inp;
  size_t inp_size = BUF_INP;
  inp = (char *) malloc(BUF_INP * sizeof(char));
  int lim = 10;
	strcpy(p, home);
	strcat(p, "/.history_cshell");
	fd = fopen(p, "r");
	if (fd == NULL) {
		perror(p);
		return;
	}
  if(argc == 2) {
    lim = atoi(argv[1]);
  }
  int c = 0;
  while (getline(&inp, &inp_size, fd) != -1) 
    c++;
  int i = 0;
  rewind(fd);
  while (getline(&inp, &inp_size, fd) != -1) {
    i++;
    if (i > c - lim) {
      printf("%s", inp);
    }
  }
  free(inp);
  inp=NULL;
  return;
}