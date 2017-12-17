#include "dhsort.h"
#include "sorter.h"

#ifndef __GNUC__
#include <mpi.h>
#include <omp.h>
#endif

const size_t maxHeapSort = 4;

void Asserter(const char *file, int line)
{
    std::cerr << "ASSERT at FILE:" << file << " LINE:"<< line << std::endl;
}

void siftDownPoints(Point *array, int root, int size, int coordinate)
{
    int maxChild;
    int leftChild;
    while ((leftChild = (root << 1) + 1), leftChild < size)
    {
        if (leftChild == size - 1)
            maxChild = leftChild;
        else if (array[leftChild].coords[coordinate]
                 > array[leftChild + 1].coords[coordinate])
            maxChild = leftChild;
        else
            maxChild = leftChild + 1;

        if (array[root].coords[coordinate]
                < array[maxChild].coords[coordinate]) {
            std::swap(array[root], array[maxChild]);
            root = maxChild;
        }
        else {
            break;
        }
    }
}

void hsortPoints(Point *array, size_t size, int coordinate)
{
    ASSERT(array && size > 0);
    for (int i = size / 2 - 1; i >= 0; --i)
        siftDownPoints(array, i, size, coordinate);

    for (int i = size - 1; i >= 1; --i)
    {
        std::swap(array[0], array[i]);
        siftDownPoints(array, 0, i, coordinate);
    }
}

#define PARALLEL 1
void dsortPoints(Point *array, size_t size, int coordinate, size_t sortedBlockSize)
{
    Point *a = array;
    Point *b = new Point[size];
    size_t firstHalfSize = size / 2;
    for (size_t blockSize = sortedBlockSize; blockSize < size; blockSize *= 2)
    {
        for (size_t blockStart = 0; blockStart < size; blockStart += 2 * blockSize)
        {
            size_t realFirstSize = MIN(blockSize, size - blockStart);
            size_t firstBorder = blockStart + realFirstSize;
            size_t realSecondSize = MIN(blockSize, int(size) - firstBorder);
            ASSERT(int(size) - int(firstBorder) >= 0);
            size_t secondBorder = firstBorder + realSecondSize;
#if PARALLEL == 1
            #pragma omp parallel sections num_threads(2)
            {
                #pragma omp section
                {
                    for (size_t ia = blockStart, ib = firstBorder, i = blockStart;
                         i < firstBorder; ++i)
                    {
                        if (ia >= firstBorder)
                            b[i] = a[ib++];
                        else if (ib >= secondBorder)
                            b[i] = a[ia++];
                        else if (a[ia].coords[coordinate]
                                 < a[ib].coords[coordinate])
                            b[i] = a[ia++];
                        else
                            b[i] = a[ib++];
                    }
                }
                #pragma omp section
                {
                    for (size_t ia = firstBorder - 1, ib = secondBorder - 1, i = secondBorder - 1;
                         i >= firstBorder; --i)
                    {
                        if (ia < blockStart)
                            b[i] = a[ib--];
                        else if (ib < firstBorder)
                            b[i] = a[ia--];
                        else if (a[ia].coords[coordinate]
                                 > a[ib].coords[coordinate])
                            b[i] = a[ia--];
                        else
                            b[i] = a[ib--];
                    }
                }
            }
#else
            for (size_t ia = blockStart, ib = firstBorder, i = blockStart;
                         i < secondBorder; ++i)
            {
                if (ia >= firstBorder)
                    b[i] = a[ib++];
                else if (ib >= secondBorder)
                    b[i] = a[ia++];
                else if (a[ia].coords[coordinate]
                         < a[ib].coords[coordinate])
                    b[i] = a[ia++];
                else
                    b[i] = a[ib++];
            }
#endif
        }
        std::swap(a, b);
    }
    if (a != array) {
        memcpy(array, a, size * sizeof(Point));
        delete a;
    }
    else {
        delete b;
    }
}

void dhsortPoints(Point *array, size_t size, int coordinate)
{
    for (size_t i = 0; i < size; i += maxHeapSort)
        hsortPoints(array + i, MIN(maxHeapSort, size - i), coordinate);

    dsortPoints(array, size, coordinate, maxHeapSort);
}
