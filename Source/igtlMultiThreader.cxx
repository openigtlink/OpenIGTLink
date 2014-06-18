/*=========================================================================

  Program:   The OpenIGTLink Library
  Language:  C++
  Web page:  http://openigtlink.org/

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkMultiThreader.cxx,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifdef _WIN32
#pragma warning ( disable : 4786 )
#include "igtlWindows.h"
#include <process.h>
#endif


#include "igtlMultiThreader.h"
#include "igtlMutexLock.h"
#include "igtlObjectFactory.h"

// These are the includes necessary for multithreaded rendering on an SGI
// using the sproc() call
#ifdef OpenIGTLink_USE_SPROC
#include <sys/resource.h>
#include <sys/prctl.h>
#include <wait.h>
#include <errno.h>
#endif

#ifdef OpenIGTLink_USE_PTHREADS
#include <unistd.h>
#endif


// Need to define "igtlExternCThreadFunctionType" to avoid warning on some
// platforms about passing function pointer to an argument expecting an
// extern "C" function.  Placing the typedef of the function pointer type
// inside an extern "C" block solves this problem.
#if defined(OpenIGTLink_USE_PTHREADS) || defined(OpenIGTLink_HP_PTHREADS)
#include <pthread.h>
extern "C" { typedef void *(*igtlExternCThreadFunctionType)(void *); }
#else
//typedef igtlThreadFunctionType igtlExternCThreadFunctionType;
#endif

#ifdef __APPLE__
#include <sys/types.h>
#include <sys/sysctl.h>
#endif

namespace igtl {

// Initialize static member that controls global maximum number of threads
static int MultiThreaderGlobalMaximumNumberOfThreads = 0;

void MultiThreader::SetGlobalMaximumNumberOfThreads(int val)
{
  if (val == MultiThreaderGlobalMaximumNumberOfThreads)
    {
    return;
    }
  MultiThreaderGlobalMaximumNumberOfThreads = val;
}

int MultiThreader::GetGlobalMaximumNumberOfThreads()
{
  return MultiThreaderGlobalMaximumNumberOfThreads;
}

// 0 => Not initialized.
static int MultiThreaderGlobalDefaultNumberOfThreads = 0;

void MultiThreader::SetGlobalDefaultNumberOfThreads(int val)
{
  if (val == MultiThreaderGlobalDefaultNumberOfThreads)
    {
    return;
    }
  MultiThreaderGlobalDefaultNumberOfThreads = val;
}

int MultiThreader::GetGlobalDefaultNumberOfThreads()
{
  if (MultiThreaderGlobalDefaultNumberOfThreads == 0)
    {
    int num = 1; // default is 1
#ifdef OpenIGTLink_USE_SPROC
    // Default the number of threads to be the number of available
    // processors if we are using sproc()
    num = prctl( PR_MAXPPROCS );
#endif

#ifdef OpenIGTLink_USE_PTHREADS
    // Default the number of threads to be the number of available
    // processors if we are using pthreads()
#ifdef _SC_NPROCESSORS_ONLN
    num = sysconf( _SC_NPROCESSORS_ONLN );
#elif defined(_SC_NPROC_ONLN)
    num = sysconf( _SC_NPROC_ONLN );
#endif
#if defined(__SVR4) && defined(sun) && defined(PTHREAD_MUTEX_NORMAL)
    pthread_setconcurrency(num);
#endif
#endif

#ifdef __APPLE__
    // Use sysctl() to determine the number of CPUs.  This is prefered
    // over MPProcessors() because it doesn't require CoreServices
    // (which is only available in 32bit on Mac OS X 10.4)
    int mib[2] = {CTL_HW, HW_NCPU};
    size_t dataLen = sizeof(int); // 'num' is an 'int'
    int result = sysctl(mib, 2, &num, &dataLen, NULL, 0);
    if (result == -1)
      {
      num = 1;
      }
#endif

#ifdef _WIN32
    {
      SYSTEM_INFO sysInfo;
      GetSystemInfo(&sysInfo);
      num = sysInfo.dwNumberOfProcessors;
    }
#endif

#ifndef OpenIGTLink_USE_WIN32_THREADS
#ifndef OpenIGTLink_USE_SPROC
#ifndef OpenIGTLink_USE_PTHREADS
    // If we are not multithreading, the number of threads should
    // always be 1
    num = 1;
#endif  
#endif  
#endif
  
    // Lets limit the number of threads to IGTL_MAX_THREADS
    if (num > IGTL_MAX_THREADS)
      {
      num = IGTL_MAX_THREADS;
      }

    MultiThreaderGlobalDefaultNumberOfThreads = num;
    }


  return MultiThreaderGlobalDefaultNumberOfThreads;
}

// Constructor. Default all the methods to NULL. Since the
// ThreadInfoArray is static, the ThreadIDs can be initialized here
// and will not change.
MultiThreader::MultiThreader()
{
  int i;

  for ( i = 0; i < IGTL_MAX_THREADS; i++ )
    {
    this->m_ThreadInfoArray[i].ThreadID           = i;
    this->m_ThreadInfoArray[i].ActiveFlag         = NULL;
    this->m_ThreadInfoArray[i].ActiveFlagLock     = NULL;
    this->m_MultipleMethod[i]                     = NULL;
    this->m_SpawnedThreadActiveFlag[i]            = 0;
    this->m_SpawnedThreadActiveFlagLock[i]        = NULL;
    this->m_SpawnedThreadInfoArray[i].ThreadID    = i;
    }

  this->m_SingleMethod = NULL;
  this->m_NumberOfThreads = 
    MultiThreader::GetGlobalDefaultNumberOfThreads();

}

// Destructor. Nothing allocated so nothing needs to be done here.
MultiThreader::~MultiThreader()
{
}

//----------------------------------------------------------------------------
int MultiThreader::GetNumberOfThreads()
{
  int num = this->m_NumberOfThreads;
  if(MultiThreaderGlobalMaximumNumberOfThreads > 0 &&
     num > MultiThreaderGlobalMaximumNumberOfThreads)
    {
    num = MultiThreaderGlobalMaximumNumberOfThreads;
    }
  return num;
}

// Set the user defined method that will be run on NumberOfThreads threads
// when m_SingleMethodExecute is called.
void MultiThreader::SetSingleMethod( ThreadFunctionType f, 
                                        void *data )
{ 
  this->m_SingleMethod = f;
  this->m_SingleData   = data;
}

// Set one of the user defined methods that will be run on NumberOfThreads
// threads when m_MultipleMethodExecute is called. This method should be
// called with index = 0, 1, ..,  NumberOfThreads-1 to set up all the
// required user defined methods
void MultiThreader::SetMultipleMethod( int index, 
                                          ThreadFunctionType f, void *data )
{ 
  // You can only set the method for 0 through NumberOfThreads-1
  if ( index >= this->m_NumberOfThreads )
    {
    igtlErrorMacro( << "Can't set method " << index << 
                   " with a thread count of " << this->m_NumberOfThreads );
    }
  else
    {
    this->m_MultipleMethod[index] = f;
    this->m_MultipleData[index]   = data;
    }
}

// Execute the method set as the SingleMethod on NumberOfThreads threads.
void MultiThreader::SingleMethodExecute()
{
  int                thread_loop = 0;

#ifdef OpenIGTLink_USE_WIN32_THREADS
  DWORD              threadId;
  HANDLE             process_id[IGTL_MAX_THREADS];
#endif

#ifdef OpenIGTLink_USE_SPROC
  siginfo_t          info_ptr;
  int                process_id[IGTL_MAX_THREADS];
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  pthread_t          process_id[IGTL_MAX_THREADS];
#endif

  if ( !this->m_SingleMethod )
    {
    igtlErrorMacro( << "No single method set!" );
    return;
    }

  // obey the global maximum number of threads limit
  if (MultiThreaderGlobalMaximumNumberOfThreads &&
      this->m_NumberOfThreads > MultiThreaderGlobalMaximumNumberOfThreads)
    {
    this->m_NumberOfThreads = MultiThreaderGlobalMaximumNumberOfThreads;
    }
  
    
  // We are using sproc (on SGIs), pthreads(on Suns), or a single thread
  // (the default)  

#ifdef OpenIGTLink_USE_WIN32_THREADS
  // Using CreateThread on a PC
  //
  // We want to use CreateThread to start this->m_NumberOfThreads - 1 
  // additional threads which will be used to call this->SingleMethod().
  // The parent thread will also call this routine.  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the this->m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the waitid call
  for (thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    this->m_ThreadInfoArray[thread_loop].UserData        = this->m_SingleData;
    this->m_ThreadInfoArray[thread_loop].NumberOfThreads = this->m_NumberOfThreads;
    process_id[thread_loop] = 
      CreateThread(NULL, 0, this->m_SingleMethod, 
             ((void *)(&this->m_ThreadInfoArray[thread_loop])), 0, &threadId);
    if (process_id[thread_loop] == NULL)
      {
      igtlErrorMacro("Error in thread creation !!!");
      } 
    }
  
  // Now, the parent thread calls this->SingleMethod() itself
  this->m_ThreadInfoArray[0].UserData = this->m_SingleData;
  this->m_ThreadInfoArray[0].NumberOfThreads = this->m_NumberOfThreads;
  this->m_SingleMethod((void *)(&this->m_ThreadInfoArray[0]));

  // The parent thread has finished this->SingleMethod() - so now it
  // waits for each of the other processes to exit
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    WaitForSingleObject(process_id[thread_loop], INFINITE);
    }

  // close the threads
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    CloseHandle(process_id[thread_loop]);
    }
#endif

#ifdef OpenIGTLink_USE_SPROC
  // Using sproc() on an SGI
  //
  // We want to use sproc to start this->m_NumberOfThreads - 1 additional
  // threads which will be used to call this->SingleMethod(). The
  // parent thread will also call this routine.  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the this->m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the waitid call

  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    this->m_ThreadInfoArray[thread_loop].UserData        = this->m_SingleData;
    this->m_ThreadInfoArray[thread_loop].NumberOfThreads = this->m_NumberOfThreads;
    process_id[thread_loop] = 
      sproc( this->m_SingleMethod, PR_SADDR, 
             ( (void *)(&this->m_ThreadInfoArray[thread_loop]) ) );
    if ( process_id[thread_loop] == -1)
      {
      igtlErrorMacro("sproc call failed. Code: " << errno << endl);
      }
    }
  
  // Now, the parent thread calls this->m_SingleMethod() itself
  this->m_ThreadInfoArray[0].UserData        = this->m_SingleData;
  this->m_ThreadInfoArray[0].NumberOfThreads = this->m_NumberOfThreads;
  this->m_SingleMethod((void *)(&this->m_ThreadInfoArray[0]) );

  // The parent thread has finished this->m_SingleMethod() - so now it
  // waits for each of the other processes to exit
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    waitid( P_PID, (id_t) process_id[thread_loop], &info_ptr, WEXITED );
    }
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  // Using POSIX threads
  //
  // We want to use pthread_create to start this->m_NumberOfThreads-1 additional
  // threads which will be used to call this->m_SingleMethod(). The
  // parent thread will also call this routine.  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the this->m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the pthread_join call

  pthread_attr_t attr;

#ifdef OpenIGTLink_HP_PTHREADS
  pthread_attr_create( &attr );
#else  
  pthread_attr_init(&attr);
#if !defined(__CYGWIN__)
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
#endif
#endif
  
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    this->m_ThreadInfoArray[thread_loop].UserData        = this->m_SingleData;
    this->m_ThreadInfoArray[thread_loop].NumberOfThreads = this->m_NumberOfThreads;

#ifdef OpenIGTLink_HP_PTHREADS
    pthread_create( &(process_id[thread_loop]),
                    attr, this->m_SingleMethod,  
                    ( (void *)(&this->m_ThreadInfoArray[thread_loop]) ) );
#else
    int                threadError;
    threadError =
      pthread_create( &(process_id[thread_loop]), &attr,
                      reinterpret_cast<igtlExternCThreadFunctionType>(
                        this->m_SingleMethod),
                      ( (void *)(&this->m_ThreadInfoArray[thread_loop]) ) );
    if (threadError != 0)
      {
      igtlErrorMacro(<< "Unable to create a thread.  pthread_create() returned "
                    << threadError);
      }
#endif
    }
  
  // Now, the parent thread calls this->m_SingleMethod() itself
  this->m_ThreadInfoArray[0].UserData        = this->m_SingleData;
  this->m_ThreadInfoArray[0].NumberOfThreads = this->m_NumberOfThreads;
  this->m_SingleMethod((void *)(&this->m_ThreadInfoArray[0]) );

  // The parent thread has finished this->m_SingleMethod() - so now it
  // waits for each of the other processes to exit
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    pthread_join( process_id[thread_loop], NULL );
    }
#endif

#ifndef OpenIGTLink_USE_WIN32_THREADS
#ifndef OpenIGTLink_USE_SPROC
#ifndef OpenIGTLink_USE_PTHREADS
  // There is no multi threading, so there is only one thread.
  this->m_ThreadInfoArray[0].UserData        = this->m_SingleData;
  this->m_ThreadInfoArray[0].NumberOfThreads = this->m_NumberOfThreads;
  this->m_SingleMethod( (void *)(&this->m_ThreadInfoArray[0]) );
#endif
#endif
#endif
}

void MultiThreader::MultipleMethodExecute()
{
  int                thread_loop;

#ifdef OpenIGTLink_USE_WIN32_THREADS
  DWORD              threadId;
  HANDLE             process_id[IGTL_MAX_THREADS];
#endif

#ifdef OpenIGTLink_USE_SPROC
  siginfo_t          info_ptr;
  int                process_id[IGTL_MAX_THREADS];
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  pthread_t          process_id[IGTL_MAX_THREADS];
#endif


  // obey the global maximum number of threads limit
  if (MultiThreaderGlobalMaximumNumberOfThreads &&
      this->m_NumberOfThreads > MultiThreaderGlobalMaximumNumberOfThreads)
    {
    this->m_NumberOfThreads = MultiThreaderGlobalMaximumNumberOfThreads;
    }

  for ( thread_loop = 0; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    if ( this->m_MultipleMethod[thread_loop] == (ThreadFunctionType)NULL)
      {
      igtlErrorMacro( << "No multiple method set for: " << thread_loop );
      return;
      }
    }

  // We are using sproc (on SGIs), pthreads(on Suns), CreateThread
  // on a PC or a single thread (the default)  

#ifdef OpenIGTLink_USE_WIN32_THREADS
  // Using CreateThread on a PC
  //
  // We want to use CreateThread to start this->m_NumberOfThreads - 1 
  // additional threads which will be used to call the m_NumberOfThreads-1
  // methods defined in this->m_MultipleMethods[](). The parent thread
  // will call this->m_MultipleMethods[m_NumberOfThreads-1]().  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the this->m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the waitid call
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    this->m_ThreadInfoArray[thread_loop].UserData = 
      this->m_MultipleData[thread_loop];
    this->m_ThreadInfoArray[thread_loop].NumberOfThreads = this->m_NumberOfThreads;
    process_id[thread_loop] = 
      CreateThread(NULL, 0, this->m_MultipleMethod[thread_loop], 
             ((void *)(&this->m_ThreadInfoArray[thread_loop])), 0, &threadId);
    if (process_id[thread_loop] == NULL)
      {
      igtlErrorMacro("Error in thread creation !!!");
      } 
    }
  
  // Now, the parent thread calls the last method itself
  this->m_ThreadInfoArray[0].UserData        = this->m_MultipleData[0];
  this->m_ThreadInfoArray[0].NumberOfThreads = this->m_NumberOfThreads;
  (this->m_MultipleMethod[0])((void *)(&this->m_ThreadInfoArray[0]) );

  // The parent thread has finished its method - so now it
  // waits for each of the other processes (created with sproc) to
  // exit
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    WaitForSingleObject(process_id[thread_loop], INFINITE);
    }

  // close the threads
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    CloseHandle(process_id[thread_loop]);
    }
#endif

#ifdef OpenIGTLink_USE_SPROC
  // Using sproc() on an SGI
  //
  // We want to use sproc to start this->m_NumberOfThreads - 1 additional
  // threads which will be used to call the m_NumberOfThreads-1 methods
  // defined in this->m_MultipleMethods[](). The parent thread
  // will call this->m_MultipleMethods[m_NumberOfThreads-1]().  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the this->m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the waitid call
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    this->m_ThreadInfoArray[thread_loop].UserData = 
      this->m_MultipleData[thread_loop];
    this->m_ThreadInfoArray[thread_loop].NumberOfThreads = this->m_NumberOfThreads;
    process_id[thread_loop] = 
      sproc( this->m_MultipleMethod[thread_loop], PR_SADDR, 
             ( (void *)(&this->m_ThreadInfoArray[thread_loop]) ) );
    }
  
  // Now, the parent thread calls the last method itself
  this->m_ThreadInfoArray[0].UserData        = this->m_MultipleData[0];
  this->m_ThreadInfoArray[0].NumberOfThreads = this->m_NumberOfThreads;
  (this->m_MultipleMethod[0])((void *)(&this->m_ThreadInfoArray[0]) );

  // The parent thread has finished its method - so now it
  // waits for each of the other processes (created with sproc) to
  // exit
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    waitid( P_PID, (id_t) process_id[thread_loop], &info_ptr, WEXITED );
    }
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  // Using POSIX threads
  //
  // We want to use pthread_create to start this->m_NumberOfThreads - 1 
  // additional
  // threads which will be used to call the NumberOfThreads-1 methods
  // defined in this->m_MultipleMethods[](). The parent thread
  // will call this->m_MultipleMethods[NumberOfThreads-1]().  When it is done,
  // it will wait for all the children to finish. 
  //
  // First, start up the this->m_NumberOfThreads-1 processes.  Keep track
  // of their process ids for use later in the pthread_join call

  pthread_attr_t attr;

#ifdef OpenIGTLink_HP_PTHREADS
  pthread_attr_create( &attr );
#else  
  pthread_attr_init(&attr);
#ifndef __CYGWIN__
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
#endif
#endif

  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    this->m_ThreadInfoArray[thread_loop].UserData = 
      this->m_MultipleData[thread_loop];
    this->m_ThreadInfoArray[thread_loop].NumberOfThreads = this->m_NumberOfThreads;
#ifdef OpenIGTLink_HP_PTHREADS
    pthread_create( &(process_id[thread_loop]),
                    attr, this->m_MultipleMethod[thread_loop],  
                    ( (void *)(&this->m_ThreadInfoArray[thread_loop]) ) );
#else
    pthread_create( &(process_id[thread_loop]),
                    &attr,
                    reinterpret_cast<igtlExternCThreadFunctionType>(
                      this->m_MultipleMethod[thread_loop]),
                    ( (void *)(&this->m_ThreadInfoArray[thread_loop]) ) );
#endif
    }
  
  // Now, the parent thread calls the last method itself
  this->m_ThreadInfoArray[0].UserData = this->m_MultipleData[0];
  this->m_ThreadInfoArray[0].NumberOfThreads = this->m_NumberOfThreads;
  (this->m_MultipleMethod[0])((void *)(&this->m_ThreadInfoArray[0]) );

  // The parent thread has finished its method - so now it
  // waits for each of the other processes to exit
  for ( thread_loop = 1; thread_loop < this->m_NumberOfThreads; thread_loop++ )
    {
    pthread_join( process_id[thread_loop], NULL );
    }
#endif

#ifndef OpenIGTLink_USE_WIN32_THREADS
#ifndef OpenIGTLink_USE_SPROC
#ifndef OpenIGTLink_USE_PTHREADS
  // There is no multi threading, so there is only one thread.
  this->m_ThreadInfoArray[0].UserData        = this->m_MultipleData[0];
  this->m_ThreadInfoArray[0].NumberOfThreads = this->m_NumberOfThreads;
  (this->m_MultipleMethod[0])( (void *)(&this->m_ThreadInfoArray[0]) );
#endif
#endif
#endif
}

int MultiThreader::SpawnThread( ThreadFunctionType f, void *userdata )
{
  int id = 0;

#ifdef OpenIGTLink_USE_WIN32_THREADS
  DWORD              threadId;
#endif

  for ( id = 0; id < IGTL_MAX_THREADS; id++ )
    {
    if ( ! this->m_SpawnedThreadActiveFlagLock[id] )
      {
      this->m_SpawnedThreadActiveFlagLock[id] = MutexLock::New();
      }
    this->m_SpawnedThreadActiveFlagLock[id]->Lock();
    if (this->m_SpawnedThreadActiveFlag[id] == 0)
      {
      // We've got a useable thread id, so grab it
      this->m_SpawnedThreadActiveFlag[id] = 1;
      this->m_SpawnedThreadActiveFlagLock[id]->Unlock();
      break;
      }
    this->m_SpawnedThreadActiveFlagLock[id]->Unlock();
    }

  if ( id >= IGTL_MAX_THREADS )
    {
    igtlErrorMacro( << "You have too many active threads!" );
    return -1;
    }

  this->m_SpawnedThreadInfoArray[id].UserData        = userdata;
  this->m_SpawnedThreadInfoArray[id].NumberOfThreads = 1;
  this->m_SpawnedThreadInfoArray[id].ActiveFlag = 
    &this->m_SpawnedThreadActiveFlag[id];
  this->m_SpawnedThreadInfoArray[id].ActiveFlagLock = 
    this->m_SpawnedThreadActiveFlagLock[id];

  // We are using sproc (on SGIs), pthreads(on Suns or HPs), 
  // CreateThread (on win32), or generating an error  

#ifdef OpenIGTLink_USE_WIN32_THREADS
  // Using CreateThread on a PC
  //
  this->m_SpawnedThreadProcessID[id] = 
      CreateThread(NULL, 0, f, 
             ((void *)(&this->m_SpawnedThreadInfoArray[id])), 0, &threadId);
  if (this->m_SpawnedThreadProcessID[id] == NULL)
    {
    igtlErrorMacro("Error in thread creation !!!");
    } 
#endif

#ifdef OpenIGTLink_USE_SPROC
  // Using sproc() on an SGI
  //
  this->m_SpawnedThreadProcessID[id] = 
    sproc( f, PR_SADDR, ( (void *)(&this->m_SpawnedThreadInfoArray[id]) ) );

#endif

#ifdef OpenIGTLink_USE_PTHREADS
  // Using POSIX threads
  //
  pthread_attr_t attr;

#ifdef OpenIGTLink_HP_PTHREADS
  pthread_attr_create( &attr );
#else  
  pthread_attr_init(&attr);
#ifndef __CYGWIN__
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_PROCESS);
#endif
#endif
  
#ifdef OpenIGTLink_HP_PTHREADS
  pthread_create( &(this->m_SpawnedThreadProcessID[id]),
                  attr, f,  
                  ( (void *)(&this->m_SpawnedThreadInfoArray[id]) ) );
#else

  pthread_create( &(this->m_SpawnedThreadProcessID[id]),
                  &attr,
                  reinterpret_cast<igtlExternCThreadFunctionType>(f),  
                  ( (void *)(&this->m_SpawnedThreadInfoArray[id]) ) );
#endif

#endif

#ifndef OpenIGTLink_USE_WIN32_THREADS
#ifndef OpenIGTLink_USE_SPROC
#ifndef OpenIGTLink_USE_PTHREADS
  // There is no multi threading, so there is only one thread.
  // This won't work - so give an error message.
  igtlErrorMacro( << "Cannot spawn thread in a single threaded environment!" );
  this->m_SpawnedThreadActiveFlagLock[id] = 0;
  id = -1;
#endif
#endif
#endif

  return id;
}

void MultiThreader::TerminateThread( int threadID )
{

  if ( !this->m_SpawnedThreadActiveFlag[threadID] )
    {
    return;
    }

  this->m_SpawnedThreadActiveFlagLock[threadID]->Lock();
  this->m_SpawnedThreadActiveFlag[threadID] = 0;
  this->m_SpawnedThreadActiveFlagLock[threadID]->Unlock();

#ifdef OpenIGTLink_USE_WIN32_THREADS
  WaitForSingleObject(this->m_SpawnedThreadProcessID[threadID], INFINITE);
  CloseHandle(this->m_SpawnedThreadProcessID[threadID]);
#endif

#ifdef OpenIGTLink_USE_SPROC
  siginfo_t info_ptr;

  waitid( P_PID, (id_t) this->m_SpawnedThreadProcessID[threadID], 
          &info_ptr, WEXITED );
#endif

#ifdef OpenIGTLink_USE_PTHREADS
  pthread_join( this->m_SpawnedThreadProcessID[threadID], NULL );
#endif

#ifndef OpenIGTLink_USE_WIN32_THREADS
#ifndef OpenIGTLink_USE_SPROC
#ifndef OpenIGTLink_USE_PTHREADS
  // There is no multi threading, so there is only one thread.
  // This won't work - so give an error message.
  igtlErrorMacro(<< "Cannot terminate thread in single threaded environment!");
#endif
#endif
#endif

  this->m_SpawnedThreadActiveFlagLock[threadID] = 0;
  this->m_SpawnedThreadActiveFlagLock[threadID] = NULL;

}

//----------------------------------------------------------------------------
MultiThreaderIDType MultiThreader::GetCurrentThreadID()
{
#if defined(OpenIGTLink_USE_PTHREADS)
  return pthread_self();
#elif defined(OpenIGTLink_USE_WIN32_THREADS)
  return GetCurrentThreadId();
#elif defined(OpenIGTLink_USE_SPROC)
  return getpid();
#else
  // No threading implementation.  Assume all callers are in the same
  // thread.
  return 0;
#endif
}

//----------------------------------------------------------------------------
int MultiThreader::ThreadsEqual(MultiThreaderIDType t1,
                                   MultiThreaderIDType t2)
{
#if defined(OpenIGTLink_USE_PTHREADS)
  return pthread_equal(t1, t2) != 0;
#elif defined(OpenIGTLink_USE_WIN32_THREADS)
  return t1 == t2;
#elif defined(OpenIGTLink_USE_SPROC)
  return t1 == t2;
#else
  // No threading implementation.  Assume all callers are in the same
  // thread.
  return 1;
#endif
}

// Print method for the multithreader
void MultiThreader::PrintSelf(std::ostream& os) const
{
  this->Superclass::PrintSelf(os); 

  const char* indent = "    ";
  os << indent << "Thread Count: " << this->m_NumberOfThreads << "\n";
  os << indent << "Global Maximum Number Of Threads: " << 
    MultiThreaderGlobalMaximumNumberOfThreads << std::endl;
  os << "Thread system used: " <<
#ifdef OpenIGTLink_USE_PTHREADS  
   "PTHREADS"
#elif defined OpenIGTLink_USE_SPROC
    "SPROC"
#elif defined OpenIGTLink_USE_WIN32_THREADS
    "WIN32 Threads"
#elif defined OpenIGTLink_HP_PTHREADS
    "HP PThreads"
#else
    "NO THREADS SUPPORT"
#endif
     << std::endl;
}


} // namespace igtl
