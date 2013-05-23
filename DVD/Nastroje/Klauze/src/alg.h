#ifndef ALG_H
#define ALG_H

#include <string>
#include <vector>

using namespace std;

class algClass {
public:
    string word;
    vector<int> matching;
    bool used;
    algClass();
    algClass(string w, int m);
};

#endif
