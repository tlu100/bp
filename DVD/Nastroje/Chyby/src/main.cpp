#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <boost/assign.hpp>

#include "word.h"
#include "sentence.h"

using namespace std;
using std::cout;

int string2int (string s) {
    istringstream i(s);
    int number;
    i >> number;
    return number;
}

ifstream parzu("tmp0");
ifstream borders;

map<postags, bool> wasWClass;
map<cases, bool> wasGCase;

// vrati, zda uz tenhle slovni pos byl
bool wasIt(postags wClass) {
    return (wasWClass.find(wClass) != wasWClass.end() ? wasWClass[wClass] : false);
}
bool wasIt(cases gCase) {
    return (wasGCase.find(gCase) != wasGCase.end() ? wasGCase[gCase] : false);
}

// byl infinitiv nebo pricesti
bool wasINFPP() {
    return (wasIt(VVPP) || wasIt(VVINF) || wasIt(VAINF) || wasIt(VAPP) || wasIt(VMINF) || wasIt(VMPP));
}

// identifikuje rysy v klauzi
map<string, bool> features(vector<word*> stc) {
    map<string, bool> ftrs;

    // je-li veta dlouha 1, pak ji preskocit, protoze o ni nemuzu nic moc rict a naskakovaly by zbytecne
    // chyby, ze chybi sloveso apod.
    if (stc.size() == 1) return ftrs;

    int person = 0;
    numbers number = _C;
    int conc = 0;

    for (size_t i = 0; i < stc.size(); i++) {

        word* s = stc[i];

        switch (s->pos) {
        case VAFIN:
        case VVFIN:
        case VMFIN:
            if (wasIt(VVFIN) || wasIt(VAFIN) || wasIt(VMFIN)) {
                //chyby.push_back("Dve urcita slovesa v jedne vete.");
                ftrs["vice_vfin"] = true;
            }
            break;
        case VVPP:
        case VAPP:
        case VMPP:
                // KON mohlo oddelit ich habe etwas gekocht und dann auch fotografiert.
                if (!wasIt(VAFIN) && !(wasIt(KOUS) || wasIt(PRELS) || wasIt(KON))) ftrs["pp_bez_av"] = true;//chyby.push_back("Pricesti minule bez pomocneho slovesa");
            break;
        // jen proto, aby neskocilo do default
        case INTERP:
        case KOMMA:
        case SONST:
            break;
        case VVINF:
        case VAINF:
        case VMINF:
            break;
        default:
            if ((wasIt(VAFIN) && wasINFPP()) && !(wasIt(KOUS) || wasIt(PRELS)) ) {
                //chyby.push_back("Pricesti minule neni na konci vety.");
                ftrs["pp_neni_na_konci"] = true;
            }
            if ((wasIt(KOUS) || wasIt(PRELS)) && (wasIt(VVFIN) || wasIt(VAFIN))) {
                //chyby.push_back("Sloveso neni ve vedlejsi vete na konci.");
                ftrs["vv_sloveso_neni_na_konci"] = true;
            }
            if (wasIt(KOUI) && (wasIt(VVIZU) || (wasIt(PTKZU) && (wasIt(VAINF) || wasIt(VMINF) || wasIt(VVINF))))) {
                //chyby.push_back("Infinitiv s zu neni na konci vety.");
                ftrs["infszu_neni_na_konci"] = true;
            }
            if (wasIt(VMFIN) && (wasIt(VAINF) || wasIt(VMINF) || wasIt(VVINF))) {
                ftrs["inf_po_vm_neni_na_konci"] = true;
            }
            break;
        }
        wasWClass[s->pos] = true;
        wasGCase[s->gcase] = true;
        if (s->person != 0 && (s->gcase == _P || s->gcase == Nom)) {
            if (person == 0) {
                person = s->person;
                number = s->number;
            } else if (s->person != person || (s->number != number && number != _C)) ftrs["vice_osob"] = true;
            else conc++;
        }
        // pokud slo nejake jmeno v prvnim gcasee (nebo PDS - zajmeno), je to treti person
        if (person == 0 && (s->pos == NE || s->pos == NN || s->pos == PDS) && s->gcase == Nom) {
            person = 3;
            number = s->number;
        }
    }

    // prvni person a number se uz nevyskytla
    if (conc == 0) ftrs["neshoda_podmet_prisudek"] = true;

    // nebylo KON - ich habe es gemacht| und gekocht
    if (!(wasIt(VVFIN) || wasIt(VAFIN) || wasIt(VMFIN)) && !wasIt(KON))
        ftrs["chybi_vfin"] = true;

    // byla spojka, ktera chce zu, ale nebyl infinitiv s zu
    if (wasIt(KOUI) && !(wasIt(VVIZU) || (wasIt(PTKZU) && (wasIt(VAINF) || wasIt(VMINF) || wasIt(VVINF))))) {
        //chyby.push_back("Chybel infinitiv s zu.");
        ftrs["chybi_infszu"] = true;
    }
    if (!wasIt(Nom)) ftrs["chybi_podmet"] = true;
    return ftrs;
}

int main(int argc, char** argv)
{
    if (argc < 2) {
        cerr << "Chybejici parametr!" << endl;
        return 0;
    }

    borders.open(argv[1]);

    // nacteni vety a morfologicke analyzy z parzu
    sentence* ve = new sentence();
    word* sl = new word();

    vector<word*> clause;
    // rysy klauze
    map<string, bool> clsFtrs;
    // rysy cele vety
    map<string, int> stcFtrs;

    string s = "";
    int tab = 1;
    char c = parzu.get();
    while (parzu.good()) {

        stcFtrs["vice_vfin"] = 0;
        stcFtrs["pp_bez_av"] = 0;
        stcFtrs["pp_neni_na_konci"] = 0;
        stcFtrs["vv_sloveso_neni_na_konci"] = 0;
        stcFtrs["infszu_neni_na_konci"] = 0;
        stcFtrs["chybi_infszu"] = 0;
        stcFtrs["inf_po_vm_neni_na_konci"] = 0;
        stcFtrs["chybi_podmet"] = 0;
        stcFtrs["chybi_vfin"] = 0;
        stcFtrs["neshoda_podmet_prisudek"] = 0;
        stcFtrs["vice_osob"] = 0;

        while (parzu.good()) {
            // prazdny radek - konec vety
            if (c == '\n' && s == "") break;
            switch (c) {
            case '\t':
                sl->parzu(s, tab);
                tab++;
                s = "";
                break;
            case '|':
                sl->parzu(s, tab);
                s = "";
                break;
            case '\n':
                sl->parzu(s, tab);
                s = "";
                ve->words.push_back(sl);
                sl = new word();
                tab = 1;
                break;
            default:
                if (c != ' ') s += c;
                break;
            }
            c = parzu.get();
        }

        // nacteni hranici klauzi a urceni rysu
        s = "";
        c = borders.get();
        int borderL = -1;
        int borderR = -1;
        while (c != '\n') {
            switch (c) {
                case '|':
                case '+':
                case '-':
                    if (borderL == -1) borderL = string2int(s);
                    else {
                        borderR = string2int(s);
                        for (int i = borderL; i <= borderR; i++)
                            clause.push_back(ve->words[i]);
                        borderL = -1;
                        borderR = -1;
                    }
                    // konec klauze
                    if (c == '|') {
                        clsFtrs = features(clause);
                        for (map<string, bool>::iterator i = clsFtrs.begin(); i != clsFtrs.end(); i++)
                            if (i->second) {
                                if (stcFtrs.find(i->first) != stcFtrs.end()) stcFtrs[i->first]++;
                                else stcFtrs[i->first] = 1;
                            }
                        clause.clear();
                        wasWClass.clear();
                        wasGCase.clear();
                    }
                    s = "";
                    break;
                default:
                    s += c;
                    break;
            }
            c = borders.get();
        }


        // INTERP, KOMA, SONST - taky maji nulu - vynechame je
        int root = 0;
        for (size_t i = 0; i < ve->words.size(); i++) {
            if (ve->words[i]->phead == -1 && !(ve->words[i]->pos == INTERP || ve->words[i]->pos == KOMMA || ve->words[i]->pos == SONST))
                root++;
        }

        // vytvorime rys sum sectenim vsech ostatnich
        int sum = 0;
        for (map<string, int>::iterator i = stcFtrs.begin(); i != stcFtrs.end(); i++)
            sum += i->second;        
        
        // pridame root k rysum vety
        stcFtrs["root"] = root;

        stcFtrs["sum"] = sum;
        stcFtrs["sumr"] = sum + root;        

        // pridame dalsi souctove rysy
        stcFtrs["neni_na_konci_sum"] = stcFtrs["pp_neni_na_konci"] + stcFtrs["vv_sloveso_neni_na_konci"]
                                        + stcFtrs["inf_po_vm_neni_na_konci"] + stcFtrs["infszu_neni_na_konci"];
        stcFtrs["chybi_sum"] = stcFtrs["chybi_infszu"] + stcFtrs["chybi_podmet"] + stcFtrs["chybi_vfin"];
        stcFtrs["vice_sum"] = stcFtrs["vice_vfin"] + stcFtrs["vice_osob"];

        // vypiseme rysy vety na vystup
        for (map<string, int>::iterator i = stcFtrs.begin(); i != stcFtrs.end(); i++)
            cout << i->first << ':' << i->second << ' ';
        stcFtrs.clear();
        c = parzu.get();
        wasGCase.clear();
        wasWClass.clear();
        cout << endl;
        ve = new sentence();
    }
    return 0;
}
