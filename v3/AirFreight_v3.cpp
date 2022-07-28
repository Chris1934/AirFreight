// Chris1934
// AIRFreight: Program to check air freight
// Version 3

#define VERSION 3

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdlib.h>
#include <conio.h>
#include <dirent.h>
#include <windows.h>
#include <shellapi.h>

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

unsigned int numSN, SNbuf = 0;
string FILEin, file[50], zint = "zint -b 55 -i ";
bool wasWritten = 1;

typedef struct {
    string SN;
    bool scanned = 0;
    bool written = 0;
} numbers;

void lineUp(int y) {
    CursorPosition.X = 0;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

void clrln(int ln1) {
    lineUp(ln1);
    cout << "\33[2K\r";
}

int showDir() {
    DIR *dr;
    string dir;
    int i = 0;

    struct dirent *en;
    dr = opendir(".");

    lineUp(2);
    cout << "Dateien: ";

    if(dr) {
        while((en = readdir(dr)) != NULL) {

            if((i+1%100) >= 10) {
                dir = to_string(i+1%100);
                dir.insert(2, "[");
            } else {
                dir = to_string(i+1);
                dir.insert(1, "[");
            }

            dir += en->d_name;

            if(dir.find(".csv") != string::npos) {
                file[i] = en->d_name;
                for(int i = 0; i < 4; i++) {
                    dir.pop_back();
                }
                cout << dir << "] ";
                i++;
            }
        }
    closedir(dr);
    lineUp(0);
    }
    return i;
}

void fTrim(numbers numbers[]) {
    for(int i = 0; i < numSN; i++) {
        if(numbers[i].SN == "") {
            for(int j = 0; j < (numSN - i); j++) {
                numbers[i+j].SN = numbers[i+j+1].SN;
            }
            i--;
        numSN--;
        }
    }
}

void input(numbers numbers[]) {
    static int fID;
    while(!numSN) {
        static int fCNT = showDir();

        cout << "AIRFreight v" << VERSION << endl;
        cout << "Name der Datei: ";
        cin >> fID;

        if((fID <= fCNT) && (fID > 0)) {
            FILEin = file[fID-1];

            fstream table;
            table.open(FILEin, ios::in);

            while(!table.eof()) {
                getline(table, numbers[numSN].SN);
                numbers[numSN].scanned = 0;
                numbers[numSN].written = 0;
                numSN++;
            }
            table.close();

            fTrim(numbers);

            clrln(3);
            lineUp(0);
            for(int i = 0; i < 4; i++) {
                FILEin.pop_back();
            }

        } else {
            lineUp(3);
            cout << "\rDatei existiert nicht! Bitte erneut eingeben";
            numSN = 0;
            lineUp(0);
        }
    }
}

void writeCode(string cName) {
    zint += cName;
    static string picName = cName;

    for(int i = 0; i < 4; i++) {
        picName.pop_back();
    }
    picName += ".png";

    zint += " -o "; zint += picName;

    char *in = new char[zint.size() + 1];
    copy(zint.begin(), zint.end(), in);
    in[zint.size()] = '\0';

    system(in);
}

void writePackage(string pName, numbers numbers[]) {
    if(pName.find("[") != string::npos) {
        for(int i = 0; i < 3; i++) {
            pName.pop_back();
        }
    }
    pName.push_back('['); pName += to_string(SNbuf+1); pName.push_back(']');
    pName.push_back('.'); pName.push_back('t'); pName.push_back('x'); pName.push_back('t');

    SNbuf++;

    ofstream Package(pName);

    for(int i = 0; i < numSN; i++) {
        if(numbers[i].scanned && !numbers[i].written) {
            numbers[i].written = 1;
            Package << numbers[i].SN << endl << endl;
        }
    }
    wasWritten = 1;
    Package.close();
    writeCode(pName);
}

int main() {
    string SNin, pName;
    bool decrease = 0, scan = 1;
    numbers numbers[300];

    system("cls");

    while(scan) {
        input(numbers);

        for(int i = 0; i < numSN; i++) {
            clrln(2);
            cout << "\rName: " << FILEin << setw(20) << "Paket: " << SNbuf+1;
            clrln(1);
            cout << "\rSN von Gerät " << i+1 << ": ";
            cin >> SNin;

            if(SNin == "p") {
                i--;
                clrln(3);
                if(!wasWritten) {
                  writePackage(FILEin, numbers);

                  cout << "\r" << "Packet " << SNbuf << " in Code " << FILEin << "[" << SNbuf << "].png geschrieben" ;
                } else {
                    cout << "Keine neuen SN eingegeben, Dateien werden nicht geschrieben";
                }
                lineUp(1);

            } else {
                for(int j = 0; j < numSN; j++) {
                    clrln(3);
                    if(SNin == numbers[j].SN) {
                        if(numbers[j].scanned) {
                            cout << "\rSN wurde bereits gescanned";
                            decrease = 1;
                            break;
                        } else {
                            cout << "\rSN eingescanned";
                            decrease = 0;
                            wasWritten = 0;
                            numbers[j].scanned = 1;
                            break;
                        }
                    } else {
                        cout << "\rSN wurde nicht gefunden";
                        decrease = 1;
                    }
                }
                if(decrease) {
                    i--;
                    decrease = 0;
                }
            }
        }
        writePackage(FILEin, numbers);

        system("cls");
        cout << "Weitere Geräte einscannen [j/n]? ";
        cin >> FILEin;

        if(FILEin == "j") {
            scan = 1;
            numSN = 0;
            SNbuf = 0;
        } else {
            scan = 0;
        }
    }

    system("cls");
    return 0;
}
