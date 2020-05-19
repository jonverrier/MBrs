/////////////////////////////////////////
// CoreQueue.h
// Copyright (c) 2020 TXPCo Ltd
/////////////////////////////////////////

#ifndef COREQUEUE_INCLUDED
#define COREQUEUE_INCLUDED

///////////////////////////////////////////////////////////////////////////////
// CoreConcurrentQueue
///////////////////////////////////////////////////////////////////////////////

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template <typename AType>
class CoreConcurrentQueue
{
public:

   AType pop()
   {
      std::unique_lock<std::mutex> mlock(m_mutex);
      while (m_queue.empty())
      {
         m_condition.wait(mlock);
      }
      auto item = m_queue.front();
      m_queue.pop();
      return item;
   }

   void pop(AType& item)
   {
      std::unique_lock<std::mutex> mlock(m_mutex);
      while (m_queue.empty())
      {
         m_condition.wait(mlock);
      }
      item = m_queue.front();
      m_queue.pop();
   }

   void push(const AType& item)
   {
      std::unique_lock<std::mutex> mlock(m_mutex);
      m_queue.push(item);
      mlock.unlock();
      m_condition.notify_one();
   }

   void push(AType&& item)
   {
      std::unique_lock<std::mutex> mlock(m_mutex);
      m_queue.push(std::move(item));
      mlock.unlock();
      m_condition.notify_one();
   }

   size_t size ()
   {
      size_t mySize = 0;

      std::unique_lock<std::mutex> mlock(m_mutex);
      mySize = m_queue.size();
      mlock.unlock();

      return mySize;
   }

private:
   std::queue<AType> m_queue;
   std::mutex m_mutex;
   std::condition_variable m_condition;
};

#endif // COREQUEUE_INCLUDED
