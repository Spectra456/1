#ifndef RTOS_API
#define RTOS_API

#define DeclareTask(TaskID,priority)\
TASK(TaskID);   \
enum {TaskID##prior=priority}


#define TASK(TaskID) void TaskID(void)
typedef void TTaskCall(void);
int ActivateTask(TTaskCall entry,int priority,char* name);
void TerminateTask(void);
int StartOS(TTaskCall entry,int priority,char* name);
void ShutdownOS();
void InitPVS(TSemaphore S, char *name);
void P(TSemaphore S);
void V(TSemaphore S);
#define DeclareSysEvent(ID) \
const int Event_##ID = (ID)*(ID);
void SetSysEvent(TEventMask mask);
void GetSysEvent(TEventMask* mask);
void WaitSysEvent(TEventMask mask);

#endif // RTOS_API

