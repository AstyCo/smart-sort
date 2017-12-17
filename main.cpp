#include "sorter.h"
#include <sstream>
#include <iostream>

#ifndef __GNUC__
#include <mpi.h>
#include <omp.h>
#endif

// Check GCC
#if __GNUC__
#if __x86_64__ || __ppc64__
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

int sequentialMain(int /*argc*/, char */*argv*/[])
{
    std::srand(unsigned(std::time(0)));

    int count;
    
#ifdef ENVIRONMENT32
    std::cout << "Hi from 32 bit!" << std::endl;
#else
#   ifdef ENVIRONMENT64
    std::cout << "Hi from 64 bit!" << std::endl;
#   else
    std::cout << "I don't know what I am!" << std::endl;
#   endif
#endif
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
        std::sort_heap(array_cp2.begin(), array_cp2.end());
        end = clock();
        elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "Elapsed secs dhsort: " << elapsed_secs << std::endl;
        print("Sorted std::sort_heap: ", array_cp2);
        

        ASSERT(array == array_cp);
        ASSERT(array == array_cp2);
    }

    return 0;
}

bool isInputValid(const long N1, const long N2, int coordinate)
{
    ASSERT(N1 >= 0);
    if (N1 < 0)
        return false;

    ASSERT(N2 >= 0);
    if (N2 < 0)
        return false;

    ASSERT(coordinate == 0 || coordinate == 1);
    if (coordinate != 0 && coordinate != 1)
        return false;

    return true;
}

double start_time_global;

void printPoints(const Point *points, int size, int rank, int tact, int coordinate, const char * extra = "")
{
    std::stringstream ss;
    for (int i = 0; i < size; ++i) {
        if (points[i].isDummy())
            ss << "Dummy";
        else
            ss << points[i].coords[coordinate];
        ss << ' ';
    }
    std::cout << '[' << rank << ',' << tact << "] " << extra << " points:" << ss.str() << std::endl;
}

#ifndef __GNUC__
int mpiMain(int argc, char *argv[])
{
	/*
    if (argc < 4)
        error("Expected 3 arguments! <program> N1 N2 <0|1>");
    const long N1 = fromString<long>(argv[1]);
    const long N2 = fromString<long>(argv[2]);
    int coordinate = fromString<int>(argv[3]);
	*/
    const long N1 = 4000;
    const long N2 = 16000;
	int coordinate = 0;

    if (!isInputValid(N1, N2, coordinate))
        error("Wrong arguments! <program> N1>0 N2>0 <0|1>");

    int arraySize = N1 * N2;
    int rank, size;

	
    MPI_Comm comm = MPI_COMM_WORLD;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(comm, &rank);
    
    if (rank == 0) {
        std::cout << "Size: " << size << std::endl;
    }

    MPI_Status status;
    MPI_Request req;

    MPI_Datatype point;
    MPI_Datatype types[2] = { MPI_FLOAT, MPI_INT };
    int counts[2] = { 2, 1 };
    MPI_Aint disps[2] = { offsetof(Point, coords), offsetof(Point, index)};
    MPI_Type_create_struct(2, counts, disps, types, &point);
    MPI_Type_commit(&point);
    MPI_Datatype MPI_POINT = point;

    if (size < 1) {
        MPI_Finalize();
        return 0;
    }

    int firstDummy = arraySize % size;
    int p = arraySize / size + (firstDummy == 0 ? 0 : 1);
	int firstIndex;
	
	
    if (firstDummy == 0 || firstDummy >= rank)
        firstIndex = p * rank;
    else
        firstIndex = firstDummy * p + (rank-firstDummy) * (p - 1);

    Point *points = new Point[p];
    Point *bufPoints;
    if (size > 1)
        bufPoints = new Point[p];

    for (int i = 0; i < p; ++i) {
        points[i].x() = random<float>(0,  1000);
        points[i].y() = random<float>(-1000, 0);

        if (i == p - 1 && rank >= firstDummy && firstDummy > 0)
            points[i].index = -1;
        else
            points[i].index = firstIndex + i;
    }
	
    maxTact = -1;
    B(0, 1, size); // fill betcherSchedule and maxTact

    const int bufferSize = (maxTact + 1) * p * sizeof(Point) + MPI_BSEND_OVERHEAD;
    char *buffer = new char[bufferSize];

    MPI_Buffer_attach(buffer, bufferSize);
//    if (rank == 0) {
//        std::stringstream tmp;
//        for (int i = 0; i < betcherSchedule.size(); ++i) {
//            tmp << '(' << betcherSchedule[i].a  << ',' << betcherSchedule[i].b
//                << ',' << betcherSchedule[i].tact << ") ";
//        }
//        std::cout << "COMPARATORS " << tmp.str() << std::endl;
//    }
    BetcherNet net(rank, maxTact + 1);
    net.fill(betcherSchedule);
    // net -> pairs for this processor

//    printPoints(points, p, rank, 0, coordinate, "init");
    start_time_global = MPI_Wtime();
    // sort p at this processor
    dsortPoints(points, p, coordinate);

    for (int tact = 0; tact <= maxTact; ++tact)
    {
        if (!net.isValidPair(tact))
            continue;
		//std::cout << rank << " Recv " << net.pair(tact) << std::endl;
		
        MPI_Bsend(points, p, MPI_POINT, net.pair(tact), tact, comm);
        MPI_Recv(bufPoints, p, MPI_POINT, net.pair(tact), tact, comm, &status);

//        printPoints(points, p, rank, tact, coordinate, "points");
//        printPoints(bufPoints, p, rank, tact, coordinate, "bufPoints");

        for (int i=0, ai = 0, bi = 0; i < p; ++i) {
            Point *src;
            if (rank > net.pair(tact)){ // greater
                if (points[ai].coords[coordinate] > bufPoints[bi].coords[coordinate])
                    src = points + ai++;
                else
                    src = bufPoints + bi++;
            }
            else { // lower
                if (points[ai].coords[coordinate] > bufPoints[bi].coords[coordinate])
                    src = bufPoints + bi++;
                else
                    src = points + ai++;
            }

            points[i] = *src;
        }
		
//        printPoints(points, p, rank, tact, coordinate);
    }

    // output
    MPI_Barrier(comm);
    double procTime = MPI_Wtime() - start_time_global;
    if (rank == 0) {
        
        double maxTime = procTime;
        for (int proc = 1; proc < size; ++proc) {
            MPI_Recv(&procTime, 1, MPI_DOUBLE, proc, 7777, comm, &status);
            if (procTime > maxTime)
                maxTime = procTime;
        }
        std::cout << "SORTING TIME:" << maxTime << std::endl;
    }
    else {
        MPI_Send(&procTime, 1, MPI_DOUBLE, 0, 7777, comm);
    }
	
	if (rank == 0) {
        Point *allPoints = new Point[arraySize];
        memcpy(allPoints, points, p * sizeof(Point));
        for (int i = 1; i < size; i++)
            MPI_Recv(allPoints + i * p, p, MPI_POINT, i, 2345, comm, &status);
        // check if array is sorted
        for (int i = 0; i < arraySize - 1; ++i) {
            if (allPoints[i].isDummy() || allPoints[i+1].isDummy())
                continue;
            if (allPoints[i].coords[coordinate] > allPoints[i + 1].coords[coordinate]) {
                std::cout << "ASSERT! " << allPoints[i].coords[coordinate] << " > "
                          << allPoints[i+1].coords[coordinate] << " (" << i << ',' << i+1 << ')' << std::endl;
                break;
            }
        }
        // check number of "Dummy"
        int countOfDummy = 0;
        for (int i = 0; i < arraySize - 1; ++i) {
            if (allPoints[i].isDummy())
                ++countOfDummy;
        }
        if ((firstDummy == 0 && countOfDummy != 0)
                || (firstDummy > 0 && countOfDummy != size - firstDummy))
        {
            std::cout << "ASSERT! Number of Dummy: " << countOfDummy << " expected: "
                      << (firstDummy? size - firstDummy : 0) << std::endl;
        }
	}
	else {
        MPI_Send(points, p, MPI_POINT, 0, 2345, comm);
	}

    delete points;
	return 0;
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
