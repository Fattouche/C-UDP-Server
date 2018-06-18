/*Required Headers*/

#include "rw.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"

/*
 * Declarations for reader-writer shared variables -- plus concurrency-control
 * variables -- must START here.
 */

static pthread_mutex_t mutex;
static pthread_cond_t myTurn;
static resource_t data;
int writers;
int readers;
int activeWriters;  // writer priority

void initialize_readers_writer() {
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&myTurn, NULL);
  activeWriters = 0;
  writers = 0;
  readers = 0;
}

// If we see writers, we sleep until broadcasted, done in a loop to avoid
// spontaneous wakeups Once we have passed the condition, increment the readers
// variable and read the data. Lastly signal to the waiting threads.
void rw_read(char *value, int len) {
  pthread_mutex_lock(&mutex);
  while (writers) {
    pthread_cond_wait(&myTurn, &mutex);
  }
  readers++;
  pthread_mutex_unlock(&mutex);
  read_resource(&data, value, len);
  pthread_mutex_lock(&mutex);
  readers--;
  pthread_mutex_unlock(&mutex);
  pthread_cond_broadcast(&myTurn);
}

// Even if we aren't writing yet, we want to have writer priority so increment
// writers. Same stuff as readers after that.
void rw_write(char *value, int len) {
  pthread_mutex_lock(&mutex);
  writers++;
  while (readers || activeWriters) {
    pthread_cond_wait(&myTurn, &mutex);
  }
  activeWriters++;
  pthread_mutex_unlock(&mutex);
  write_resource(&data, value, len);
  pthread_mutex_lock(&mutex);
  activeWriters--;
  writers--;
  pthread_mutex_unlock(&mutex);
  pthread_cond_broadcast(&myTurn);
}
