/*=========================================================================

  Program:   OpenIGTLink Library
  Language:  C++

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


//=========================================================================
//
//  MultiThreader Test 2 --- Multiple Method Test
//
//  This test check the behaviour of igtl::MultiThreader::MultipleMethodExecute()
//  and igtl::MutexLock.
//  The test create NUM_THREAD threads from methods that repeats:
//
//     s1 = s;
//     sleep(interval)
//     s2 = s;
//     s = s1 + s2;
//
//  for NUM_REPEAT times, where 's' is a global variable (shared by all
//  threads) initialized with 1, and 's1', 's2' and 'interval' are local
//  variables. 'interval' differs from thread to thread.
//  If the threads work correctly, and the code block above is properly
//  protected by a semaphore, 's' finally becomes 2^(NUM_REPEAT * NUM_THREAD-1).
//  
//=========================================================================


#include "igtlMultiThreader.h"
#include "igtlOSUtil.h"

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

// NOTE: (NUM_THREAD + NUM_REPEAT) < 32 on 32-bit environment
#if IGTL_MAX_THREADS > 1
  #define NUM_THREAD   5
  #define NUM_REPEAT   4
#else
  #define NUM_THREAD   1
  #define NUM_REPEAT   4
#endif


typedef struct {
  int   nloop;
  int   *sum;
  igtl::MutexLock::Pointer glock;
} ThreadData;


void* ThreadFunction1(void* ptr)
{
  long interval = 10; // (ms)

  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  ThreadData* data = static_cast<ThreadData*>(info->UserData);
  int nloop = data->nloop;
  igtl::MutexLock::Pointer glock = data->glock;

  for (int i = 0; i < nloop; i ++)
    {
    glock->Lock();
    int s1 = *(data->sum);
    igtl::Sleep(interval);
    int s2 = *(data->sum);
    *(data->sum) = s1 + s2;
    glock->Unlock();
    }

  return NULL;
}

void* ThreadFunction2(void* ptr)
{
  long interval = 20; // (ms)

  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  ThreadData* data = static_cast<ThreadData*>(info->UserData);
  int nloop = data->nloop;
  igtl::MutexLock::Pointer glock = data->glock;

  for (int i = 0; i < nloop; i ++)
    {
    glock->Lock();
    int s1 = *(data->sum);
    igtl::Sleep(interval);
    int s2 = *(data->sum);
    *(data->sum) = s1 + s2;
    glock->Unlock();
    }

  return NULL;
}

void* ThreadFunction3(void* ptr)
{
  long interval = 30; // (ms)

  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  ThreadData* data = static_cast<ThreadData*>(info->UserData);
  int nloop = data->nloop;
  igtl::MutexLock::Pointer glock = data->glock;

  for (int i = 0; i < nloop; i ++)
    {
    glock->Lock();
    int s1 = *(data->sum);
    igtl::Sleep(interval);
    int s2 = *(data->sum);
    *(data->sum) = s1 + s2;
    glock->Unlock();
    }

  return NULL;
}

void* ThreadFunction4(void* ptr)
{
  long interval = 40; // (ms)

  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  ThreadData* data = static_cast<ThreadData*>(info->UserData);
  int nloop = data->nloop;
  igtl::MutexLock::Pointer glock = data->glock;

  for (int i = 0; i < nloop; i ++)
    {
    glock->Lock();
    int s1 = *(data->sum);
    igtl::Sleep(interval);
    int s2 = *(data->sum);
    *(data->sum) = s1 + s2;
    glock->Unlock();
    }

  return NULL;
}


void* ThreadFunction5(void* ptr)
{
  long interval = 50; // (ms)

  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);

  ThreadData* data = static_cast<ThreadData*>(info->UserData);
  int nloop = data->nloop;
  igtl::MutexLock::Pointer glock = data->glock;

  for (int i = 0; i < nloop; i ++)
    {
    glock->Lock();
    int s1 = *(data->sum);
    igtl::Sleep(interval);
    int s2 = *(data->sum);
    *(data->sum) = s1 + s2;
    glock->Unlock();
    }

  return NULL;
}


int main(int , char * [] )
{

  igtl::MutexLock::Pointer glock = igtl::MutexLock::New();

  int sum = 1;

  ThreadData td;
  td.nloop = NUM_REPEAT;
  td.sum   = &sum;
  td.glock = glock;
  
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();

  threader->SetNumberOfThreads(NUM_THREAD);
  threader->SetMultipleMethod(0, (igtl::ThreadFunctionType) &ThreadFunction1, &td);
#if IGTL_MAX_THREADS > 1
  threader->SetMultipleMethod(1, (igtl::ThreadFunctionType) &ThreadFunction2, &td);
  threader->SetMultipleMethod(2, (igtl::ThreadFunctionType) &ThreadFunction3, &td);
  threader->SetMultipleMethod(3, (igtl::ThreadFunctionType) &ThreadFunction4, &td);
  threader->SetMultipleMethod(4, (igtl::ThreadFunctionType) &ThreadFunction5, &td);
#endif
  threader->MultipleMethodExecute();

  int answer = 0x00000001;
  answer <<= (NUM_THREAD * NUM_REPEAT);
  
  //std::cerr << "sum = " << sum << "  answer = " << answer << std::endl;

  if (sum == answer)
    {
    return EXIT_SUCCESS;
    }
  else
    {
    return EXIT_FAILURE;
    }
}





