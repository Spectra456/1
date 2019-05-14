/*************************************/
/*
resource.c
*/
/*************************************/
#include "sys.h"

#include "rtos_api.h"

#include <stdio.h>

void InitPVS(TSemaphore S, char * name) {
  printf("InitPVS %s\n", name);
  ResourceQueue[S].name = name;
  ResourceQueue[S].block = 0;
}
void P(TSemaphore S) {
  printf("P %s\n", ResourceQueue[S].name);
  while (ResourceQueue[S].block) { // ресурс заблокирован
    printf("Resource is blocked\n");
    TaskQueue[RunningTask].task_state = TASK_WAITING;
    TaskQueue[RunningTask].waited_resource = S;
    Dispatch();
  }
  ResourceQueue[S].block = 1;
  printf("End of P %s\n", ResourceQueue[S].name);
}
void V(TSemaphore S) {
  printf("V %s\n", ResourceQueue[S].name);
  ResourceQueue[S].block = 0;
  for (int i = 0; i < MAX_TASK; i++) {
    if (TaskQueue[i].task_state == TASK_WAITING && TaskQueue[i].waited_resource == S) {
      TaskQueue[i].task_state = TASK_READY;
      TaskQueue[i].waited_resource = -1;
    }
  }
  Dispatch();
  printf("End of V %s\n", ResourceQueue[S].name);
}
