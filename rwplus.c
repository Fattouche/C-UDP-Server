/*Required Headers*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"
#include "rw.h"

/* The idea for this "light_switch_t" solution is taken directly from the little
 * book of semaphores.
 */

// Our light switch struct used to keep track of who has most recently accessed
// the data
typedef struct {
  int counter;
  sem_t mutex;
} light_switch_t;

sem_t no_readers;
sem_t no_writers;
static light_switch_t read;
static light_switch_t write;
static resource_t data;

// Initialize all semaphores to 1
void initialize_readers_writer() {
  sem_init(&no_readers, 0, 1);
  sem_init(&no_writers, 0, 1);
  read.counter = 0;
  sem_init(&read.mutex, 0, 1);
  write.counter = 0;
  sem_init(&write.mutex, 0, 1);
}

// If the counter is 1 it means the thread is the first
// thread attempting to access the data so we must wait until the semaphore has
// been signaled by the other user type(reader or writer). The first thread will
// then block all future threads of same type untill it has access. ie if the
// reader is the first reader attempting to access the data, it will wait untill
// all writers have finished. Since the first reader is waiting for the writers
// to finish, future readers will be blocked on sem_wait(light.mutex).
void lock_switch(light_switch_t *light, sem_t *sem) {
  sem_wait(&(*light).mutex);
  (*light).counter++;
  if ((*light).counter == 1) {
    sem_wait(sem);
  }
  sem_post(&(*light).mutex);
}

// Similar to lock_switch except that we wait until the last thread has finished
// executing it's critical section before signaling to other thread types that
// they can access the data.
void unlock_switch(light_switch_t *light, sem_t *sem) {
  sem_wait(&(*light).mutex);
  (*light).counter--;
  if ((*light).counter == 0) {
    sem_post(sem);
  }
  sem_post(&(*light).mutex);
}

// 1. Make sure there is no writers currently executing
// 2. Check if we need to wait for first reader to access
// 3. Signal to unblock readers
// 4. Read
// 5. Signal to first reader in queue that they can start reading
void rw_read(char *value, int len) {
  sem_wait(&no_readers);
  lock_switch(&read, &no_writers);
  sem_post(&no_readers);
  read_resource(&data, value, len);
  unlock_switch(&read, &no_writers);
}

// 1. Check if we need to wait for first writer, queue up writers for
// writer priority
// 2. Wait for the rest of the writers to finish writing
// 3. Write
// 4. Signal future writers
// 5. Signal to first writer in queue that they can start writing
void rw_write(char *value, int len) {
  lock_switch(&write, &no_readers);
  sem_wait(&no_writers);
  write_resource(&data, value, len);
  sem_post(&no_writers);
  unlock_switch(&write, &no_readers);
}
