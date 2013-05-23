#include "sentence.h"

void sentence::dump() {
    for (size_t i = 0; i < this->words.size(); i++) {
        cout << this->words[i]->content << ' ';
    }
}
