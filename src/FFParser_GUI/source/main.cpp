#include "mainwindow.h"
#include <QApplication>
#include <QDebug>


//for DLL
#include <memory>
#include <functional>
#include <windows.h>
#include "../include/FFParser_DLL.h"

using namespace FFParser;


const wchar_t* dllname = L"FFParser_DLL.dll";

using GetStorageFunc = IStorageFactory* (CALL *)();




int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();


    //dll load
    HINSTANCE dll_load = LoadLibrary(dllname);

    if (!dll_load) {
        exit(1);
    }

    GetStorageFunc dll_getstorage = (GetStorageFunc) GetProcAddress(dll_load, "GetStorage");

    IStorageFactory* DLLStorage = dll_getstorage();

    //get history
    IRecordsStream* history = DLLStorage->createRecordsStream(ERecordTypes::HISTORY, 0);


    // TEST getRecordByIndex - OK
    qDebug() << "getRecordByIndex:\n";
    IRecord* onerec = history->getRecordByIndex(0);
    if (onerec != nullptr) {
        qDebug()
            << "\t" << onerec->getFieldValue(0) << " "
            << "\t" << onerec->getFieldValue(1) << " "
            << "\t" << onerec->getFieldValue(2) << " "
            << "\t" << onerec->getFieldValue(3) << "\n";
    }

    //TEST getNext - OK
    qDebug() << "getNextRecord:\n";
    for (int i = 0; i < 5; ++i) {
        onerec = history->getNextRecord();
        if (onerec != nullptr) {
            qDebug()
                   << "\t" << onerec->getFieldValue(0) << " "
                   << "\t" << onerec->getFieldValue(1) << " "
                   << "\t" << onerec->getFieldValue(2) << " "
                   << "\t" << onerec->getFieldValue(3) << "\n";
        }
    }



    return a.exec();
}
