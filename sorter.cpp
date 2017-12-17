#include "sorter.h"

bool verbal = false;


void printCurrentTime()
{
    time_t t = std::time(0);   // get time now
    struct tm * now = localtime( & t );
    std::cout   << (now->tm_year + 1900) << '-'
                << (now->tm_mon + 1) << '-'
                <<  now->tm_mday
                 << std::endl;
}

void error(const char *info, int returnValue)
{
    std::cerr << info << '\n';
    exit(returnValue);
}

float &Point::x() { return coords[0];}

float &Point::y() { return coords[1];}

bool Point::isDummy() const { return index == -1;}
