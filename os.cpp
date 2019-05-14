/******************************/
/*
os.c
*/
/******************************/
#include <stdio.h>

#include "sys.h"

#include "rtos_api.h"

int StartOS(TTaskCall entry, int priority, char * name) {
  int i;
  RunningTask = -1;
  TaskCount = 0;
  FreeTask = 0;
  WorkingEvents = 0;
  printf("StartOS!\n");
  for (i = 0; i < MAX_PRIORITY; i++)
    HeadTasks[i] = -1;
  for (i = 0; i < MAX_TASK; i++) {
    TaskQueue[i].next = i + 1;
    TaskQueue[i].prev = i - 1;
    TaskQueue[i].task_state = TASK_SUSPENDED;
    TaskQueue[i].switch_count = 0;
    TaskQueue[i].waited_resource = -1;
    TaskQueue[i].waiting_events = 0;
  }
  TaskQueue[MAX_TASK - 1].next = 0;
  TaskQueue[0].prev = MAX_TASK - 1;
  if (!setjmp(MainContext))
    ActivateTask(entry, priority, name);
  return 0;
}
void ShutdownOS() {
  printf("ShutdownOS!\n");
}
