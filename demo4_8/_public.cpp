#include "_public.h"

csemp::csemp()
{
	m_semid=-1;
	m_sem_flg=SEM_UNDO;
}

csemp::~csemp()
{
}

// 如果信号量已经存在 获取信号量；如果信号量不存在则创建并初始化为value
bool csemp::init(key_t key, unsigned short value, short sem_flg) 
{
	m_sem_flg = sem_flg;

	// 信号量的初始化分为三个步骤
	// step 1 获取信号量，如果成功 函数返回
	// step 2 如果失败 则创建信号量
	// step 3 设置信号量的初始值

	// 获取信号量
	m_semid = semget(key, 1, 0666);
	if (m_semid == -1) {
		// 如果信号量不存在 则创建
		if (errno == ENOENT) {
			// 使用 IPC_EXCL 标志确保只有一个进程创建并初始化信号量 其他进程只能获取
			m_semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
			if (m_semid == -1) {
				if (errno == EEXIST) {
					m_semid = semget(key, 1, 0666);
					if (m_semid == -1) {
						perror("init 1 semget()");
						return false;
					}
					return true;
				} else {
					perror("init 2 semget()");
					return false;
				}
			}
			// 信号量创建成功后 需要将其初始化为value
			union semun {
				int val;
				struct semid_ds *buf;
				unsigned short *array;
				struct seminfo *__buf;
			} sem_union;
			sem_union.val = value; // 设置信号量初始值
			if (semctl(m_semid, 0, SETVAL, sem_union) < 0) {
				perror("init semctl()");
				return false;
			}
		} else {
			perror("init 3 semget()");
			return false;
		}
	}
	return true;
}

// 信号量的P操作（将信号量的值加上value） 如果信号量的值是0 将阻塞等待 直到信号量的值大于0
bool csemp::wait(short value) {
	if(m_semid==-1) {
		perror("wait ererer\n");
		return false;
	}

	struct sembuf sem_b;
	sem_b.sem_num = 0;		// 信号量编号 0代表第一个信号量
	sem_b.sem_op = value;	// P操作的value必须小于0
	sem_b.sem_flg = m_sem_flg;

	if(semop(m_semid, &sem_b, 1)==-1) {
		perror("p semop()");
		return false;
	}
	return true;
}

// 信号量的V操作（将信号量的值加value）
bool csemp::post(short value)
{
	if(m_semid==-1) {
		perror("post ererer\n");
		return false;
	}

	struct sembuf sem_b;
	sem_b.sem_num = 0;		// 信号量的编号 0代表第一个信号量
	sem_b.sem_op = value;	// V操作的value必须大于0
	sem_b.sem_flg = m_sem_flg;
	if(semop(m_semid, &sem_b, 1)) {
		perror("v semop()");
		return false;
	}
	return true;
}

// 获取信号量的值 成功返回信号量的值 失败返回-1
int csemp::getvalue() {
	return semctl(m_semid, 0, GETVAL);
}

// 销毁信号量
bool csemp::destroy() {
	if(m_semid==-1) {
		return false;
	}
	if(semctl(m_semid, 0, IPC_RMID) == -1) {
		perror("destroy semctl()");
		return false;
	}
	return true;
}