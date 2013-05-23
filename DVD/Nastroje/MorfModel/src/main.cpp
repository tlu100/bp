#include <iostream>
#include <fstream>
#include <cstring>
#include <string>

using namespace std;

int main(int argc, char** argv) {

    // vstupni soubor s daty z ParZu
    ifstream in;
    // vystupni soubory
    ofstream out1, out2, out3, out4, out5, out6, out7, out8, out9;

    if (argc > 1 && strcmp(argv[1], "")) {
        // otevreni souboru v adresari z prvniho parametru
        string dir(argv[1]);
        in.open((dir + "/rsd+morf").c_str());
        out1.open((dir + "/rsd").c_str());
        out2.open((dir + "/rsd+rod").c_str());
        out3.open((dir + "/rsd+pad").c_str());
        out4.open((dir + "/rsd+cislo").c_str());
        out5.open((dir + "/rod").c_str());
        out6.open((dir + "/pad").c_str());
        out7.open((dir + "/cislo").c_str());
        out8.open((dir + "/rodstejny").c_str());
        out9.open((dir + "/osoba+cislo").c_str());
    } else {
        cerr << "Nebyl zadan vystupni adresar!" << endl;
        return 0;
    }

    // nacitany retezec
    string s = "";
    // aktualni slovni druh
    string rsd = "";
    // rod predchoziho slova
    string rod = "";
    // indikuje, zda byl v aktualnim slove rod
    bool bylrod = false;

    while (in.peek() != -1) {
        // nacteme znak ze vstupniho souboru
        char c = in.get();
        switch(c) {
        // pokud byla dvojtecka, pak konci slovni druh a nasleduje morfologicka analyza
        case ':':
            // pokud v predchozim slove nebyl rod, vynulujeme promennou rod a bylrod
            if (!bylrod) rod = "";
            else bylrod = false;
            // vypiseme slovni druh nebo w
            out1 << s;
            out2 << s;
            out3 << s;
            out4 << s;
            out5 << 'w';
            out6 << 'w';
            out7 << 'w';
            out8 << 'w';
            out9 << 'w';
            // nastavime aktualni slovni druh
            rsd = s;
            // vynulujeme nacteny retezec
            s = "";
            break;
        case ' ':
        case '|':
        case '\n':
            // nektere slovni druhy preskocime, protoze se u nich objevuje spatne urceni nebo je nezadouci
            // stanoveno podle vyhodnoceni na 50 tis. vetach - viz stss-prehled.pdf
            if (rsd != "CARD" && rsd != "FM" && rsd != "KON" && rsd != "KOUS"
                && rsd != "KOKOM" && rsd != "ADV" && rsd != "TRUNC") {
                // pokud jsme narazili na rod, vypiseme ho do patricnych modelu
                if (s == "Fem" || s == "Masc" || s == "Neut") {
                    out2 << ':' << s;
                    out5 << s;
                    // pokud predchozi slovo nemelo rod nebo se neshoduje s aktualnim, vypiseme "rod", jinak "stejny"
                    if (rod == "" || rod != s) {
                        out8 << "rod";
                        rod = s;
                    } else {
                        out8 << "stejny";
                    }
                    // nastavime, ze byl rod
                    bylrod = true;
                }
                // pokud byl pad, vypiseme ho do patricnych souboru
                if (s == "Nom" || s == "Gen" || s == "Dat" || s == "Akk") {
                    out3 << ':' << s;
                    out6 << s;
                }
                // stejne tak cisla
                if (s == "Sg" || s == "Pl") {
                    out4 << ':' << s;
                    out7 << s;
                    out9 << s;
                }
                // u prvniho modelu jenom s rozsirenym slovnim druhem jeste budeme rozlisovat clen urcity/neurcity
                if (s == "Def" || s == "Indef") {
                    out1 << s;
                }
                // vypsani osoby
                if (s == "1" || s == "2" || s == "3") {
                    out9 << s;
                }
            }
            // oddeleni slov mezerou nebo konec vety odradkovanim
            if (c == ' ' || c == '\n') {
                    out1 << c;
                    out2 << c;
                    out3 << c;
                    out4 << c;
                    out5 << c;
                    out6 << c;
                    out7 << c;
                    out8 << c;
                    out9 << c;
            }
            // vynulovani aktualniho retezce
            s = "";
            break;
        default:
            // pricteme aktualne nacteny znak
            s += c;
            break;
        }
    }

    return 0;
}


