#ifndef CLS_H
#define CLS_H

#include <vector>

#include "cls.h"

using namespace std;

class clsClass {
public:
    int number;
    vector<int> words;
    vector<int> borderL;
    vector<int> borderR;
    clsClass(int n);
};

#endif
