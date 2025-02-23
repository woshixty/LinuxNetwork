// 演示使用信号量给共享内存加锁
#include "_public.h"

struct stgirl
{
    int no;
    char name[51];
};

int main(int argc, char* argv[]) {
    if(argc != 3) {
        cout << "Usage: ./test no name\n";
        return -1;
    }

    int shmid = shmget(0x5005, sizeof(stgirl), 0640|IPC_CREAT);
    if(shmid == -1) {
        cout << "shmget(0x5005) failed \n";
        return -1;
    }
    cout << "shmid=" << shmid << endl;

    stgirl* ptr = (stgirl*)shmat(shmid, 0, 0);
    if(ptr == (void*)-1) {
        cout << "shmat() failed\n";
        return -1;
    }

    // 创建、初始化二元信号量
    csemp mutex;
    if(mutex.init(0x5005)==false) {
        cout << "mutex.init() failed\n";
        return -1;
    }

    cout << "申请加锁...\n";
    mutex.wait();
    cout << "申请加锁成功\n";

    // 使用共享内存进行加锁
    cout << "原值: no=" << ptr->no << ", name=" << ptr->name << endl;
    ptr->no = atoi(argv[1]);
    strcpy(ptr->name, argv[2]);
    cout << "新值: no=" << ptr->no << ", name=" << ptr->name << endl;
    sleep(10);

    shmdt(ptr);

    return 0;
}
