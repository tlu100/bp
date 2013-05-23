#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include "alg.h"
#include "cls.h"
#include "stc.h"

using namespace std;

// soubor s alignmentem
ifstream algFile;
// soubor s klauzemi
ifstream clsFile;
// vystupni soubor
ofstream outFile;

// pomocna funkce na prevod stringu na int
int string2int (string s) {
    istringstream i(s);
    int cislo;
    i >> cislo;
    return cislo;
}

void clauses(stcClass* s) {
    char c;
    int wordIndex = 0;
    int clsNumber = 0;
    string str = "";
    vector<int> zeroClause;
    while (clsFile.good()) {
        c = clsFile.get();
        if (c == '\n') {
            // seradime slova klauzi podle puvodniho poradi v hypoteze a vyradime ty prazdne
            for (size_t j = 0; j < s->clauses.size(); j++) {
                if (s->clauses[j]->words.size() > 0)
                    sort(s->clauses[j]->words.begin(), s->clauses[j]->words.end());
                else {
                    s->clauses.erase(s->clauses.begin() + j);
                    j--;
                }
            }
            // nechame identifikovat hranice nemeckych klauzi
            s->borders(outFile);
            return;
        }
        switch(c) {
        // pred | stoji slovo - to nepotrebujeme
        case '|':
            str = "";
            break;
        // mezera oddeluje slovo a nyni mame nactene cislo klauze
        case ' ':
            clsNumber = string2int(str);
            // je-li cislo klauze > 0, pridame slovo do klauze - nejprve pridame k teto klauzi i ta slova,
            // co mela cislo klauze 0
            // jinak pridame do seznamu slov s nulou, ktere cekaji na naledujici klauzi, kam je vlozime
            if (clsNumber > 0) {
                    if (zeroClause.size() > 0) {
                        for (size_t i = 0; i < zeroClause.size(); i++)
                            s->addWordToClause(clsNumber, zeroClause[i]);
                        zeroClause.clear();
                    }
                    s->addWordToClause(clsNumber, wordIndex);
            } else zeroClause.push_back(wordIndex);
            wordIndex++;
            str = "";
            break;
        default:
            str += c;
            break;
        }
    }
}

int main(int argc, char** argv)
{

    if (argc < 4) {
        cerr << "Chybejici parametr!" << endl;
        return 0;
    }

    algFile.open(argv[1]);
    clsFile.open(argv[2]);
    outFile.open(argv[3]);

    char c;
    int tab = 0;
    stcClass* sentence = new stcClass();
    string str = "";
    int matchEn = -1;
    int matchDe = -1;
    while (algFile.good()) {
        c = algFile.get();
        if (c == '\n') {
            clauses(sentence);
            tab = 0;
            str = "";
            delete sentence;
            sentence = new stcClass();
            outFile << '\n';
        }
        switch (tab) {
        case 1:
        case 2:
            switch(c){
            case ' ':
            case '\t':
                // mezery jako slova nechceme - ani prazdna slova
                if (str != "" && str != " ") {
                    if (tab == 1) sentence->en.push_back(new algClass(str, -1));
                    else sentence->de.push_back(new algClass(str, -1));
                }
                str = "";
                break;
            default:
                str += c;
                break;
            }
            break;
        // zajima nas 9. sloupec alignmentu - zarovnani int
        case 9:
            switch(c) {
            case '-':
                matchEn = string2int(str);
                str = "";
                break;
            case ' ':
            case '\t':
                // obcas je tam mezera navic
                if (str != "") {
                    // ulozime odpovidajici si slova dle alignmentu
                    matchDe = string2int(str);
                    sentence->en[matchEn]->matching.push_back(matchDe);
                    str = "";
                }
                break;
            default:
                str += c;
                break;
            }
        break;
        }
        if (c == '\t') tab++;
    }
    return 0;
}
