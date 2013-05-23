#include "alg.h"

algClass::algClass(string w, int m) {
    this->word = w;
    this->matching.push_back(m);
}

algClass::algClass() {
    this->used = false;
}


