#ifndef STC_H
#define STC_H

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "alg.h"
#include "cls.h"

using namespace std;

class stcClass {
public:
    vector<algClass*> en;
    vector<algClass*> de;
    vector<clsClass*> clauses;
    void addWordToClause(int number, int wordIndex);
    static bool compareCls(clsClass* i, clsClass* j);
    void borders(ostream& out);
};

#endif
