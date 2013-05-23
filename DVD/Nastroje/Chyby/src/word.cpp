#include "word.h"

map<string, postags> posMap = boost::assign::map_list_of("ADJA", ADJA) ("ADJD", ADJD) ("ADV", ADV) ("APPR", APPR)
    ("APPRART", APPRART) ("APPO", APPO) ("APZR", APZR) ("ART", ART) ("CARD", CARD) ("FM", FM) ("ITJ", ITJ) ("KOUI", KOUI)
    ("KOUS", KOUS) ("KON", KON) ("KOKOM", KOKOM) ("NN", NN) ("NE", NE) ("PDS", PDS) ("PDAT", PDAT) ("PIS", PIS) ("PIAT", PIAT)
    ("PIDAT", PIDAT) ("PPER", PPER) ("PPOSS", PPOSS) ("PPOSAT", PPOSAT) ("PRELS", PRELS) ("PRELAT", PRELAT) ("PRF", PRF)
    ("PWS", PWS) ("PWAT", PWAT) ("PWAV", PWAV) ("PAV", PAV) ("PTKZU", PTKZU) ("PTKNEG", PTKNEG) ("PTKVZ", PTKVZ) ("PTKANT", PTKANT)
    ("PTKA", PTKA) ("TRUNC", TRUNC) ("VVFIN", VVFIN) ("VVIMP", VVIMP) ("VVINF", VVINF) ("VVIZU", VVIZU) ("VVPP", VVPP)
    ("VAFIN", VAFIN) ("VAIMP", VAIMP) ("VAINF", VAINF) ("VAPP", VAPP) ("VMFIN", VMFIN) ("VMINF", VMINF) ("VMPP", VMPP) ("XY", XY)
    ("KOMMA", KOMMA) ("INTERP", INTERP) ("SONST", SONST);
map<string, genders> genderMap = boost::assign::map_list_of("Fem", Fem) ("Masc", Masc) ("Neut", Neut);
map<string, numbers> numberMap = boost::assign::map_list_of("Sg", Sg) ("Pl", Pl);
map<string, cases> caseMap = boost::assign::map_list_of("Nom", Nom) ("Gen", Gen) ("Dat", Dat) ("Akk", Akk);

word::word() {
    this->pos = _D;
    this->gender = _R;
    this->number = _C;
    this->gcase = _P;
    this->person = 0;
    this->phead = -2;
}

void word::parzu(string s, int tab) {
    switch(tab) {
    case 1:
        this->content = s;
        break;
    case 2:
        this->pos = posMap[s];
        break;
    case 3:
        if (s == "Fem" || s == "Masc" || s == "Neut")
            this->gender = genderMap[s];
        else if (s == "1" || s == "2" || s == "3")
            this->person = string2int(s);
        else if (s == "Nom" || s == "Gen" || s == "Dat" || s == "Akk")
            this->gcase = caseMap[s];
        else if (s == "Sg" || s == "Pl")
            this->number = numberMap[s];
        break;
    case 4:
        // indexy slov mame od nuly, tedy o jedna nizsi
        this->phead = string2int(s) - 1;
        break;
    }
}

