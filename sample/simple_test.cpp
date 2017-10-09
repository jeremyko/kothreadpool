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
#include "KoThreadPool.hpp"

///////////////////////////////////////////////////////////////////////////////
bool MyThreadWork(int val, std::string& str)
{
    //std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
    std::cout << "user value=" << val << "/" << str << "\n";
    return true;
}

///////////////////////////////////////////////////////////////////////////////
class MyClass
{
    public:
        MyClass()  {} ;
        ~MyClass() {} ;

        bool MyThreadWork(int context_val)
        {
            context_val_ = context_val ;
            //std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
            std::cout <<"thread work : context =" << context_val_<< "\n";

            return true;
        }

    private:
        int context_val_ ;
};

///////////////////////////////////////////////////////////////////////////////
int main()
{
    KoThreadPool tpool;

    if( ! tpool.InitThreadPool() ) 
    {
        std::cerr << "Error : Init" << "\n";
        return -1;
    }

    MyClass myclass;

    int i = 0;
    while(true)
    {
        if(i >= 10)
        {
            break;
        }

        //class member
        std::function<void()> temp_func1 = std::bind( &MyClass::MyThreadWork, &myclass, i)  ;
        tpool.AssignTask(temp_func1 )  ;

        //free function
        char temp_buffer[20];
        snprintf(temp_buffer, sizeof(temp_buffer), "str %d ", i );
        std::function<void()> temp_func2 = std::bind( &MyThreadWork, i, std::string( temp_buffer ) )  ;
        tpool.AssignTask(temp_func2 )  ;

        i++;

        std::this_thread::sleep_for(std::chrono::milliseconds(3));
    }
    
    tpool.Terminate();

    std::cout << "main exit...\n";

    return 0;

}

