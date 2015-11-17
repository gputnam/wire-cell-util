#include "WireCellUtil/Type.h"
#include "WireCellUtil/Testing.h"

#include <iostream>
#include <vector>
using namespace std;
using namespace WireCell;

int main()
{
    int i;
    vector<int> vi;


    cerr << "int: " << type(i) << endl;
    cerr << "vector<int>: " << type(vi) << endl;

    AssertMsg("std::vector<int, std::allocator<int> >" == type(vi), "Can not pretty print C++ types on your compiler");

}
