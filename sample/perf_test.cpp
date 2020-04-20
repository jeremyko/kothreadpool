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

std::atomic<int> gSum1 ;
std::atomic<int> gSum2 ;
std::atomic<int> gSum3 ;

///////////////////////////////////////////////////////////////////////////////
void SumWork1()
{
    gSum1++;
    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

///////////////////////////////////////////////////////////////////////////////
class MyClass
{
    public:
        MyClass()  {} ;
        ~MyClass() {} ;
        void SumWork2() {
            gSum2++;
        }
        void SumWork3(int a) {
            gSum3 += a;
        }
};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    gSum1 = 0;
    gSum2 = 0;
    gSum3 = 0;
    MyClass      myclass ;
    KoThreadPool tpool   ;

    if( ! tpool.InitThreadPool(4) ) { //Find the optimal thread count through testing.
        std::cerr << "Error : Init" << "\n";
        exit(1);
    }
    tpool.SetWaitingCnt(10*10000);
    for(int i = 0; i < 10000; i++ ){
        //to make gSum1 = 4
        std::function<void()> temp_func1 = std::bind( &SumWork1 ) ;
        tpool.AssignTask(temp_func1 )  ;
        std::function<void()> temp_func2 = std::bind( &SumWork1 ) ;
        tpool.AssignTask(temp_func2 )  ;
        std::function<void()> temp_func3 = std::bind( &SumWork1 ) ;
        tpool.AssignTask(temp_func3 )  ;
        std::function<void()> temp_func4 = std::bind( &SumWork1 ) ;
        tpool.AssignTask(temp_func4); 
        //to make gSum2 = 3
        std::function<void()> temp_func5 = std::bind( &MyClass::SumWork2, &myclass) ;
        tpool.AssignTask(temp_func5 )  ;
        std::function<void()> temp_func6 = std::bind( &MyClass::SumWork2, &myclass) ;
        tpool.AssignTask(temp_func6 )  ;
        std::function<void()> temp_func7 = std::bind( &MyClass::SumWork2, &myclass) ;
        tpool.AssignTask(temp_func7 )  ;
        //to make gSum3 = 3
        std::function<void()> temp_func8 = std::bind( &MyClass::SumWork3, &myclass,1) ;
        tpool.AssignTask(temp_func8 )  ;
        std::function<void()> temp_func9 = std::bind( &MyClass::SumWork3, &myclass,1) ;
        tpool.AssignTask(temp_func9 )  ;
        std::function<void()> temp_func10 = std::bind( &MyClass::SumWork3, &myclass,1) ;
        tpool.AssignTask(temp_func10 )  ;

    }
    //wait all works done.
    tpool.WaitAllWorkDone(); // --> blocking call. 
    std::cout << "all work done \n" ;
    std::cout << "sum  : " << gSum1 << "," << gSum2 << "," <<gSum3 << "\n";
    //time to program exit, terminate thread pool.
    tpool.Terminate(); //graceful terminate : wait until all work done
    //tpool.Terminate(true); //true -->> terminate immediately
    return 0;
}


