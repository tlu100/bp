#ifndef SENTENCE_H
#define SENTENCE_H

#include <vector>
#include <iostream>
#include "word.h"

using namespace std;

class sentence {
public:
    vector<word*> words;
    void dump();
};

#endif
