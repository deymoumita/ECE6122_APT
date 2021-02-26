#include <pthread.h>
#include <iostream>

using namespace std;

int activeThreads = 0;

pthread_mutex_t coutMutex;
pthread_mutex_t activeMutex;
pthread_cond_t  allDoneCondition;

void* Thread(void* v)
{
  unsigned long myId = (unsigned long)v;
  //pthread_mutex_lock(&activeMutex);
  //activeThreads++;
  //pthread_mutex_unlock(&activeMutex);
  
  pthread_mutex_lock(&coutMutex);
  cout << "Hello from thread " << myId << endl;
  pthread_mutex_unlock(&coutMutex);
  pthread_mutex_lock(&activeMutex);
  activeThreads--;
  if (activeThreads == 0)
   {
     pthread_cond_signal(&allDoneCondition);
   }
  pthread_mutex_unlock(&activeMutex);
}

int main(int argc, char** argv)
{
  int totalThreads = 8;
  pthread_mutex_init(&coutMutex, 0);
  pthread_mutex_init(&activeMutex, 0);
  pthread_cond_init(&allDoneCondition, 0);
  
  pthread_mutex_lock(&activeMutex);
  activeThreads = totalThreads;
  for (int i = 0; i < totalThreads; ++i)
    {
      pthread_t t;
      pthread_create(&t, 0, Thread,  (void*)i);
    }
  cout << "Hello from main before exit" << endl;
  pthread_cond_wait(&allDoneCondition, &activeMutex);
  pthread_mutex_unlock(&activeMutex);
}








