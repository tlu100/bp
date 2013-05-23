#ifndef WORD_H
#define WORD_H

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <boost/assign.hpp>

using namespace std;

typedef enum{_D, ADJA, ADJD, ADV, APPR, APPRART, APPO, APZR, ART, CARD, FM, ITJ, KOUI, KOUS, KON, KOKOM, NN, NE,
 PDS, PDAT, PIS, PIAT, PIDAT, PPER, PPOSS, PPOSAT, PRELS, PRELAT, PRF, PWS, PWAT, PWAV, PAV, PTKZU, PTKNEG, PTKVZ, PTKANT,
 PTKA, TRUNC, VVFIN, VVIMP, VVINF, VVIZU, VVPP, VAFIN, VAIMP, VAINF, VAPP, VMFIN, VMINF, VMPP, XY, KOMMA, INTERP, SONST} postags;

typedef enum{_R, Fem, Masc, Neut} genders;

typedef enum {_C, Sg, Pl} numbers;

typedef enum {_P, Nom, Gen, Dat, Akk} cases;

extern int string2int (string s);

class word {
public:
    string content;
    postags pos;
    genders gender;
    numbers number;
    cases gcase;
    int person;
    int phead;
    word();
    void parzu(string s, int tab);
};

#endif
