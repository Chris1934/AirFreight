// Christopher Schatz & Serhat Sural
// AIRFreight: Programm zum Überprüfen von Seriennummern bei Luftfracht
// Version 3 (GUI)

#define VERSION 3

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <dirent.h>
#include <windows.h>

#include <gtk/gtk.h>

using namespace std;

GtkWidget *window1;
GtkFileChooserButton *wlkmAddFile;
GtkButton *wlkmBtn;
GtkLabel *wlkmLabel;

GtkWidget *window2;
GtkEntry *mainEntry;
GtkButton *mainBtnBundleDevice;
GtkButton *mainBtnBundlePacket;
GtkHeaderBar *mainDualHeaderEntryDeviceNum;
GtkHeaderBar *mainDualHeaderEntryPacketNum;
GtkLabel *mainResponse;
GtkLabel *mainDeviceName;

GtkWidget *window3;
GtkWidget *last;
GtkWidget *lastHeaderInfo;
GtkWidget *lastHeaderChoice;
GtkWidget *lastChoice;
GtkWidget *lastChoiceContinue;
GtkWidget *lastChoiceDone;

GError *error = NULL;
GtkBuilder *builder;

struct numbers {
    string SN;
    bool scanned = 0;
    bool written = 0;
};

int numSN = 0, cntSN = 0, SNbuf = 1, fileSet;
string FILEin, fName, SNin, Info;
bool wasWritten = 1;

struct numbers numbers[300];

const char* read() {
    return gtk_entry_get_text(mainEntry);
}

void fTrim() {
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

void input() {
            fstream table;
            table.open(FILEin, ios::in);

            while(!table.eof()) {
                getline(table, numbers[numSN].SN);
                numbers[numSN].scanned = 0;
                numbers[numSN].written = 0;
                numSN++;
            }
            table.close();

            for(int i = 0; i < 4; i++) {
                FILEin.pop_back();
            }

            fTrim();
}

void writeCode(string cName) {
    static string zint;
    zint = "zint -b 55 -i ";
    zint += (char)34;
    zint += cName;
    zint += (char)34;

    static string picName;
    picName = (char)34;
    picName += cName;

    for(int i = 0; i < 4; i++) {
        picName.pop_back();
    }
    picName += ".png";
    picName += (char)34;

    zint += " -o "; zint += picName;

    WinExec(zint.c_str(), SW_HIDE);
}

void writePackage(string pName) {

    if(pName.find("[") != string::npos) {
        for(int i = 0; i < 3; i++) {
            pName.pop_back();
        }
    }

    pName.push_back('['); pName += to_string(SNbuf); pName.push_back(']');
    pName += ".txt";

    SNbuf++;

    ofstream Package(pName);

    for(int i = 0; i < numSN; i++) {
        if(numbers[i].scanned && !numbers[i].written) {
            numbers[i].written = 1;
            Package << numbers[i].SN << endl;
        }
    }
    wasWritten = 1;
    Package.close();

    writeCode(pName);
}

void Package() {
    if(!wasWritten) {
        Info = "Packet ";
        Info += to_string(SNbuf); Info += " in Datei\n"; Info += FILEin;
        Info += "["; Info += to_string(SNbuf); Info += "] geschrieben";

        writePackage(FILEin);
    } else {
        Info = "Keine neuen SN eingegeben, Dateien werden nicht geschrieben";
    }
    gtk_label_set_text(mainResponse, Info.c_str());
}

void fChosen(GtkWidget *widget, gpointer data) {
    if(fileSet) {
            input();
            gtk_widget_hide(window1);
            gtk_widget_show(window2);
      } else {
          gtk_label_set_markup(wlkmLabel, "<span color=\"red\">Keine gültige Datei erkannt</span>");
      }
}

void fSet(GtkWidget *widget, gpointer data) {
    string markup;

    FILEin = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(wlkmAddFile));

    fName = FILEin;
    fName.erase(0, fName.find_last_of((char)92)+1);

    if(fName.find(".csv") == string::npos) {
        fileSet = 0;
    } else {
        fileSet = 1;
        for(int i = 0; i < 4; i++) {
            fName.pop_back();
        }
    }

    markup = "<span color=\"black\">";
    markup += FILEin;
    markup += "</span>";

    gtk_label_set_markup(wlkmLabel, markup.c_str());
}

void checkSN(GtkWidget *widget, gpointer data) {

    SNin = read();
    gtk_entry_set_text(mainEntry, "");

        for(int j = 0; j < numSN; j++) {
            if(SNin == numbers[j].SN) {
                if(numbers[j].scanned) {
                    Info = "<span color=\"orange\">SN wurde bereits gescanned</span>";
                    break;
                } else {
                    Info = "<span color=\"green\">SN eingescanned</span>";
                    cntSN++;
                    wasWritten = 0;
                    numbers[j].scanned = 1;
                    break;
                }
            } else {
                Info = "<span color=\"red\">SN wurde nicht gefunden</span>";
            }
        }

        gtk_label_set_markup(mainResponse, Info.c_str());
        gtk_header_bar_set_title(mainDualHeaderEntryPacketNum, (to_string(SNbuf)).c_str());
        gtk_header_bar_set_title(mainDualHeaderEntryDeviceNum, (to_string(cntSN+1)).c_str());

        if(cntSN >= numSN) {
            Package();
            gtk_widget_hide(window1);
            gtk_widget_hide(window2);
            gtk_widget_show(window3);
        }
}

void printLabel() {
    static string NameMessage;
    NameMessage = "Gerätetyp: ";
    NameMessage += fName;

    gtk_label_set_text(mainDeviceName, NameMessage.c_str());
    gtk_label_set_text(mainResponse, "Bereit zum Scannen");
    gtk_header_bar_set_title(mainDualHeaderEntryPacketNum, (to_string(SNbuf)).c_str());
    gtk_header_bar_set_title(mainDualHeaderEntryDeviceNum, (to_string(cntSN+1)).c_str());
}

void startOver() {
    gtk_file_chooser_unselect_all(GTK_FILE_CHOOSER(wlkmAddFile));
    gtk_label_set_markup(wlkmLabel, "<span color=\"black\">Bitte Datei auswählen</span>");
    numSN = 0;
    SNbuf = 1;
    cntSN = 0;
    fileSet = 0;
    gtk_widget_hide(window3);
    gtk_widget_hide(window2);
    gtk_widget_show(window1);
}

int main(int argc, char *argv[]) {
    string SNin, pName;

    builder = gtk_builder_new();

    gtk_init(&argc, &argv);
    gtk_builder_add_from_file(builder, "GUI.glade", &error);

    window1 = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    wlkmAddFile = GTK_FILE_CHOOSER_BUTTON(gtk_builder_get_object(builder, "wlkmAddFile"));
    wlkmBtn = GTK_BUTTON(gtk_builder_get_object(builder, "wlkmBtn"));
    wlkmLabel = GTK_LABEL(gtk_builder_get_object(builder, "wlkmLabel"));

    window2 = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));
    mainEntry = GTK_ENTRY(gtk_builder_get_object(builder, "mainEntry"));
    mainBtnBundleDevice = GTK_BUTTON(gtk_builder_get_object(builder, "mainBtnBundleDevice"));
    mainBtnBundlePacket = GTK_BUTTON(gtk_builder_get_object(builder, "mainBtnBundlePacket"));
    mainDualHeaderEntryDeviceNum = GTK_HEADER_BAR(gtk_builder_get_object(builder, "mainDualHeaderEntryDeviceNum"));
    mainDualHeaderEntryPacketNum = GTK_HEADER_BAR(gtk_builder_get_object(builder, "mainDualHeaderEntryPacketNum"));
    mainResponse = GTK_LABEL(gtk_builder_get_object(builder, "mainResponse"));
    mainDeviceName = GTK_LABEL(gtk_builder_get_object(builder, "mainDeviceName"));

    window3 = GTK_WIDGET(gtk_builder_get_object(builder, "window3"));
    lastChoiceContinue = GTK_WIDGET(gtk_builder_get_object(builder, "lastChoiceContinue"));
    lastChoiceDone = GTK_WIDGET(gtk_builder_get_object(builder, "lastChoiceDone"));

    g_signal_connect(window1, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(wlkmAddFile, "file-set", G_CALLBACK(fSet), NULL);
    g_signal_connect(wlkmBtn, "clicked", G_CALLBACK(fChosen), NULL);

    g_signal_connect(window2, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window2, "show", G_CALLBACK(printLabel), NULL);
    g_signal_connect(mainBtnBundleDevice, "clicked", G_CALLBACK(checkSN), NULL);
    g_signal_connect(mainEntry, "activate", G_CALLBACK(checkSN), NULL);
    g_signal_connect(mainBtnBundlePacket, "clicked", G_CALLBACK(Package), NULL);

    g_signal_connect(window3, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(lastChoiceDone, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(lastChoiceContinue, "clicked", G_CALLBACK(startOver), NULL);

    gtk_widget_show(window1);
    gtk_label_set_text(wlkmLabel, "Bitte Datei auswählen");

    gtk_main();

    return 0;
}
