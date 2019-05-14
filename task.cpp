/*********************************/
/*
task.c
*/
/*********************************/
#include <stdio.h>

#include "sys.h"

#include "rtos_api.h"
 // добавить задачу перед item, head указывает на голову списка
void InsertTaskBefore(int task, int item, int * head) {
  if (TaskQueue[task].next != -1)
    return;
  if (item == * head)
    *
    head = task;
  TaskQueue[task].next = item;
  TaskQueue[task].prev = TaskQueue[item].prev;
  TaskQueue[TaskQueue[item].prev].next = task;
  TaskQueue[item].prev = task;
}
void InsertTaskAfter(int task, int item) {
  if (TaskQueue[task].next != -1)
    return;
  TaskQueue[task].next = TaskQueue[item].next;
  TaskQueue[task].prev = item;
  TaskQueue[TaskQueue[item].next].prev = task;
  TaskQueue[item].next = task;
}
void RemoveTask(int task, int * head) {
  if (TaskQueue[task].next == -1)
    return;
  if ( * head == task) {
    if (TaskQueue[task].next == task)
      *
      head = -1;
    else
      *head = TaskQueue[task].next;
  }
  TaskQueue[TaskQueue[task].prev].next = TaskQueue[task].next;
  TaskQueue[TaskQueue[task].next].prev = TaskQueue[task].prev;
  TaskQueue[task].next = -1;
  TaskQueue[task].prev = -1;
}
int ActivateTask(TTaskCall entry, int priority, char * name) {
  int task, occupy;
  printf("ActivateTask %s\n", name);
  task = HeadTasks[priority];
  occupy = FreeTask;
  //изменяем список свободных задач
  RemoveTask(occupy, & FreeTask);
  TaskQueue[occupy].priority = priority;
  TaskQueue[occupy].ceiling_priority = priority;
  TaskQueue[occupy].name = name;
  TaskQueue[occupy].entry = entry;
  TaskQueue[occupy].switch_count = 0;
  TaskQueue[occupy].task_state = TASK_READY;
  TaskCount++;
  printf("End of ActivateTask %s\n", name);
  Schedule(occupy);
  if (task != HeadTasks[priority]) {
    Dispatch();
  }
  return occupy;
}
void TerminateTask(void) {
  int task;
  TaskCount--;
  task = RunningTask;
  printf("TerminateTask %s\n", TaskQueue[task].name);
  TaskQueue[task].task_state = TASK_SUSPENDED;
  RemoveTask(task, & (HeadTasks[TaskQueue[task].ceiling_priority]));
  //добавляем задачу в список свободных
  InsertTaskBefore(task, FreeTask, & FreeTask);
  printf("End of TerminateTask %s\n", TaskQueue[task].name);
  //задач не осталось, возвращаемся
  if (TaskCount == 0)
    longjmp(MainContext, 1);
  Dispatch();
}
void Schedule(int task) {
  int priority;
  if (TaskQueue[task].task_state == TASK_SUSPENDED)
    return;
  printf("Schedule %s\n", TaskQueue[task].name);
  priority = TaskQueue[task].ceiling_priority;
  RemoveTask(task, & (HeadTasks[priority]));
  if (HeadTasks[priority] == -1) {
    HeadTasks[priority] = task;
    TaskQueue[task].next = task;
    TaskQueue[task].prev = task;
  } else
    InsertTaskAfter(task, TaskQueue[HeadTasks[priority]].prev);
  printf("End of Schedule %s\n", TaskQueue[task].name);
}
void TaskSwitch(int nextTask) {
  if (nextTask == -1)
    return;
  if (RunningTask != -1 && TaskQueue[RunningTask].task_state == TASK_RUNNING)
    TaskQueue[RunningTask].task_state = TASK_READY;
  TaskQueue[nextTask].task_state = TASK_RUNNING;
  RunningTask = nextTask;
  TaskQueue[nextTask].switch_count++;
  if (TaskQueue[nextTask].switch_count == 1) {
    printf("Dispatch - run task %s\n", TaskQueue[nextTask].name);
    //в первый раз
    TaskQueue[nextTask].entry();
  } else {
    printf("Dispatch - task switch to %s\n", TaskQueue[nextTask].name);
    //контекст задачи уже существует
    longjmp(TaskQueue[nextTask].context, 1);
  }
}
// резервируем область стека для задачи
void TaskSwitchWithCushion(int nextTask) {
  char space[100000];
  space[99999] = 1; // отключаем оптимизацию массивов
  TaskSwitch(nextTask);
}
void Dispatch() {
  if (RunningTask != -1)
    printf("Dispatch - %s\n", TaskQueue[RunningTask].name);
  else
    printf("Dispatch\n");
  int nextTask = HeadTasks[0];
  int priority = 0;
  while (TaskCount) {
    if (nextTask != -1) {
      if (TaskCount == 1 && TaskQueue[nextTask].task_state != TASK_READY)
        break;
      if (TaskQueue[nextTask].task_state == TASK_READY || TaskQueue[nextTask].task_state ==
        TASK_RUNNING) {
        //переключаемся на следующую задачу
        if (RunningTask == -1 || TaskQueue[RunningTask].task_state ==
          TASK_SUSPENDED)
          TaskSwitch(nextTask);
        else if (RunningTask != nextTask)
          if (!setjmp(TaskQueue[RunningTask].context)) {
            if (TaskQueue[RunningTask].switch_count == 1)
              TaskSwitchWithCushion(nextTask);
            else
              TaskSwitch(nextTask);
          }
        break;
      }
      nextTask = TaskQueue[nextTask].next;
    }
    if (nextTask == -1 || nextTask == HeadTasks[priority]) { //переключаемся на следующий приоритет
      priority++;
      if (priority < MAX_PRIORITY)
        nextTask = HeadTasks[priority];
      else {
        printf("Error - dont have ready tasks\n");
        longjmp(MainContext, 1);
      }
    }
  }
  //добавляем задачу в конец списка
  RemoveTask(nextTask, & (HeadTasks[priority]));
  if (HeadTasks[priority] == -1)
    HeadTasks[priority] = TaskQueue[nextTask].next = TaskQueue[nextTask].prev = nextTask;
  else
    InsertTaskAfter(nextTask, TaskQueue[HeadTasks[priority]].prev);
  printf("End of Dispatch - %s\n", TaskQueue[RunningTask].name);
}
