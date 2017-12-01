#include "betcher_schedule.h"

using namespace std;

vector<Comparator> result;

const char *byte_to_binary_n(int x, int id)
{
    static char b[10][26];
    if (id > 9)
        id = 0; // max - 9

    b[id][0] = 0;

    int z;
    for (z = 1 << 24; z > 0; z >>= 1)
    {
        strcat(b[id], ((x & z) == z) ? "1" : "0");
    }

    return b[id];
}

const char *byte_to_binary(int x)
{
    return byte_to_binary_n(x, 0);
}

int findMaxTact(const vector<Comparator> &v)
{
    if (v.size() == 0)
        return 0;
    unsigned maxTactIndex = 0;
    for (unsigned i = 1; i < v.size(); ++i)
        if (v[i].tact > v[maxTactIndex].tact)
            maxTactIndex = i;

    return v[maxTactIndex].tact;
}

string toReport(const vector<Comparator> &result)
{
    ostringstream stream;
    for (unsigned i = 0; i < result.size(); ++i)
        stream << toString(result.at(i).a) << ' '
               << toString(result.at(i).b) << ' '
               << toString(result.at(i).tact) << endl;
    stream << "n_comp " << toString(result.size()) << endl
           << "n_tact " << toString(findMaxTact(result)) << endl;
    return stream.str();
}

string toAnswer(int n, const vector<Comparator> &result)
{
    ostringstream stream;
    stream << toString(n) << " 0 0" << endl;
    for (unsigned i = 0; i < result.size(); ++i)
        stream << toString(result.at(i).a) << ' '
               << toString(result.at(i).b) << endl;
    stream << toString(result.size()) << endl
           << toString(findMaxTact(result));
    return stream.str();
}

void testResult(int size)
{
    int maxTactP1 = findMaxTact(result) + 1;
    set<int> nodes;
    for (int i = 1; i < maxTactP1; ++i) {
        bool tactFound = false;
        nodes.clear();
        for (unsigned j = 0; j < result.size(); ++j) {
            if (result[j].tact != i) {
                if (tactFound)
                    break;
                else
                    continue;
            }
            tactFound = true;
            if (nodes.find(result[j].a) != nodes.end()
                    || nodes.find(result[j].b) != nodes.end()) {
                cout << "ERROR: network of size " << toString(size) << endl;
                exit(1);
            }
            nodes.insert(result[j].a);
            nodes.insert(result[j].b);
        }
    }
}

void reverseResultTacts()
{
    int maxTactP1 = findMaxTact(result) + 1;
    for (unsigned i = 0; i < result.size(); ++i)
        result[i].tact = maxTactP1 - result[i].tact;
}

void S(int a, int b, int d, int n, int m, int tact)
{
    if (n == 0 || m == 0)
        return;
    if (n == 1 && m == 1) {
        addComparator(a, b, tact);
        return;
    }
    int halfN = n - n / 2;
    int halfM = m - m / 2;
    int i;

    S(a, b, 2 * d, halfN, halfM, tact + 1);
    S(a + d, b + d, 2 * d, n - halfN, m - halfM, tact + 1);

    for (i = 1; i < n - 1; i += 2)
        addComparator(a + d * i, a + d * (i + 1), tact);

    if (n % 2 == 0) {
        addComparator(a + d * (n - 1), b, tact);
        i = 1;
    }
    else {
        i = 0;
    }

    for (; i < m - 1; i += 2)
        addComparator(b + d * i, b + d * (i + 1), tact);
}

void B0(int first, int step, int count, int tact)
{
    if (count < 2)
        return;
    if (count == 2) {
        addComparator(first, first + step, tact + 1);
        return;
    }
    int half = count - count / 2;
    int countOfMergeSteps = ceil(log2(half)) + 1;

    B0(first, step, half, tact + countOfMergeSteps);
    B0(first + step * half, step, count - half, tact + countOfMergeSteps);
    S(first, first + step * half, step, half, count - half, tact + 1);
}

void B(int first, int step, int count)
{
    result.clear();
    B0(first, step, count, 1);
    //    reverseResultTacts();
    newSetTacts();
}

int makeAnswer(int permutation)
{
    int oneCount = 0;
    for (int i = 1, r = 1; i < 25; ++i, r <<= 1) {
        if (r & permutation)
            ++oneCount;
    }
    return (1 << oneCount) - 1;
}

void swap_1_0(int a, int b, int &array, int array_size)
{
    const int a_mask = 1 << (array_size - a - 1);
    const int b_mask = 1 << (array_size - b - 1);

    if ((array & a_mask) && !(array & b_mask)) {
        array ^= a_mask | b_mask;
    }
}

int sorted(int array, int size)
{
    for (unsigned i = 0; i < result.size(); ++i)
        swap_1_0(result[i].a, result[i].b, array, size);
    return array;
}

void myAssert(int permutation, int answer, int size)
{
    int result = sorted(permutation, size);
    if (result != answer) {
        cout << "ERROR: " << byte_to_binary(permutation) << ' '
             << byte_to_binary_n(answer, 1) << ' '
             << byte_to_binary_n(result, 2) << ' ' << size << endl;
        exit(1);
    }
}

void clearTacts()
{
    for (unsigned i = 0; i < result.size(); ++i)
        result[i].tact = -1;
}

void newSetTacts()
{
    set<int> nodes;
    int currentTact = 1;

    clearTacts();
    for (unsigned i = 0; i < result.size(); ++i) {
        Comparator &comp1 = result[i];
        if (comp1.tact != -1)
            continue;

        for (unsigned j = i; j < result.size(); ++j) {
            Comparator &comp = result[j];
            if (comp.tact != -1)
                continue;

            if (!contains(nodes, comp.a) && !contains(nodes, comp.b))
                comp.tact = currentTact;

            nodes.insert(comp.a);
            nodes.insert(comp.b);
        }
        ++currentTact;
        nodes.clear();
    }
}

bool test(int n)
{
    B(0, 1, n);
    sort(result.begin(), result.end());
    testResult(n);
    int lastPermutation = 1 << n;
    for (int permutation = 0; permutation < lastPermutation; ++permutation) {
        int answer = makeAnswer(permutation);
        myAssert(permutation, answer, n);
    }
    return true;
}

void makeMyReportToFile(ofstream &file, int n)
{
    B(0, 1, n);
    //    sort(result.begin(), result.end());
    file << toReport(result) << endl;
}

void addComparator(int a, int b, int tact)
{
    result.push_back(Comparator(a, b, tact));
}
