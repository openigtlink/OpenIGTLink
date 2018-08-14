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
  Module:    $RCSfile: vtkMultiThreader.h,v $

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME igtlMultiThreader - A class for performing multithreaded execution
// .SECTION Description
// igtlMultithreader is a class that provides support for multithreaded
// execution using sproc() on an SGI, or pthread_create on any platform
// supporting POSIX threads.  This class can be used to execute a single
// method on multiple threads, or to specify a method per thread. 

#ifndef __igtlMultiThreader_h
#define __igtlMultiThreader_h

#include "igtlObject.h"
#include "igtlObjectFactory.h"
#include "igtlMacro.h"
#include "igtlMutexLock.h"


#ifdef OpenIGTLink_USE_SPROC
#include <sys/types.h> // Needed for unix implementation of sproc
#include <unistd.h> // Needed for unix implementation of sproc
#endif

#if defined(OpenIGTLink_USE_PTHREAD) || defined(OpenIGTLink_HP_PTHREAD)
#include <pthread.h> // Needed for PTHREAD implementation of mutex
#include <sys/types.h> // Needed for unix implementation of pthreads
#include <unistd.h> // Needed for unix implementation of pthreads
#endif

namespace igtl
{

// If OpenIGTLink_USE_SPROC is defined, then sproc() will be used to create
// multiple threads on an SGI. If OpenIGTLink_USE_PTHREAD is defined, then
// pthread_create() will be used to create multiple threads (on
// a sun, for example)

// Defined in igtlSystemIncludes.h:
//   IGTL_MAX_THREADS

// If OpenIGTLink_USE_PTHREADS is defined, then the multithreaded
// function is of type void *, and returns NULL
// Otherwise the type is void which is correct for WIN32
// and SPROC
//BTX

// The maximum number of threads allowed
#ifdef OpenIGTLink_USE_SPROC
#define IGTL_MAX_THREADS              128
#endif

#ifdef OpenIGTLink_USE_PTHREADS
#define IGTL_MAX_THREADS              128
#endif

#ifdef OpenIGTLink_USE_WIN32_THREADS
#define IGTL_MAX_THREADS              128
#endif

// cygwin threads are unreliable
#ifdef __CYGWIN__
#undef IGTL_MAX_THREADS
#define IGTL_MAX_THREADS 128 
#endif

// mingw threads cause crashes  so limit to 1
#if defined(__MINGW32__)
#undef IGTL_MAX_THREADS
#define IGTL_MAX_THREADS 1 
#endif
  
// On some sgi machines, threads and stl don't mix so limit to 1
#if defined(__sgi) && defined(_COMPILER_VERSION) && _COMPILER_VERSION <= 730
#undef IGTL_MAX_THREADS
#define IGTL_MAX_THREADS 1 
#endif
  
#ifndef IGTL_MAX_THREADS
#define IGTL_MAX_THREADS 1
#endif

#ifdef OpenIGTLink_USE_SPROC
typedef int ThreadProcessIDType;
typedef int MultiThreaderIDType;
#endif

#ifdef OpenIGTLink_USE_PTHREADS
typedef void *(*ThreadFunctionType)(void *);
typedef pthread_t ThreadProcessIDType;
typedef pthread_t MultiThreaderIDType;
#endif

#ifdef OpenIGTLink_USE_WIN32_THREADS
typedef igtlWindowsLPTHREAD_START_ROUTINE ThreadFunctionType;
typedef igtlWindowsHANDLE ThreadProcessIDType;
typedef igtlWindowsDWORD MultiThreaderIDType;
#endif

#if !defined(OpenIGTLink_USE_PTHREADS) && !defined(OpenIGTLink_USE_WIN32_THREADS)
typedef void (*ThreadFunctionType)(void *);
typedef int ThreadProcessIDType;
typedef int MultiThreaderIDType;
#endif
//ETX


class IGTLCommon_EXPORT MultiThreader : public Object 
{
public:
  igtlTypeMacro(MultiThreader, Object);
  igtlNewMacro(Self);  

  // Description:
  // This is the structure that is passed to the thread that is
  // created from the SingleMethodExecute, MultipleMethodExecute or
  // the SpawnThread method. It is passed in as a void *, and it is
  // up to the method to cast correctly and extract the information.
  // The ThreadID is a number between 0 and NumberOfThreads-1 that indicates
  // the id of this thread. The NumberOfThreads is this->NumberOfThreads for
  // threads created from SingleMethodExecute or MultipleMethodExecute,
  // and it is 1 for threads created from SpawnThread.
  // The UserData is the (void *)arg passed into the SetSingleMethod,
  // SetMultipleMethod, or SpawnThread method.

  //BTX
#define ThreadInfoStruct MultiThreader::ThreadInfo
  class ThreadInfo
  {
  public:
    int                 ThreadID;
    int                 NumberOfThreads;
    int                 *ActiveFlag;
    MutexLock::Pointer  ActiveFlagLock;
    void                *UserData;
  };
  //ETX

  // Description:
  // Get/Set the number of threads to create. It will be clamped to the range
  // 1 - IGTL_MAX_THREADS, so the caller of this method should check that the
  // requested number of threads was accepted.
  igtlSetClampMacro( NumberOfThreads, int, 1, IGTL_MAX_THREADS );
  virtual int GetNumberOfThreads();

  // Description:
  // Set/Get the maximum number of threads to use when multithreading.
  // This limits and overrides any other settings for multithreading.
  // A value of zero indicates no limit.
  static void SetGlobalMaximumNumberOfThreads(int val);
  static int  GetGlobalMaximumNumberOfThreads();

  // Description:
  // Set/Get the value which is used to initialize the NumberOfThreads
  // in the constructor.  Initially this default is set to the number of 
  // processors or IGTL_MAX_THREADS (which ever is less).
  static void SetGlobalDefaultNumberOfThreads(int val);
  static int  GetGlobalDefaultNumberOfThreads();

  // These methods are excluded from Tcl wrapping 1) because the
  // wrapper gives up on them and 2) because they really shouldn't be
  // called from a script anyway.
  //BTX 
  
  // Description:
  // Execute the SingleMethod (as define by SetSingleMethod) using
  // this->NumberOfThreads threads.
  void SingleMethodExecute();

  // Description:
  // Execute the MultipleMethods (as define by calling SetMultipleMethod
  // for each of the required this->NumberOfThreads methods) using
  // this->NumberOfThreads threads.
  void MultipleMethodExecute();
  
  // Description:
  // Set the SingleMethod to f() and the UserData field of the
  // ThreadInfo that is passed to it will be data.
  // This method (and all the methods passed to SetMultipleMethod)
  // must be of type ThreadFunctionType and must take a single argument of
  // type void *.
  void SetSingleMethod(ThreadFunctionType, void *data );
 
  // Description:
  // Set the MultipleMethod at the given index to f() and the UserData 
  // field of the ThreadInfo that is passed to it will be data.
  void SetMultipleMethod( int index, ThreadFunctionType, void *data ); 

  // Description:
  // Create a new thread for the given function. Return a thread id
  // which is a number between 0 and IGTL_MAX_THREADS - 1. This id should
  // be used to kill the thread at a later time.
  int SpawnThread( ThreadFunctionType, void *data );

  // Description:
  // Terminate the thread that was created with a SpawnThreadExecute()
  void TerminateThread( int thread_id );

  // Description:
  // Get the thread identifier of the calling thread.
  static MultiThreaderIDType GetCurrentThreadID();

  // Description:
  // Check whether two thread identifiers refer to the same thread.
  static int ThreadsEqual(MultiThreaderIDType t1,
                          MultiThreaderIDType t2);

protected:
  MultiThreader();
  ~MultiThreader();

  void PrintSelf(std::ostream& os) const;

  // The number of threads to use
  int                        m_NumberOfThreads;

  // An array of thread info containing a thread id
  // (0, 1, 2, .. IGTL_MAX_THREADS-1), the thread count, and a pointer
  // to void so that user data can be passed to each thread
  ThreadInfo                 m_ThreadInfoArray[IGTL_MAX_THREADS];

  // The methods
  ThreadFunctionType         m_SingleMethod;
  ThreadFunctionType         m_MultipleMethod[IGTL_MAX_THREADS];

  // Storage of MutexFunctions and ints used to control spawned 
  // threads and the spawned thread ids
  int                        m_SpawnedThreadActiveFlag[IGTL_MAX_THREADS];
  MutexLock::Pointer         m_SpawnedThreadActiveFlagLock[IGTL_MAX_THREADS];
  ThreadProcessIDType        m_SpawnedThreadProcessID[IGTL_MAX_THREADS];
  ThreadInfo                 m_SpawnedThreadInfoArray[IGTL_MAX_THREADS];

//ETX

  // Internal storage of the data
  void                       *m_SingleData;
  void                       *m_MultipleData[IGTL_MAX_THREADS];

private:
  MultiThreader(const MultiThreader&);  // Not implemented.
  void operator=(const MultiThreader&);  // Not implemented.
};

} // namespace igtl
#endif





