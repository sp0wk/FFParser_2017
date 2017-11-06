#include "mainwindow.h"
#include <QApplication>
#include <QDebug>


//for DLL
#include <windows.h>
#include "../include/FFParser_DLL.h"

using namespace FFParser;

typedef IStorageFactory* (CALL *GetStorageFunc)();

//global access to storage
IStorageFactory* DLLStorage;


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    //dll load
    const wchar_t* dllname = L"FFParser_DLL.dll";

    HINSTANCE dll_load = LoadLibrary(dllname);

    if (!dll_load) {
        exit(1);
    }

    GetStorageFunc dll_getstorage = (GetStorageFunc) GetProcAddress(dll_load, "GetStorage");

    DLLStorage = dll_getstorage();

    //get history example
    IRecordsStream* recstr = DLLStorage->createRecordsStream(ERecordTypes::HISTORY, 0);

    size_t from = 100;
	size_t number = 2;
    size_t cnt = recstr->loadRecords(from, number);
    size_t cnt2 = recstr->loadNextRecords(number);
	size_t sz = recstr->getNumberOfRecords();

    // TEST getRecordByIndex - OK
    qDebug() << "getRecordByIndex:\n";
    IRecord* onerec = recstr->getRecordByIndex(0);
    if (onerec != nullptr) {
        qDebug()
            << onerec->getFieldValue(0) << " "
            << onerec->getFieldValue(1) << " "
            << onerec->getFieldValue(2) << " "
            //<< onerec->getFieldValue(3) << " "
            << "\n";
    }

    //TEST getNext - OK
    qDebug() << "getNextRecord:\n";
    for (size_t i = 1; i < sz; ++i) {
        onerec = recstr->getNextRecord();
        if (onerec != nullptr) {
            qDebug()
                   << onerec->getFieldValue(0) << " "
                   << onerec->getFieldValue(1) << " "
                   << onerec->getFieldValue(2) << " "
                   //<< onerec->getFieldValue(3) << " "
                   << "\n";
        }
    }


    return a.exec();
}
