/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "igtlConditionVariable.h"
#include "igtlMultiThreader.h"
#include "igtlOSUtil.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"


void* TestThreadFunction1(void* ptr);
void* TestThreadFunction2(void* ptr);
void* TestThreadWaiting1(void* ptr);
void* TestThreadWaiting2(void* ptr);

int icount1 = 0, icount2 = 0, icountBroadcast = 0;

typedef struct {
  igtl::SimpleMutexLock* glock;
  igtl::ConditionVariable::Pointer conditionVar;
  bool isBroadCast;
  int iFinalCount;
  bool condition;
  std::vector<bool> threads;
} ThreadData;

TEST(ConditionVariableTest, SingleChildThreadTest)
{
  // From this test we can see that, the wait function really works,
  // the main thread waits until the spawned thread is finished, otherwize the icount1 will be 0.
  igtl::ConditionVariable::Pointer conditionVar = igtl::ConditionVariable::New();
  igtl::SimpleMutexLock *localMutex = new igtl::SimpleMutexLock();
  
  ThreadData td;
  td.threads = std::vector<bool>(1);
  td.conditionVar = conditionVar;
  td.glock = localMutex;
  td.isBroadCast = false;
  td.iFinalCount = 0;
  td.condition = false;
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadFunction1, &td);
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadWaiting1, &td);
  while (td.threads[0] == false)
    igtl::Sleep(20);
  std::cerr<<"The child thread is released";
  EXPECT_EQ(td.iFinalCount, 10);
}

TEST(ConditionVariableTest, MultiChildThreadTest)
{
  igtl::ConditionVariable::Pointer conditionVar1 = igtl::ConditionVariable::New();
  igtl::SimpleMutexLock *localMutex1 = new igtl::SimpleMutexLock();
  ThreadData td1;
  td1.conditionVar = conditionVar1;
  td1.glock = localMutex1;
  td1.isBroadCast = false;
  td1.threads = std::vector<bool>(2,false);
  td1.iFinalCount = 0;
  td1.condition = false;
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadWaiting1, &td1);
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadFunction1, &td1);
  igtl::Sleep(1000);
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadWaiting2, &td1);
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadFunction2, &td1);
  while (td1.threads[0] == false || td1.threads[1] == false)
    igtl::Sleep(20);
  EXPECT_EQ(td1.iFinalCount, 20);
  std::cerr<<"The child threads are released";
}

TEST(ConditionVariableTest, Broadcast)
{
  igtl::ConditionVariable::Pointer conditionVar1 = igtl::ConditionVariable::New();
  igtl::SimpleMutexLock *localMutex1 = new igtl::SimpleMutexLock();
  ThreadData td1;
  td1.conditionVar = conditionVar1;
  td1.glock = localMutex1;
  td1.isBroadCast = true;
  td1.threads = std::vector<bool>(2);
  td1.iFinalCount = 0;
  td1.condition = false;
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadWaiting1, &td1);
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadFunction1, &td1);
  igtl::Sleep(1000);
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadWaiting2, &td1);
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadFunction2, &td1);
  while (td1.threads[0] == false || td1.threads[1] == false)
    igtl::Sleep(20);
  EXPECT_LT(td1.iFinalCount, 20);
  std::cerr<<"The child threads are released";
}

void* TestThreadWaiting1(void* ptr)
{
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  ThreadData* td = static_cast<ThreadData*>(info->UserData);
  td->glock->Lock();
  while(!td->condition)
    td->conditionVar->Wait(td->glock);
  td->threads[0]= true;
  td->glock->Unlock();
  return NULL;
}


void* TestThreadWaiting2(void* ptr)
{
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  ThreadData* td = static_cast<ThreadData*>(info->UserData);
  td->glock->Lock();
  while(!td->condition)
    td->conditionVar->Wait(td->glock);
  td->threads[1]= true;
  td->glock->Unlock();
  return NULL;
}

  
void* TestThreadFunction1(void* ptr)
{
  //------------------------------------------------------------
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  ThreadData* td = static_cast<ThreadData*>(info->UserData);
  //igtl::SimpleMutexLock *glock = td->glocks[0];
  int i = 0;
  while(++i <=10)
  {
    std::cerr<<i<<" in the first child thread"<<std::endl;
    igtl::Sleep(200);
    td->iFinalCount++;
  };
  td->condition = true;
  if (td->isBroadCast)
  {
    td->conditionVar->Broadcast();
  }
  else
  {
    td->conditionVar->Signal();
  }
  return NULL;
}

void* TestThreadFunction2(void* ptr)
{
  //------------------------------------------------------------
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  ThreadData* td = static_cast<ThreadData*>(info->UserData);
  int i = 0;
  while(++i <=10)
  {
    std::cerr<<i<<" in the first child thread"<<std::endl;
    igtl::Sleep(200);
    td->iFinalCount++;
  };
  td->condition = true;
  if (td->isBroadCast)
  {
    td->conditionVar->Broadcast();
  }
  else
  {
    td->conditionVar->Signal();
  }
  return NULL;
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

