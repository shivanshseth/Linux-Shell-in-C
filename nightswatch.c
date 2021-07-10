#include "headers.h"
#include "globals.h"
#include "utils.h"

struct pollfd khit;

int ncpu;

void interrupt(int interval) {
  char* inp;
  size_t inp_size = BUF_INP;
  int ncpu = 0;
  khit.fd  = STDIN_FILENO;
  khit.events = POLLIN;
  inp = (char *) malloc(BUF_INP * sizeof(char)); 
  time_t cur, last;
  last = time(NULL) - interval;
  FILE* f;
  int len;
 
  f = fopen("/proc/interrupts", "r");
    if (f == NULL) {
      perror("/proc/interrupts");
      return;
    }
  len = getline(&inp, &inp_size, f);
  inp[len-1] = '\0';
  char* tok = strtok(inp, " ");
  while(tok != NULL) {
    if(strlen(tok) > 3 && strncmp("CPU", tok, 3) == 0) {
      printf("%9s", tok);
      ncpu++;
    }
    tok = strtok(NULL, " ");
  }
  printf("\n");
  fclose(f);

  char q;
  while(1) {
    
    if (poll(&khit, 1, 1) > 0) {
      q = getchar();
      if (q == 'q') {
        while(getchar() != '\n');
        return;
      } 
    }
    cur = time(NULL);
    if (cur - last >= interval) {
      f = fopen("/proc/interrupts", "r");
      if (f == NULL) {
        perror("/proc/interrupts");
        return;
      }
      char val[9];
      char temp[9];
      last = cur;
      len = getline(&inp, &inp_size, f);
      len = getline(&inp, &inp_size, f);
      fscanf(f, "%s", temp);
      for(int i = 0; i < ncpu; i++) {
        fscanf(f, "%s", val);
        printf("%9s", val);
      }
      printf("\n");
      fclose(f);
    }
  }
}

void newborn(int interval) {
  char* inp;
  size_t inp_size = BUF_INP;

  khit.fd  = STDIN_FILENO;
  khit.events = POLLIN;
  inp = (char *) malloc(BUF_INP * sizeof(char)); 
  time_t cur, last;
  last = time(NULL) - interval;
  FILE* f;
  int len;
  char q;
  while(1) {
    if (poll(&khit, 1, 1) > 0) {
      q = getchar();
      if (q == 'q') {
        while(getchar() != '\n');
        return;
      } 

    }
    cur = time(NULL);
    if (cur - last >= interval) {
      f = fopen("/proc/loadavg", "r");
      if (f == NULL) {
        perror("/proc/loadavg");
        return;
      }
      char val[9];
      last = cur;
      for(int i = 0; i < 5; i++) {
        fscanf(f, "%s", val);
      }
      printf("%9s", val);
      printf("\n");
      fclose(f);
    }
  }
}

void nightswatch(int argc, char* argv[]) {
  
  char* usage = "Invalid Argument(s).\nUsage: nightswatch [​options​] <​command​>​\nOptions​:-n ​seconds​\nCommand​: interrupt/newborn\n";
  int ch = -1;

  if (argc < 4 || strcmp(argv[1], "-n") != 0 || ((strcmp(argv[3], "interrupt") != 0) && (strcmp(argv[3], "newborn") != 0))) {  
    printf("%s", usage);
    return;
  } 
  
  if ((strcmp(argv[3], "interrupt") == 0)) ch = 1;
  else if((strcmp(argv[3], "newborn") == 0)) ch = 2;

  
  for(int i =0; i < strlen(argv[2]); i++) {
    if(!isdigit(argv[2][i])) {
      printf("%s", usage);
      return;
    }
  }
  int interval = atoi(argv[2]);
  HideStdinKeystrokes();
  if (ch == 1) interrupt(interval);
  else newborn(interval);
  ShowStdinKeystrokes();
}