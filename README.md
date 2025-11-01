
# KoThreadPool #

### What ###

simple thread pool library for c++  

### Usage ###

see sample directory

```cpp

MyClass myclass; // Assume that MyClass has the following member functions:
                 // void MyThreadWork1(), void MyThreadWork2(int,int,int)
void MyThreadWork() ; // Let's assume there is a free function 

// ------------ Then you can use this library like this ------------
KoThreadPool tpool;
tpool.InitThreadPool(); 
//tpool.InitThreadPool(4); //explicit thread worker count 
tpool.SetWaitingCnt(3); //set total work count    

//assign your work.
std::function<void()> temp_func1 = std::bind( &MyClass::MyThreadWork1, &myclass)  ;
tpool.AssignTask(temp_func1 )  ;
std::function<void()> temp_func2 = std::bind( &MyClass::MyThreadWork2, &myclass,1,1,1)  ;
tpool.AssignTask(temp_func2 )  ;
std::function<void()> temp_func3 = std::bind( &MyThreadWork )  ;
tpool.AssignTask(temp_func3 )  ;

//wait all 3 works done.
//or you can call "Terminate(true)" here to exit program. 
tpool.WaitAllWorkDone(); // --> blocking call. 

//At the end of the program, exit the thread pool.
tpool.Terminate(); //graceful terminate : wait until all work done
//tpool.Terminate(true); // terminate immediately

```
