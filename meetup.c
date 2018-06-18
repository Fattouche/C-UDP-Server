/*Required Headers*/

#include "meetup.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"

int meetFirst;
int finalGroupSize;
int meetupCount;
sem_t waitForArrival;
sem_t waitForCode;
sem_t mutex;
static resource_t data;

void initialize_meetup(int n, int mf) {
  char label[100];
  int i;
  if (n < 1) {
    fprintf(stderr, "Who are you kidding?\n");
    fprintf(stderr, "A meetup size of %d??\n", n);
    exit(1);
  }
  // Initialize a shared semaphore to value of 1 so we can use as mutex
  sem_init(&mutex, 0, 1);
  sem_init(&waitForArrival, 0, 0);
  sem_init(&waitForCode, 0, 1);
  meetupCount = 0;
  finalGroupSize = n;
  meetFirst = mf;
}

void join_meetup(char *value, int len) {
  sem_wait(&mutex);
  meetupCount++;
  if (meetupCount == finalGroupSize) {
    if (!meetFirst) {
      write_resource(&data, value, len);
    }
    sem_wait(&waitForCode);
    sem_post(&waitForArrival);
  } else if (meetFirst && meetupCount == 1) {
    write_resource(&data, value, len);
  }
  sem_post(&mutex);

  sem_wait(&waitForArrival);
  sem_post(&waitForArrival);

  read_resource(&data, value, len);

  sem_wait(&mutex);
  meetupCount--;
  if (meetupCount == 0) {
    sem_wait(&waitForArrival);
    sem_post(&waitForCode);
  }
  sem_post(&mutex);

  sem_wait(&waitForCode);
  sem_post(&waitForCode);
}
