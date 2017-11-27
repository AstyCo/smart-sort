#ifndef DHSORT_H
#define DHSORT_H

#include <algorithm>

#include <cstring>

#define ASSERT(x) Q_ASSERT(x)

namespace phpc
{

template <typename T>
void siftDown(T *array, int root, int size)
{
    int maxChild;
    int leftChild;
    while ((leftChild = (root << 1) + 1), leftChild < size)
    {
        if (leftChild == size - 1)
            maxChild = leftChild;
        else if (array[leftChild] > array[leftChild + 1])
            maxChild = leftChild;
        else
            maxChild = leftChild + 1;

        if (array[root] < array[maxChild]) {
            std::swap(array[root], array[maxChild]);
            root = maxChild;
        }
        else {
            break;
        }
    }
}

template <typename T>
void hsort(T *array, std::size_t size)
{
    ASSERT(array && size > 0);
    for (int i = size / 2 - 1; i >= 0; --i)
        siftDown(array, i, size);

    for (int i = size - 1; i >= 1; --i)
    {
        std::swap(array[0], array[i]);
        siftDown(array, 0, i);
    }
}

template <typename T>
void hsort(std::vector<T> &array)
{
    hsort(array.data(), array.size());
}

#define MIN(a,b) ((a)<(b)? (a) :(b))
#define MAX(a,b) ((a)<(b)? (b) :(a))

template <typename T>
void dsort(T *array, size_t size, size_t sortedBlockSize = 1)
{
    T *a = array;
    T *b = new T[size];
    for (size_t blockSize = sortedBlockSize; blockSize < size; blockSize *= 2)
    {
        for (size_t blockStart = 0; blockStart < size; blockStart += 2 * blockSize)
        {
            size_t realFirstSize = MIN(blockSize, size - blockStart);
            size_t firstBorder = blockStart + realFirstSize;
            size_t realSecondSize = MIN(blockSize, int(size) - firstBorder);
            ASSERT(int(size) - int(firstBorder) >= 0);
            size_t secondBorder = firstBorder + realSecondSize;

            for (size_t ia = blockStart, ib = firstBorder, i = blockStart;
                 i < secondBorder; ++i)
            {
                if (ia >= firstBorder)
                    b[i] = a[ib++];
                else if (ib >= secondBorder)
                    b[i] = a[ia++];
                else if (a[ia] < a[ib])
                    b[i] = a[ia++];
                else
                    b[i] = a[ib++];
            }
        }
        std::swap(a, b);
    }
    if (a != array) {
        memcpy(array, a, size * sizeof(T));
        delete a;
    }
    else {
        delete b;
    }
}

template <typename T>
void dsort(std::vector<T> &array)
{
    ASSERT(array.size() > 0);
    dsort(array.data(), array.size());
}


const size_t max_heap_sort = 4;

template <typename T>
void dhsort(T *array, size_t size)
{
    for (size_t i = 0; i < size; i += max_heap_sort)
        hsort(array + i, MIN(max_heap_sort, size - i));

    dsort(array, size, max_heap_sort);
}

template <typename T>
void dhsort(std::vector<T> &array)
{
    ASSERT(array.size() > 0);
    dhsort(array.data(), array.size());
}

} // namespace phpc

#endif // DHSORT_H
