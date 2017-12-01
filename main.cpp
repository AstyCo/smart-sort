//#include <QCoreApplication>
#include <iostream>
#include <vector>
#include <ctime>

#include "dhsort.h"

bool verbal = false;

using namespace phpc;

typedef std::vector<float> Numbers;

template <typename T>
T random(double from, double to)
{
    return from + (to - from) * double(std::rand()) / RAND_MAX;
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

void print_current_time()
{
    time_t t = std::time(0);   // get time now
    struct tm * now = localtime( & t );
    std::cout   << (now->tm_year + 1900) << '-'
                << (now->tm_mon + 1) << '-'
                <<  now->tm_mday
                << std::endl;
}

int sequentialMain(int /*argc*/, char */*argv*/[])
{
    std::srand(unsigned(std::time(0)));

    int count;
    while (std::cin >> count)
    {
        if (count <= 0) {
            std::cerr << "Positive integer expected!" << std::endl;
            continue;
        }
        Numbers array;
        array.reserve(count);

        for (int i = 0; i < count; ++i)
            array.push_back(random<float>(-100, 100));

        print("Generated array: ", array);

        Numbers array_cp(array);
        Numbers array_cp2(array);


        clock_t begin = clock();
        hsort(array);
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "Elapsed secs hsort: " << elapsed_secs << std::endl;
        print("Sorted array: ", array);

        begin = clock();
        dsort(array_cp);
        end = clock();
        elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "Elapsed secs dsort: " << elapsed_secs << std::endl;
        print("Sorted array: ", array_cp);

        begin = clock();
        dhsort(array_cp2);
        end = clock();
        elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "Elapsed secs dhsort: " << elapsed_secs << std::endl;
        print("Sorted array: ", array_cp2);

        ASSERT(array == array_cp);
        ASSERT(array == array_cp2);
    }

    return 0;
}

#ifndef __GNUC__
    int mpiMain(int argc, char *argv[])
    {

    }
#endif

int main(int argc, char *argv[])
{
#ifdef __GNUC__
    return sequentialMain(argc, argv);
#else
    return mpiMain(argc, argv);
#endif
}
