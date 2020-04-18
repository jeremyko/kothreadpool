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

std::atomic<int> gSum1;
std::atomic<int> gSum2;
std::atomic<int> gSum3;

///////////////////////////////////////////////////////////////////////////////
void MyThreadWork()
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
        void MyThreadWork1() {
            gSum2++;
        }
        void MyThreadWork2(int a, int b, int c) {
            gSum3 += (a+b+c);
        }
};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    KoThreadPool tpool;

    if( ! tpool.InitThreadPool() ) {
        std::cerr << "Error : Init" << "\n";
        exit(1);
    }
    gSum1 = 0;
    gSum2 = 0;
    gSum3 = 0;
    MyClass myclass;
    int i = 0;
    while(true) {
        if(i >= 1000) {
            break;
        }
        //class member
        std::function<void()> temp_func1 = std::bind( &MyClass::MyThreadWork1, &myclass)  ;
        tpool.AssignTask(temp_func1 )  ;

        std::function<void()> temp_func2 = std::bind( &MyClass::MyThreadWork2, &myclass,1,1,1)  ;
        tpool.AssignTask(temp_func2 )  ;
        
        //free function
        std::function<void()> temp_func3 = std::bind( &MyThreadWork )  ;
        tpool.AssignTask(temp_func3 )  ;
        i++;
    }
    tpool.Terminate(); //graceful terminate : wait until all work done
    //tpool.Terminate(true); //true -->> terminate immediately

    std::cout << "gSum1 =" << gSum1 << "\n";
    std::cout << "gSum2 =" << gSum2 << "\n";
    std::cout << "gSum3 =" << gSum3 << "\n";
    return 0;
}

