#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <pthread.h>
#include <mntent.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>
#include <poll.h>
#include <errno.h>
#include <csignal>
#include <mqueue.h>
#include <sys/resource.h>
#include <cerrno>

using namespace std;

int msgid;
key_t key;

struct Targets{
    bool flag;
    int number;
};

struct Message{
   long mtype;
   int priority;
};

struct Message message;

void *ThreadFunction1(void *value1){
    Targets *val1 = (Targets*) value1;
    cout<<"Thread 1 work was started\n";
    while (val1->flag == true) {
        int priority = getpriority(PRIO_PROCESS, 0);
        if (priority == -1) {
        perror("Failed to get thread priority");
        } else {
        cout << "Thread priority: " << priority << endl;
        }
        message.mtype = 1;
        message.priority = priority;

        if (msgsnd(msgid, &message, sizeof(int), IPC_NOWAIT)){
           cout << "msgsnd: " << strerror(errno) << endl;
        }
        sleep(1);
    }
    cout<<"Thread 1 work was finished\n";
    pthread_exit((void*) 3);
}


int main(){
   pthread_t id1;
   Targets val1;
   int *exitcode1;
   val1.flag = true;
   val1.number = 1;

   key = ftok("lab7", 'A');
   if (key == -1){
      cout << "ftok: " <<strerror(errno) << endl;
   } else {
      cout << "key was generated" << endl;
   }

   msgid = msgget(key, 0);
   if (msgid < 0){
      cout<< "msgget: " << strerror(errno)<<"\n";
      msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    }

   if (pthread_create(&id1, NULL, ThreadFunction1, &val1)){
        perror("pthread_create ");
        return 1;
    }
    getchar();
    cout << "Enter was pressed" << endl;
    val1.flag = false;

    if (pthread_join(id1, (void**)&exitcode1)){
        cout << "pthread_join: " << strerror(errno) << endl;
    }
    else{
    cout<<"Exitcode 1: "<< exitcode1 <<endl;
    }

    if (msgctl(msgid, IPC_RMID, NULL) == -1){
       cout << "msgctl: " << strerror(errno) << endl;
    } else {
       cout << "Queue was illuminated" << endl;
    }

    return 0;
}
