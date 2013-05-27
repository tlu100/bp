#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <sstream>
#include <cstring>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <math.h>

using namespace std;

int string2int (string s) {
    istringstream i(s);
    int cislo;
    i >> cislo;
    return cislo;
}

// slouzi pro natrenovani modelu - vstupem jsou dva stringy s nazvy souboru
void train(string text, string model) {
    ifstream in(text.c_str());
    ofstream out(model.c_str());
    map<string, map<string, vector<int> > > ftrs;
    char c;
    string s;
    int tab = 1;
    string fluency;
    string name;
    while (in.good()) {
        c = in.get();
        switch (c) {
        case '\n':
        case ' ':
            // muze se stat na konci radku - je tam mezera hned za ni \n
            if (s != "") {
                if (tab == 1) fluency = s;
                else {
                    ftrs[fluency][name].push_back(string2int(s));
                }
            }
            if (c == '\n') tab = 1;
            else tab++;
            s = "";
            break;
        case ':':
            name = s;
            s = "";
            break;
        default:
            s += c;
            break;
        }
    }
    // vypise jednotlive vystupy do modelu
    for (map<string, map<string, vector<int> > >::iterator i = ftrs.begin(); i != ftrs.end(); i++)
        out << i->first << '|';
    out << endl;
    vector<string> names;
    vector<string> limits;
    int n = 0;
    // vypocita jednotlive mediany podle trenovacich dat - nejprve si je ulozi, aby je mohl vypsat ve spravnem poradi
    for (map<string, map<string, vector<int> > >::iterator i = ftrs.begin(); i != ftrs.end(); i++) {
        for (map<string, vector<int> >::iterator j = ftrs[i->first].begin(); j != ftrs[i->first].end(); j++) {
        	  // nactene hodnoty setridime a uprostred nalezneme median
            sort(j->second.begin(), j->second.end());
            vector<int> kl = j->second;
            if (i == ftrs.begin()) {
                limits.push_back(boost::lexical_cast<string>(j->second[(j->second.size()-1) / 2]) + "|");
                names.push_back(j->first);
            }
            else limits[n] += boost::lexical_cast<string>(j->second[(j->second.size()-1) / 2]) + "|";
            n++;
        }
        n = 0;
    }
    // vypise mediany do modelu
    for (size_t i = 0; i < limits.size(); i++) {
        out << names[i] << endl;
        out << limits[i] << endl;
    }
}

// otestuje model na testovacich datech
void test(string predict, string model) {
    ifstream inP(predict.c_str());
    ifstream inM(model.c_str());
    string s = "";
    vector<string> outcomes;
    // nacteni outcomes
    char c = inM.get();
    while (c != '\n') {
        switch(c) {
        case '|':
            outcomes.push_back(s);
            s = "";
            break;
        default:
            s += c;
            break;
        }
        c = inM.get();
    }
    // nacteni rysu a jejich medianu
    map<string, vector<int> > ftrs;
    bool odd = true; // lichy radek - nazev rysu, sudy - mediany
    c = inM.get();
    s = "";
    string feature;
    while (inM.good()) {
        switch (c) {
        case '\n':
            odd = (odd ? false : true);
            //ftrs[s] = vector<int>;
            feature = s;
            s = "";
            break;
        case '|':
            ftrs[feature].push_back(string2int(s));
            s = "";
            break;
        default:
            s += c;
            break;
        }
        c = inM.get();
    }
    // pole, ktere bude udavat cislo vystupu podle hodnoty rysu
    map<string, vector<int> > ftr2out;
    int n = 0;
    for (map<string, vector<int> >::iterator i = ftrs.begin(); i != ftrs.end(); i++) {
        for (size_t j = 0; j < i->second.size(); j++) {
            while (ftr2out[i->first].size() <= (size_t)i->second[j]) ftr2out[i->first].push_back(-1);
            vector<int> kl = ftr2out[i->first];
            // pokud je na miste volno, vlozime tam hodnotu, pokud ne, vlozime ji na toto misto a zbytek posuneme doprava
            if (ftr2out[i->first][i->second[j]] == -1) ftr2out[i->first][i->second[j]] = n;
            else ftr2out[i->first].insert(ftr2out[i->first].begin()+i->second[j], n);
            n++;
        }
        n=0;
    }
    int lastValue = -1;
    // zaplnime hodnoty mezi mediany
    for (map<string, vector<int> >::iterator i = ftr2out.begin(); i != ftr2out.end(); i++) {
        for (int j = i->second.size() - 1; j >= 0; j--) {
            if (i->second[j] != -1) lastValue = i->second[j];
            else {
                int j2 = 0;
                while (i->second[j-j2] == -1) if (j-j2 > 0) j2++; else break;
                // j+j2 muze byt pred prvnim zleva a tam uz zadna dalsi hodnota nebude
                int nearValue = (i->second[j-j2] == -1 ? lastValue : i->second[j-j2]);
                // jsou-li mista s -1 alespon dve (j je prvni)
                if (j2 > 0) {
                    for (int k = j; k >= j-(j2/2); k--)
                        i->second[k] = lastValue;
                    for (int k = j-((j2+1)/2); k > j-j2; k--)
                        i->second[k] = nearValue;
                } else i->second[j] = lastValue;
            }
        }
    }

    int tab = 1;
    string fluency;
    string name;
    int sum = 0;
    int num = 0;
    int acc = 0;
    int total = 0;
    int differ = 0;
    // vyhodnoti testovaci data
    while (inP.good()) {
        c = inP.get();
        switch (c) {
        case '\n':
        case ' ':
            // muze se stat na konci radku - je tam mezera, hned za ni \n
            if (s != "") {
                if (tab == 1) fluency = s;
                else {
                    int index = string2int(s);
                    sum += ((size_t)index >= ftr2out[name].size() ? ftr2out[name][ftr2out[name].size()-1] : ftr2out[name][string2int(s)]);
                    num++;
                }
            }
            if (c == '\n') {
                cout << outcomes[sum/num] << endl;
                int ind = floor(sum/(float)num + 0.5);
                if (outcomes[ind] == fluency) acc++;
                // podiva se do okolnich vystupu, zda se nelisi jen o jedna
                if ((ind-1 >= 0 && outcomes[ind-1] == fluency) || ((size_t)ind+1 < outcomes.size() && outcomes[ind+1] == fluency)) differ++;
                total++;
                tab = 1;
                sum = 0;
                num = 0;
            } else tab++;
            s = "";
            break;
        case ':':
            name = s;
            s = "";
            break;
        default:
            s += c;
            break;
        }
    }
    cerr << "Accuracy: " << acc << "/" << total << " (" << (acc/(float)total) * 100 << "%" << ")" << endl;
    cerr << "Differing by one: " << differ << "/" << total << " (" << (differ/(float)total) * 100 << "%" << ")" << endl;

}

int main(int argc, char** argv) {
    string model = "";
    string predict = "";
    string text = "";
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "-m") == 0 && i+1 < argc) {
            i++;
            model = argv[i];
        } else if (strcmp(argv[i], "-p") == 0 && i+1 < argc) {
            i++;
            predict = argv[i];
        } else if (strcmp(argv[i], "-t") == 0 && i+1 < argc) {
            i++;
            text = argv[i];
        }
    }

    if (model == "") {
        cout << "Nebyl zadan soubor modelu. (-m model)" << endl;
        return 0;
    }
    if (predict == "" && text == "") {
        cout << "Musi byt zadan jeden z parametru -t nebo -p." << endl;
        return 0;
    } else {
        if (text != "") train(text, model);
        else test(predict, model);
    }

    return 0;
}
