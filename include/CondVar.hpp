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

#ifndef _CONDVAR_HPP_
#define _CONDVAR_HPP_

#include <condition_variable>
#include <mutex>
#include <chrono>

typedef enum __ENUM_COND_VAR_RSLT__
{
    COND_CAR_RSLT_TIMEOUT = 0,
    COND_CAR_RSLT_SIGNALED
} ENUM_COND_VAR_RSLT ;

///////////////////////////////////////////////////////////////////////////////
class CondVar
{
    public:
        CondVar()  = default;
        ~CondVar() = default;

        void NotifyOne()
        {
            std::unique_lock<std::mutex> lock (cond_var_lock_);
            is_notified_ = true;
            cond_var_.notify_one();
        }

        void NotifyAll()
        {
            std::unique_lock<std::mutex> lock (cond_var_lock_);
            is_notified_ = true;
            cond_var_.notify_all();
        }

        void WaitForSignal()
        {
            std::unique_lock<std::mutex> lock (cond_var_lock_);
            while (!is_notified_) 
            {
                cond_var_.wait(lock );
            }    
            is_notified_=false; 
        }

        ENUM_COND_VAR_RSLT WaitForSignalTimeout(int timeout_secs)
        {
            std::unique_lock<std::mutex> lock (cond_var_lock_);
            std::cv_status ret = std::cv_status::no_timeout;

            auto duration_sec = std::chrono::seconds(timeout_secs);

            while(!is_notified_ && std::cv_status::timeout !=ret)
            { 
                ret=cond_var_.wait_for(lock, duration_sec);
            }

            is_notified_=false;
            if(std::cv_status::timeout ==ret)
            {
                return COND_CAR_RSLT_TIMEOUT;
            }
            return COND_CAR_RSLT_SIGNALED;
        }


    private:    
        std::mutex              cond_var_lock_ ;
        std::condition_variable cond_var_ ;
        bool is_notified_ {false};

};

#endif //_CONDVAR_HPP_



