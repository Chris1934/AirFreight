// Christopher Schatz & Serhat Sural
// AIRFreight: Programm zum Überprüfen von Seriennummern bei Luftfracht
// Version 2

#define VERSION 2

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

int numSN, SNbuf = 0;
string FILEin;
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

void showDir() {
    DIR *dr;
    string dir;

    struct dirent *en;
    dr = opendir(".");

    lineUp(2);
    cout << "Dateien: ";

    if(dr) {
        while((en = readdir(dr)) != NULL) {
            dir = en->d_name;
            if(dir.find(".csv") != string::npos) {
                for(int i = 0; i < 4; i++) {
                    dir.pop_back();
                }
                cout << dir << "  ";
            }
        }
    closedir(dr);
    lineUp(0);
    }
}

void input(numbers numbers[]) {
    while(!numSN) {
        system("cls");
        showDir();
        cout << "AIRFreight v" << VERSION << endl;
        cout << "Name der Datei: ";
        cin >> FILEin;
        FILEin += ".csv";

        fstream table;
        table.open(FILEin, ios::in);

        if(!table) {
            lineUp(0);
            cout << "Datei existiert nicht!\nBitte erneut eingeben";
            clrln(2);
            Sleep(2000);
        } else {
            numSN = 0;
            while(!table.eof()) {
                getline(table, numbers[numSN].SN);
                numbers[numSN].scanned = 0;
                numbers[numSN].written = 0;
                numSN++;
            }
        }
        table.close();
        clrln(2);
        lineUp(0);
        for(int i = 0; i < 4; i++) {
            FILEin.pop_back();
        }
    }
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
            Package << numbers[i].SN << (char)92 << "r" << (char)92 << "n" << endl;
        }
    }
    wasWritten = 1;
    Package.close();
}

int main() {
    string SNin, pName;
    bool decrease = 0, scan = 1;
    numbers numbers[300];

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

                  cout << "\r" << "Packet " << SNbuf << " in Datei " << FILEin << "[" << SNbuf << "].txt geschrieben" ;
                } else {
                    cout << "Keine neuen SN eingegeben, Datei wird nicht geschrieben";
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
        cout << "Weitere Geräte einscannen? ";
        cin >> FILEin;

        if(FILEin == "j") {
            scan = 1;
            numSN = 0;
            SNbuf = 0;
        } else {
            scan = 0;
        }
    }

    ShellExecute(NULL, "open", "https://barcode.tec-it.com/de/PDF417", NULL, NULL, SW_SHOWNORMAL);

    system("cls");
    return 0;
}
