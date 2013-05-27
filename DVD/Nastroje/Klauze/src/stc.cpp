#include "stc.h"

// slouzi pro pridani slova do klauze
void stcClass::addWordToClause(int number, int wordIndex) {
    int clsIndex = -1;
    for (size_t i = 0; i < this->clauses.size(); i++)
        if (this->clauses[i]->number == number) {
                clsIndex = i;
                break;
        }
    if (clsIndex == -1) {
        this->clauses.push_back(new clsClass(number));
        clsIndex = this->clauses.size() - 1;
    }
    // pridani indexu nemeckeho slova dle alignmentu

    for (size_t i = 0; i < this->en[wordIndex]->matching.size(); i++) {
        int matchWord = this->en[wordIndex]->matching[i];
        if (matchWord >= 0 && !this->de[matchWord]->used) {
            this->clauses[clsIndex]->words.push_back(matchWord);
            this->de[matchWord]->used = true;
        }
    }
}

// pomocna funkce pro porovnavani klauzi dle prvniho slova
bool stcClass::compareCls(clsClass* i, clsClass* j) {
        return i->words[0] < j->words[0];
}

// metoda pro identifikaci a vypsani klauzi, ktere pokryji celou vetu
void stcClass::borders(ostream& out) {
	// serazeni klauzi podle prvniho slova
    sort(this->clauses.begin(), this->clauses.end(), stcClass::compareCls);
    size_t sentenceLength = this->de.size();
    // na zaklade nejnizsiho a nejvyssiho indexu slova v klauzi navrhneme hranice
    for (size_t j = 0; j < this->clauses.size(); j++) {
        this->clauses[j]->borderL.push_back(this->clauses[j]->words[0]);
        this->clauses[j]->borderR.push_back(this->clauses[j]->words[this->clauses[j]->words.size() - 1]);
    }

    // pokud alignment nezarovnal nic
    if (this->clauses.size() == 0) {
        this->clauses.push_back(new clsClass(0));
        this->clauses[0]->borderL.push_back(0);
        this->clauses[0]->borderR.push_back(this->de.size()-1);
    }

	// zkontrolujeme, zda se vety neprekryvaji nebo zda jsou vlozene
    int totalCls = this->clauses.size();
    // pomocny vektor, kazdy prvek predstavuje pozici ve vete
    vector<int> n;
    // nejprve pridame same -1, abychom mohli poznat mezery mezi klauzemi
    for (size_t i = 0; i < sentenceLength; i++) n.push_back(-1);
    bool nested;
    int first;
    for (int j = 0; j < totalCls; j++) {
    	   // pokud aktualni hranice zasahuje do mista, kde uz nejaka klauze je
        if (n[this->clauses[j]->borderR[0]] != -1) {
            // zapamatujeme si hodnotu na teto pozici
            first = this->clauses[j]->borderL[0];
            // projdeme ji az do konce a pokud narazime na jinou hodnotu, pak se vety prekryvaji
            // pokud bude hodnota vsude stejna, jedna se o vlozenou vetu
            for (int k = this->clauses[j]->borderL[0]; k <= this->clauses[j]->borderR[0]; k ++) {
                if (first != n[k]) {
                    nested = false;
                    break;
                }
            }
            nested = true;
        }
        // je-li veta vlozena nebo na pozici zadna neni, zapiseme tam aktualni
        for (int k = this->clauses[j]->borderL[0]; k <= this->clauses[j]->borderR[0]; k ++) {
            if (n[k] == -1 || nested) n[k] = j;
        }
        // puvodni hranice smazeme
        this->clauses[j]->borderL.clear();
        this->clauses[j]->borderR.clear();
    }

    // vyplnime ty nezarovnane
    int lastNo = 0;
    for (size_t i = 0; i < n.size(); i++)
        if (n[i] != -1) {
            lastNo = n[i];
            break;
        }
    for (size_t i = 0; i < n.size(); i++) {
        if (n[i] == -1) n[i] = lastNo;
        else lastNo = n[i];
    }

    int sentenceNo;
    sentenceNo = n[0];
    this->clauses[sentenceNo]->borderL.push_back(0);
	// projdeme pozice slov a najdeme finalni hranice
    for (size_t i = 0; i < n.size(); i++) {
        if (n[i] == sentenceNo) continue;
        else {
            this->clauses[sentenceNo]->borderR.push_back(i-1);
            sentenceNo = n[i];
            this->clauses[sentenceNo]->borderL.push_back(i);
        }
    }
	// vypiseme hranice na vystup
    this->clauses[sentenceNo]->borderR.push_back(n.size()-1);
    for (int j = 0; j < totalCls; j++) {

        for (size_t i = 0; i < this->clauses[j]->borderL.size(); i++) {
            if (i > 0) out << '+';
            out << this->clauses[j]->borderL[i] << '-' << this->clauses[j]->borderR[i];
        }
        out << '|';
    }
}


