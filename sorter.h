#ifndef SORTER_H
#define SORTER_H

#include <iostream>
#include <vector>
#include <ctime>

#include "dhsort.h"
#include "betcher_schedule.h"

struct Point
{
    float coords[2];
    int index;

    float &x();
    float &y();
    bool isDummy() const;
};

extern bool verbal;

typedef std::vector<float> Numbers;

template <typename T>
T random(double from, double to)
{
    return T(from + (to - from) * double(std::rand()) / RAND_MAX);
}

template <typename TContainer>
void print(const char *str, const TContainer &array)
{
    if (verbal) {
        if (str)
            std::cout << str;

        for (unsigned i = 0; i < array.size(); ++i)
            std::cout << array[i] << ' ';
        std::cout << std::endl;
    }
}


void error(const char *info, int returnValue = 0);

void printCurrentTime();

#endif // SORTER_H
