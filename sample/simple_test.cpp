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

#include <iostream>
#include <string>
#include <atomic>
#include "KoThreadPool.hpp"

std::atomic<int> g_sum ;

///////////////////////////////////////////////////////////////////////////////
class MyClass
{
    public:
        MyClass()  {} ;
        ~MyClass() {} ;
        void SumWork( int a) {
            g_sum += a;
        }
};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    g_sum = 0;
    MyClass      myclass ;
    KoThreadPool tpool   ;

    if( ! tpool.InitThreadPool() ) {
        std::cerr << "Error : Init" << "\n";
        exit(1);
    }

    tpool.SetWaitingCnt(10); //set total work count    

    for(int i = 0; i < 10; i++ ){
        std::function<void()> temp_func = std::bind( &MyClass::SumWork, &myclass,1) ;
        tpool.AssignTask(temp_func)  ;
    }
    //wait all works done.
    tpool.WaitAllWorkDone(); // --> blocking call. 
    if(g_sum != 10){ //to vefify
        std::cout << "error  : " << g_sum << "\n";
        exit(1);
    }

    std::cout << "all work done \n" ;
    //time to program exit, terminate thread pool.
    tpool.Terminate(); //graceful terminate : wait until all work done
    //tpool.Terminate(true); //true -->> terminate immediately
    return 0;
}


