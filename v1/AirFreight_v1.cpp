// Christopher Schatz & Serhat Sural
// AIRFreight: Programm zum Überprüfen von Seriennummern bei Luftfracht

#define VERSION 1

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <conio.h>
#include <stdbool.h>
#include <windows.h>
#include <shellapi.h>

using namespace std;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

int numSN, SNbuf = 0;
string FILEin;

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

void input(numbers numbers[]) {
    while(!numSN) {
        system("cls");
        cout << "AIRFreight v" << VERSION << endl;
        cout << "Name der Datei: ";
        cin >> FILEin;
        FILEin += ".csv";

        fstream table;
        table.open(FILEin, ios::in);

        if(!table) {
            lineUp(0);
            cout << "Datei existiert nicht!\nBitte erneut eingeben";
            Sleep(2000);
        } else {
            while(!table.eof()) {
                getline(table, numbers[numSN].SN);
                numSN++;
            }
        }
        table.close();
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

    Package.close();
}

int main() {
    string SNin, pName;
    bool decrease = 0;
    numbers numbers[300];

    input(numbers);

    for(int i = 0; i < 4; i++) {
        FILEin.pop_back();
    }

    for(int i = 0; i < numSN; i++) {
        lineUp(1);
        cout << "\rSN von Gerät " << i+1 << "|" << SNbuf+1 << ": ";
        cin >> SNin;

        if(SNin == "p") {
          i--;
          writePackage(FILEin, numbers);

          lineUp(2);
          cout << "\r" << "Packet " << SNbuf << " in Datei " << FILEin << "[" << SNbuf << "].txt geschrieben" ;
          lineUp(1);

        } else {
            for(int j = 0; j < numSN; j++) {
                if(SNin == numbers[j].SN) {
                    if(numbers[j].scanned) {
                        lineUp(2);
                        cout << "\rSN wurde bereits gescanned";
                        lineUp(1);
                        decrease = 1;
                        break;
                    } else {
                        lineUp(2);
                        cout << "\rSN eingescanned";
                        lineUp(1);
                        decrease = 0;
                        numbers[j].scanned = 1;
                        break;
                    }
                } else {
                    lineUp(2);
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

    ShellExecute(NULL, "open", "https://barcode.tec-it.com/de/PDF417", NULL, NULL, SW_SHOWNORMAL);

    cin.ignore();
    system("cls");
    return 0;
}
