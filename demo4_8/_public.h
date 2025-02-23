#ifndef __PUBLIC_HH
#define __PUBLIC_HH

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>

using namespace std;

// 循环队列
template<class TT, int MaxLength>
class squeue
{
private:
	bool 	m_inited;		// 队列初始化标志
	TT 	m_data[MaxLength];	// 使用数组存储循环队列中的元素
	int	m_head;
	int	m_tail;
	int	m_length;
	squeue(const squeue &) = delete;		// 禁止拷贝构造函数
	squeue &operator=(const squeue &) = delete;	// 禁止赋值函数

public:
	squeue() { init(); }	// 构造函数
	
	// 循环队列的初始化操作
	// 注意：如果用于共享内存的队列 不会调用构造函数 则必须调用此函数初始化 ???
	void init()
	{
		cout << "init\n";
		if(m_inited!=true)
		{
			m_head=0;
			m_tail=MaxLength-1;
			m_length=0;
			memset(m_data, 0, sizeof(m_data));
			m_inited=true;
		}
	}

	// 元素入队
	bool push(const TT &ee)
	{
		if(full() == true)
		{
			cout << "循环队列已满，入队失败\n"; return false;
		}
		// 先移动队伍尾指针 再拷贝数据
		m_tail=(m_tail+1)%MaxLength;
		m_data[m_tail]=ee;
		m_length++;
		return true;
	}

	// 求循环队列的长度
	int size()
	{
		return m_length;
	}

	// 判断循环队列是否为空
	bool empty()
	{
		if(m_length == 0) return true;
		return false;
	}

	// 判断循环队列是否已满
	bool full()
	{
		if(m_length == MaxLength) return true;
		return false;
	}

	// 查看队头元素值，元素不出队
	TT& front()
	{
		return m_data[m_head];
	}

	// 元素出队
	bool pop()
	{
		if(empty() == true) return false;
		m_head=(m_head+1)%MaxLength;
		m_length--;
		return true;
	}

	// 显示村换队列的全部元素
	void printqueue()
	{
		for(int ii=0; ii<size(); ii++)
		{
			cout << "m_data ii=" << ii << " [" << (m_head+ii)%MaxLength << "], value=" \
				<< m_data[(m_head+ii)%MaxLength] << endl;
		}
	}
};

// 信号量
class csemp
{
private:
	union semun		// 用于信号量操作的共同体
	{
		int val;
		struct semid_ds* buf;
		unsigned short* arry;
	};

	int m_semid;	// 信号量id（描述符）

	// 如果将 sem_fllg 设置为SEM_UNDO，操作系统将跟踪进程对信号量的修改情况
	// 在全部修改过信号量的进程终止后，操作系统将把信号量恢复为初始值
	// 如果信号量用于互斥锁 设置为SEM_UNDO
	// 如果信号量用于生产者消费者模型 设置为0
	short m_sem_flg;

	csemp(const csemp&) = delete;
	csemp& operator=(const csemp&) = delete;

public:
	csemp();

	// 如果信号量已经存在 获取信号量的值；如果信号量不存在 创建并初始化为value
	// 如果用于互斥锁 value填1 sem_flg填SEM_UNDO
	// 如果用于生产者消费者模型 value填0 sem_flg填0 
	bool init(key_t key, unsigned short value=1, short sem_flg=SEM_UNDO);
	bool wait(short sem_op=-1);		// 信号量的P操作
	bool post(short sem_op=1);		// 信号量的V操作
	int getvalue();					// 获取信号量的值 成功返回信号量的值 失败返回-1
	bool destroy();					// 销毁信号量
	~csemp();
};

#endif