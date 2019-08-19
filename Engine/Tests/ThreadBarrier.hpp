#include <mutex>
#include <condition_variable>
#include <Core/Misc/Defines/Common.hpp>

TRE_NS_START

class ThreadBarrier
{

 public:
    ThreadBarrier(int count)
     : counter(0), waiting(0), thread_count(count)
    {}

    void wait()
    {
        //fence mechanism
        std::unique_lock<std::mutex> lk(m);
        ++counter;
        ++waiting;
        cv.wait(lk, [&]{return counter >= thread_count;});
        cv.notify_one();
        --waiting;
        if(waiting == 0)
        {
           //reset barrier
           counter = 0;
        }
        lk.unlock();
    }

 private:
    std::mutex m;
    std::condition_variable cv;
    uint32 counter;
    uint32 waiting;
    uint32 thread_count;
};

TRE_NS_END