#pragma once

#include <chrono>
#include <string>

class Timestamp {
private:
    time_t secondsSinceEpoch_;

public:
    Timestamp();
    explicit Timestamp(int64_t secondsSinceEpoch);

    static Timestamp now();

    time_t toInt();
    std::string toString();
};