#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t threadnum,const std::string& threadtype):stop_(false),threadtype_(threadtype)
{
    // 启动threadnum个线程，每个线程将阻塞在条件变量上。
	for (size_t ii = 0; ii < threadnum; ii++)
    {
        // 用lambda函创建线程。
		threads_.emplace_back([this]
        {
            printf("create %s thread(%ld).\n",threadtype_.c_str(),syscall(SYS_gettid));     // 显示线程类型和线程ID。

			while (stop_==false)
			{
				std::function<void()> task;       // 用于存放出队的元素。

				{   // 锁作用域的开始。 ///////////////////////////////////
					std::unique_lock<std::mutex> lock(this->mutex_);

					// 等待生产者的条件变量。
					this->condition_.wait(lock, [this] 
                    { 
                        return ((this->stop_==true) || (this->taskqueue_.empty()==false));
                    });

                    // 在线程池停止之前，如果队列中还有任务，执行完再退出。
					if ((this->stop_==true)&&(this->taskqueue_.empty()==true)) return;

                    // 出队一个任务。
					task = std::move(this->taskqueue_.front());
					this->taskqueue_.pop();
				}   // 锁作用域的结束。 ///////////////////////////////////

                printf("%s(%ld) execute task.\n",threadtype_.c_str(),syscall(SYS_gettid));
				task();  // 执行任务。
			}
		});
    }
}

void ThreadPool::addtask(std::function<void()> task)
{
    {   // 锁作用域的开始。 ///////////////////////////////////
        std::lock_guard<std::mutex> lock(mutex_);
        taskqueue_.push(task);
    }   // 锁作用域的结束。 ///////////////////////////////////

    condition_.notify_one();   // 唤醒一个线程。
}

ThreadPool::~ThreadPool()
{
	stop_ = true;

	condition_.notify_all();  // 唤醒全部的线程。

    // 等待全部线程执行完任务后退出。
	for (std::thread &th : threads_) 
        th.join();
}

// 获取线程池的大小。
size_t ThreadPool::size()
{
    return threads_.size();
}

//////////////////////////////////////////////////////////////////
/*
void show(int no, const std::string &name)
{
    printf("小哥哥们好，我是第%d号超级女生%s。\n",no,name.c_str());
}

void test()
{
    printf("我有一只小小鸟。\n");
}

int main()
{
    ThreadPool threadpool(3);
    
    std::string name="西施";
    threadpool.addtask(std::bind(show,8,name));
    sleep(1);

    threadpool.addtask(std::bind(test));
    sleep(1);

    threadpool.addtask(std::bind([]{ printf("我是一只傻傻鸟。\n");}));
    sleep(1);
}
*/
//////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
/*
class AA
{
public:
    void show()
    {
        printf("我是一只傻傻鸟。\n");
    }

    ~AA() 
    {
        printf("调用了析构函数。\n");
    }
};

#include <memory>

void fun(std::shared_ptr<AA> aa)
{
    sleep(5);
    aa->show();
}

int main()
{
    ThreadPool tp(2,"TEST");

    {
        std::shared_ptr<AA> aa(new AA);
        tp.addtask(std::bind(fun,aa));
    }

    sleep(10);   // 让线程有时间可以运行。

    return 0;
}
*/
//////////////////////////////////////////////////////////////////


// g++ -o test ThreadPool.cpp -lpthread