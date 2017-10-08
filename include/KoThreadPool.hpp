/****************************************************************************
 Copyright (c) 2017 ko jung hyun
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef _KOTHREADPOOL_HPP_
#define _KOTHREADPOOL_HPP_

#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <functional>
#include <vector>
#include <queue>

#include "CondVar.hpp"

///////////////////////////////////////////////////////////////////////////////
template <typename T>
class TaskQueue 
{
    public:
        void PushQueue(T const & value) 
        {
            std::unique_lock<std::mutex> lock(mutex_);
            task_queue_.push(value);
        }

        bool  PopQueue() 
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (task_queue_.empty())
            {
                return false;
            }

            std::function<void()> func = task_queue_.front();
            task_queue_.pop();

            func();

            return true;
        }

        bool IsEmpty() 
        {
            std::unique_lock<std::mutex> lock(mutex_);
            return task_queue_.empty();
        }

    private:
        std::queue<T> task_queue_ ;
        std::mutex    mutex_      ;
};


///////////////////////////////////////////////////////////////////////////////
class KoThreadPool 
{
    public:
        KoThreadPool()  = default;
        ~KoThreadPool() = default;

        bool InitThreadPool(int num_of_threads= 0) 
        {
            if(num_of_threads == 0 )
            {
                int concurrency_hint = std::thread::hardware_concurrency();
                std::cout <<"std::thread::hardware_concurrency() = " << concurrency_hint << "\n";

                if(concurrency_hint > 1 )
                {
                    num_of_threads_  = concurrency_hint ;
                }
                else
                {
                    std::cerr << "std::thread::hardware_concurrency() failed! specify number of thread \n";
                    return false;
                }
            }
            else
            {
                num_of_threads_  = num_of_threads ;
            }

            vec_thread_terminated_.reserve( num_of_threads_ );

            for(int i=0; i < num_of_threads_ ; i++)
            {
                vec_thread_.push_back( std::thread (&KoThreadPool::WorkerThreadRoutine, this, i) ) ;
                vec_thread_terminated_.push_back(false);
            }

            return true;
        }

        void AssignTask( std::function<void()> & func ) 
        {
            task_queue_.PushQueue(func);
            cond_var_.NotifyOne();
        }

        void Terminate()
        {
            stop_flag_ = true;
            cond_var_.NotifyAll();

            int terminated_count = 0 ;
            while( true ) 
            {
                bool is_terminated = vec_thread_terminated_[terminated_count] ;

                if( !is_terminated )
                {
                    cond_var_.NotifyAll();
                    std::this_thread::sleep_for(std::chrono::milliseconds(5)); 
                    continue;
                }
                else
                {
                    terminated_count ++;
                    if(terminated_count == num_of_threads_)
                    {
                        break;
                    }
                }
            }

            for(size_t i = 0; i < vec_thread_.size(); i++)
            {
                if (vec_thread_[i].joinable())
                {
                    vec_thread_[i].join();
                }
            }
        }


    private:

        TaskQueue<std::function<void()>  > task_queue_ ;
        std::atomic<bool>   stop_flag_ {false};
        std::vector<std::thread> vec_thread_ ;
        std::vector<bool> vec_thread_terminated_ ;
        CondVar     cond_var_ ;
        int         num_of_threads_ {-1};

    private:
        void WorkerThreadRoutine(int index)
        {
            while (true) 
            {
                if(task_queue_.IsEmpty())
                {
                    cond_var_.WaitForSignal();
                }

                task_queue_.PopQueue() ;

                if(stop_flag_)
                {
                    break;
                }
            } 

            vec_thread_terminated_[index] = true; 
        }
        
        KoThreadPool(const KoThreadPool &) = delete;
        KoThreadPool(KoThreadPool &&) = delete;
        KoThreadPool & operator=(const KoThreadPool &) = delete;
        KoThreadPool & operator=(KoThreadPool &&) = delete;
};

#endif //_KOJHTHREADPOOL_HPP_

