/*=========================================================================
 
 Program:   OpenIGTLink Library
 Language:  C++
 Date:      $Date: 2016/02/12 19:53:38 $
 
 Copyright (c) Insight Software Consortium. All rights reserved.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notices for more information.
 
 =========================================================================*/

#include "../Source/igtlConditionVariable.h"
#include "igtl_header.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "igtlMultiThreader.h"
#include "igtlOSUtil.h"

void* TestThreadFunction1(void* ptr);
void* TestThreadFunction2(void* ptr);
void* TestThreadFunctionBroadcast(void* ptr);
int icount1 = 0, icount2 = 0, icountBroadcast = 0;

typedef struct {
  igtl::SimpleMutexLock *glock;
  igtl::ConditionVariable::Pointer conditionVar;
} ThreadData;

TEST(ConditionVariableTest, SingleChildThreadTest)
{
  // From this test we can see that, the wait function really works,
  // the main thread waits until the spawned thread is finished, otherwize the icount1 will be 0.
  igtl::ConditionVariable::Pointer conditionVar = igtl::ConditionVariable::New();
  igtl::SimpleMutexLock *localMutex = new igtl::SimpleMutexLock();
  ThreadData td;
  td.conditionVar = conditionVar;
  td.glock = localMutex;
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadFunction1, &td);
  conditionVar->Wait(localMutex);
  std::cerr<<"The child thread is released";
  EXPECT_EQ(icount1, 9);
}

TEST(ConditionVariableTest, MultiChildThreadTest)
{
  igtl::ConditionVariable::Pointer conditionVar1 = igtl::ConditionVariable::New();
  igtl::SimpleMutexLock *localMutex1 = new igtl::SimpleMutexLock();
  ThreadData td1;
  td1.conditionVar = conditionVar1;
  td1.glock = localMutex1;
  
  igtl::ConditionVariable::Pointer conditionVar2 = igtl::ConditionVariable::New();
  igtl::SimpleMutexLock *localMutex2 = new igtl::SimpleMutexLock();
  ThreadData td2;
  td2.conditionVar = conditionVar2;
  td2.glock = localMutex2;
  
  igtl::MultiThreader::Pointer threader = igtl::MultiThreader::New();
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadFunction1, &td1);
  igtl::Sleep(1000);
  threader->SpawnThread((igtl::ThreadFunctionType) &TestThreadFunction2, &td2);
  conditionVar1->Wait(localMutex1);
  conditionVar2->Wait(localMutex2);
  EXPECT_EQ(icount1, 9);
  EXPECT_EQ(icount2, 9);
  std::cerr<<"The child threads are released";
}

void* TestThreadFunction1(void* ptr)
{
  //------------------------------------------------------------
  // Get thread information
  igtl::MultiThreader::ThreadInfo* info =
  static_cast<igtl::MultiThreader::ThreadInfo*>(ptr);
  ThreadData* td = static_cast<ThreadData*>(info->UserData);
  int i = 0;
  icount1 = 0;
  while(++i<10)
  {
    std::cerr<<i<<" in the first child thread"<<std::endl;
    igtl::Sleep(200);
    icount1++;
  };
  td->conditionVar->Signal();
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
  icount2 = 0;
  while(++i<10)
  {
    std::cerr<<i<<" in the second child thread"<<std::endl;
     icount2++;
    igtl::Sleep(200);
  };
  td->conditionVar->Signal();
  return NULL;
}


int main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

