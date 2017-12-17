#ifndef BETCHER_SCHEDULE_H
#define BETCHER_SCHEDULE_H

#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <string>
#include <ctime>
#include <sstream>
#include <fstream>

#include <math.h>
#include <string.h>

#include "dhsort.h"


extern int maxTact;

template<typename T>
std::string toString(const T &value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template<typename T>
T fromString(const std::string &str)
{
    T value;
    std::istringstream ss(str);
    ss >> value;
    return value;
}

template <typename TContainer, typename T>
bool contains(const TContainer &nodes, T i)
{
    return nodes.find(i) != nodes.end();
}

struct Comparator
{
    int a;
    int b;
    int tact;

    Comparator() {}
    Comparator(int a_, int b_, int tact_)
        : a(a_), b(b_), tact(tact_) {}

    bool operator<(const Comparator &another) const
    {
        return this->tact < another.tact;
    }
};

class BetcherNet
{
    std::vector<int> _pairs;
    const int _rank;
    int _nTact;
public:
    explicit BetcherNet(int rank, int nTact);

    void fill(const std::vector<Comparator> &schedule);

    bool isValidPair(size_t index);

    void append(int tact, int pair);
    int size() const { return _pairs.size();}
    int pair(int tact) const { return _pairs[tact];}
    int maxTact() const { return _nTact;}

};

extern std::vector<Comparator> betcherSchedule;

const char *byte_to_binary_n(int x, int id);

const char *byte_to_binary(int x);

int findMaxTact(const std::vector<Comparator> &v);

std::string toReport(const std::vector<Comparator> &betcherSchedule);

std::string toAnswer(int n, const std::vector<Comparator> &betcherSchedule);

void testResult(int size);

void reverseResultTacts();

void addComparator(int a, int b, int tact);

void S(int a, int b, int d, int n, int m, int tact);

void B0(int first, int step, int count, int tact);

void newSetTacts();

void B(int first, int step, int count);

int makeAnswer(int permutation);

void swap_1_0(int a, int b, int &array, int array_size);

int sorted(int array, int size);

void myAssert(int permutation, int answer, int size);

void clearTacts();

void newSetTacts();

bool test(int n);

void makeMyReportToFile(std::ofstream &file, int n);

#endif // BETCHER_SCHEDULE_H
