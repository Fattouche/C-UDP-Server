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
sem_t waitToRead;
sem_t mutex;
sem_t waitForGroup;
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
  sem_init(&waitToRead, 0, 0);
  sem_init(&waitForGroup, 0, 1);
  meetupCount = 0;
  finalGroupSize = n;
  meetFirst = mf;
}

// We use three different semaphores here:
// 1. The semaphore called mutex is used to block concurrent access to variables
// 2. The sempahore called waitForGroup is used to ensure that we only proceed
// if it is our groups turn,
// ie. if n=2 and a third thread comes we want to block until the first 2
// threads have finished.
// 3. The semaphore called waitToRead is used to ensure that we wait for all n
// threads to arrive before we start reading.
void join_meetup(char *value, int len) {
  sem_wait(&waitForGroup);
  sem_wait(&mutex);
  meetupCount++;
  // If we have the final group size, don't signal anymore to the waitForGroup
  // sem because we want to block until our entire thread group has finished. We
  // also need to signal to any threads blocked on waitToRead()
  if (meetupCount == finalGroupSize) {
    if (!meetFirst) {
      write_resource(&data, value, len);
    }
    sem_post(&waitToRead);
  } else if (meetFirst && meetupCount == 1) {
    sem_post(&waitForGroup);
    write_resource(&data, value, len);
  } else {
    sem_post(&waitForGroup);
  }
  sem_post(&mutex);

  // Make sure we don't go through until all threads have arrived, first
  // signaled thread will wake up and then immediately signal to the next
  // thread.
  sem_wait(&waitToRead);
  sem_post(&waitToRead);

  // critical section
  read_resource(&data, value, len);

  sem_wait(&mutex);
  meetupCount--;
  // Once all threads have finished reading we can return waitToRead to 0 which
  // will block future groups and we can also signal that the next group of
  // threads can start working again
  if (meetupCount == 0) {
    sem_wait(&waitToRead);
    sem_post(&waitForGroup);
  }
  sem_post(&mutex);
}
