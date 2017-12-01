#include "dhsort.h"

using namespace phpc;

const size_t phpc::maxHeapSort = 4;

void phpc::Asserter(const char *file, int line)
{
    std::cerr << "ASSERT at FILE:" << file << " LINE:"<< line << std::endl;
}
