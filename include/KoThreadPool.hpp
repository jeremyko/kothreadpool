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
#include <condition_variable>
#include <chrono>

typedef enum __ENUM_COND_VAR_RSLT__
{
    COND_VAR_RSLT_TIMEOUT = 0,
    COND_VAR_RSLT_SIGNALED
} ENUM_COND_VAR_RSLT ;

const size_t IMPOSSIBLE_HUGE_CNT = 9999999999 ;

///////////////////////////////////////////////////////////////////////////////
class CondVar
{
    public:
        CondVar()  = default;
        ~CondVar() = default;

        void NotifyOne() {
            std::unique_lock<std::mutex> lock (cond_var_lock_);
            notified_cnt_++ ;
            cond_var_.notify_one();
        }

        void NotifyAll() {
            std::unique_lock<std::mutex> lock (cond_var_lock_);
            notified_cnt_++ ;
            cond_var_.notify_all();
        }

        void WaitForSignal() {
            std::unique_lock<std::mutex> lock (cond_var_lock_);
            while (!notified_cnt_) {
                cond_var_.wait(lock );
            }    
            if(!is_all_waiting_end_) {
                notified_cnt_-- ;
            }
        }

        ENUM_COND_VAR_RSLT WaitForSignalTimeout(int timeout_secs) {
            std::unique_lock<std::mutex> lock (cond_var_lock_);
            std::cv_status ret = std::cv_status::no_timeout;
            auto duration_sec = std::chrono::seconds(timeout_secs);

            while(!notified_cnt_ && std::cv_status::timeout !=ret) { 
                ret=cond_var_.wait_for(lock, duration_sec);
            }
            if(!is_all_waiting_end_) {
                notified_cnt_-- ;
            }
            if(std::cv_status::timeout ==ret) {
                return COND_VAR_RSLT_TIMEOUT;
            }
            return COND_VAR_RSLT_SIGNALED;
        }

        void SetAllWaitingEnd() {
            is_all_waiting_end_ = true;
        }

    private:    
        std::mutex              cond_var_lock_ ;
        std::condition_variable cond_var_ ;
        size_t notified_cnt_ {0}; 
        //XXX use count. no boolean. 
        //NotifyOne, NotifyAll can be called multiple times before WaitForSignal is called.
        bool is_all_waiting_end_ {false};

};

///////////////////////////////////////////////////////////////////////////////
class KoThreadPool 
{
    public:
        KoThreadPool()  = default;

        ~KoThreadPool() {
            if(!stop_flag_) {
                Terminate();
            }
        }
        // Find the optimal thread count through testing.    
        bool InitThreadPool(int num_of_threads= 0) {
            if(num_of_threads == 0 ) {
                int concurrency_hint = std::thread::hardware_concurrency();
                std::cout <<"std::thread::hardware_concurrency() = " 
                          << concurrency_hint << "\n";

                if(concurrency_hint > 1 ) {
                    num_of_threads_  = concurrency_hint ;
                } else {
                    std::cerr << "std::thread::hardware_concurrency() failed! "
                              << "specify number of thread \n";
                    return false;
                }
            } else {
                num_of_threads_  = num_of_threads ;
            }
            for(int i=0; i < num_of_threads_ ; i++) {
                vec_thread_.push_back( std::thread (&KoThreadPool::WorkerThreadRoutine, this, i) ) ;
            }
            return true;
        }

        void AssignTask( std::function<void()> & func ) {
            PushQueue(func);
            cond_var_.NotifyOne();
        }

        void Terminate(bool terminate_immediately=false) {
            stop_flag_ = true;
            is_terminate_immediately_ = terminate_immediately;

            if(is_terminate_immediately_) {
                cond_var_.SetAllWaitingEnd(); //terminate immediately
            }
            cond_var_.NotifyAll();
            for(size_t i = 0; i < vec_thread_.size(); i++) {
                if (vec_thread_[i].joinable()) {
                    vec_thread_[i].join();
                }
            }
        }
        void SetWaitingCnt(size_t count) {  
            waiting_cnt_ = count ; 
        }
        void WaitAllWorkDone() {  //blocking call.
            while(true){
                if(done_cnt_ == waiting_cnt_ ){
                    done_cnt_    = 0 ; 
                    waiting_cnt_ = IMPOSSIBLE_HUGE_CNT ; 
                    break;
                }
                cond_var_wait_done_.WaitForSignal();
            }
        }
    public:
        std::atomic<size_t> waiting_cnt_ {IMPOSSIBLE_HUGE_CNT};
        std::atomic<size_t> done_cnt_    {0};
    private:
        std::queue<std::function<void()>  > task_queue_ ;
        std::mutex    mutex_      ;
        std::atomic<bool>   stop_flag_ {false};
        std::atomic<bool>   is_terminate_immediately_ {false};
        std::vector<std::thread> vec_thread_ ;
        CondVar     cond_var_ ;
        CondVar     cond_var_wait_done_ ;
        int         num_of_threads_ {-1};

    private:

        void PushQueue( std::function<void()> const & value) {
            std::unique_lock<std::mutex> lock(mutex_);
            task_queue_.push(value);
        }

        bool  PopQueue() {
            std::function<void()> func ;
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (task_queue_.empty()) {
                    return false;
                }
                func = task_queue_.front();
                task_queue_.pop();
            }
            func();
            done_cnt_ ++;
            cond_var_wait_done_.NotifyOne();
            return true;
        }

        bool IsQueueEmpty() {
            std::unique_lock<std::mutex> lock(mutex_);
            return task_queue_.empty();
        }

        void WorkerThreadRoutine(int index) {
            while (true) {
                if(IsQueueEmpty()) {
                    if(stop_flag_) {
                        //graceful terminate
                        cond_var_.NotifyAll();
                        return; 
                    }
                    cond_var_.WaitForSignal();
                }
                if(is_terminate_immediately_ ) {
                    //force terminate
                    return;
                }
                PopQueue() ;
            } 
        }
        
        KoThreadPool(const KoThreadPool &) = delete;
        KoThreadPool(KoThreadPool &&) = delete;
        KoThreadPool & operator=(const KoThreadPool &) = delete;
        KoThreadPool & operator=(KoThreadPool &&) = delete;
};

#endif //_KOJHTHREADPOOL_HPP_

