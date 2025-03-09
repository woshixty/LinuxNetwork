// 演示Linux底层文件的操作-写入文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;     // 定义一个文件描述符/文件句柄

    fd=open("data.txt", O_CREAT | O_RDWR | O_TRUNC);    // 打开文件
    if(fd==-1)
    {
        perror("open(data.txt)");
        return -1;
    }
    printf("文件描述符fd=%d\n", fd);
    
    char buffer[1024];
    strcpy(buffer, "我是一只傻傻鸟\n");
    if(write(fd, buffer, strlen(buffer))==-1)
    {
        perror("write");
        return -1;
    }
    sleep(100);
    close(fd);
    return 0;
}