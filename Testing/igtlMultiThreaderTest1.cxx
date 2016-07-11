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
//  MultiThreader Test 1 --- Single Method Test
//
//  This test check the behaviour of igtl::MultiThreader::SingleMethodExecute()
//  and igtl::MutexLock.
//  The test create NUM_THREAD threads from a single method that repeats:
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


void* ThreadFunction(void* ptr)
{
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info = 
    static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  int id = info->ThreadID;

  // Set interval at 100 * id (ms)
  long interval = 10*id;

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


int main(int, char * [] )
{
  igtl::MutexLock::Pointer glock = igtl::MutexLock::New();

  int sum = 1;

  ThreadData td;
  td.nloop = NUM_REPEAT;
  td.sum   = &sum;
  td.glock = glock;
  
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();

  threader->SetNumberOfThreads(NUM_THREAD);
  threader->SetSingleMethod((igtl::ThreadFunctionType) &ThreadFunction, &td);
  threader->SingleMethodExecute();

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





