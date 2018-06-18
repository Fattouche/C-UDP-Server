/*Required Headers*/

#include "meetup.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"

int meetFirst;
int size;

void initialize_meetup(int n, int mf) {
  char label[100];
  int i;
  if (n < 1) {
    fprintf(stderr, "Who are you kidding?\n");
    fprintf(stderr, "A meetup size of %d??\n", n);
    exit(1);
  }

  size = n;
  meetFirst = mf;

  /*
   * Initialize the shared structures, including those used for
   * synchronization.
   */
}

void join_meetup(char *value, int len) {
  printf("WOWZA\n");
  printf("NOTHING IMPLEMENTED YET FOR join_meetup\n");
}
