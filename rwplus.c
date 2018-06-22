/*Required Headers*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "resource.h"
#include "rw.h"

/* The idea for this "lightswitch" solution is taken directly from the little
 * book of semaphores.
 */

typedef struct {
  int counter;
  sem_t mutex;
} LightSwitch;

sem_t no_readers;
sem_t no_writers;
LightSwitch read;
LightSwitch write;

void initialize_readers_writer() {
  sem_init(&no_readers, 0, 1);
  sem_init(&no_writers, 0, 0);
  read.counter = 0;
  sem_init(&read.mutex, 0, 1);
  write.counter = 0;
  sem_init(&write.mutex, 0, 1);
}

void rw_read(char *value, int len) {
  sem_wait(&no_readers);
  lock_switch(&read, &no_writers));
  sem_post(&no_readers);
  read_resource(&data, value, len);
  unlock_switch(&read, &no_writers);
}

void rw_write(char *value, int len) {
  lock_switch(&write, &no_readers);
  sem_wait(&no_writers);
  write_resource(&data, value, len);
  sem_post(&no_writers);
  unlock_switch(&write, &no_readers);
}

void lock_switch(LightSwitch *light, sem_t *sem) {
  sem_wait(light.mutex);
  light.counter++;
  if (light.counter == 1) {
    sem_wait(sem);
  }
  sem_post(light.mutex);
}

void unlock_switch(LightSwitch *light, sem_t *sem) {
  sem_wait(light.mutex);
  light.counter--;
  if (light.counter == 0) {
    sem_post(sem);
  }
  sem_post(light.mutex);
}
