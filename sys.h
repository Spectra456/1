#ifndef SYS# define SYS
/****************************************/
/*
sys.h
/
/****************************************/
#include "defs.h"

#include <csetjmp>

enum TTaskState {
  TASK_RUNNING = 0, TASK_READY = 1, TASK_SUSPENDED = 2, TASK_WAITING = 3
};
typedef unsigned int TEventMask;
typedef unsigned int TSemaphore;
typedef struct Type_Task {
  int next, prev;
  int priority;
  int ceiling_priority;
  void( * entry)(void);
  char * name;
  TTaskState task_state;
  int switch_count;
  jmp_buf context;
  TSemaphore waited_resource;
  TEventMask waiting_events;
}
TTask;
typedef struct Type_resource {
  int block;
  //int priority;
  char * name;
}
TResource;
extern TTask TaskQueue[MAX_TASK];
extern TResource ResourceQueue[MAX_RES];
//текущая выполняющаяся задача, изменять только в функции TaskSwitch
extern int RunningTask;
extern int HeadTasks[MAX_PRIORITY]; //первая задача в списке
extern int TaskCount;
extern int FreeTask;
extern TEventMask WorkingEvents; //сработавшие события
extern jmp_buf MainContext; //главный контекст
void Schedule(int task);
void Dispatch();

#endif // SYS
