#ifndef JOBTHREADSJOINER_H
#define JOBTHREADSJOINER_H

#include <vector>
#include <thread>

class JobThreadsJoiner
{
public:
    explicit JobThreadsJoiner(std::vector<std::thread>& threads_):
        threads(threads_){}
    ~JobThreadsJoiner()
    {
        for (unsigned long i = 0; i < threads.size(); ++i)
        {
            if (threads[i].joinable())
                threads[i].join();
        }
    }

private:
    std::vector<std::thread>& threads;
};
#endif
