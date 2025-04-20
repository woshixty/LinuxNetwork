#include "Timestamp.h"

#include <sys/time.h>

#include <cassert>

Timestamp::Timestamp() {
    secondsSinceEpoch_ = time(0);
}

Timestamp::Timestamp(int64_t secondsSinceEpoch) {
    secondsSinceEpoch_ = secondsSinceEpoch;
}

Timestamp Timestamp::now() {
    return Timestamp();
}

time_t Timestamp::toInt() {
    return secondsSinceEpoch_;
}

std::string Timestamp::toString() {
    char buf[128] = {0};
    tm *timeInfo = localtime(&secondsSinceEpoch_);
    snprintf(
        buf, 
        sizeof(buf), 
        "%4d/%02d/%02d %02d:%02d:%02d",
        timeInfo->tm_year + 1900, 
        timeInfo->tm_mon + 1, 
        timeInfo->tm_mday,
        timeInfo->tm_hour, 
        timeInfo->tm_min, 
        timeInfo->tm_sec
    );
    return buf;
}

// 编写测试的main函数
// #include <iostream>
// int main() {
//     Timestamp now = Timestamp::now();
//     std::cout << now.toString() << std::endl;
//     std::cout << now.toInt() << std::endl;
//     return 0;
// }