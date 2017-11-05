#include <iostream>
#include <windows.h>

#include "include/FFParser_DLL.h"

#include <boost/test/unit_test.hpp>


using namespace FFParser;



const wchar_t* dllname = L"FFParser_DLL.dll";

using GetStorageFunc = IStorageFactory* (CALL *)();



int main()
{
	//dll load
	HINSTANCE dll_load = LoadLibrary(dllname);

	if (!dll_load) {
		exit(1);
	}

	GetStorageFunc dll_getstorage = (GetStorageFunc) GetProcAddress(dll_load, "GetStorage");

	IStorageFactory* FactoryStorage = dll_getstorage();
	
	//get history
	IRecordsStream* history = FactoryStorage->createRecordsStream(ERecordTypes::LOGINS, 0);
	history->loadRecords(1);
	

	// TEST getRecordByIndex
	IRecord* onerec = history->getRecordByIndex(0);
	if (onerec != nullptr) {
		onerec->getFieldValue(0);
		onerec->getFieldValue(1);
		onerec->getFieldValue(2);
	}
	
	//TEST getNextRecord
	for (int i = 0; i < 5; ++i) {
		onerec = history->getNextRecord();
		if (onerec != nullptr) {
			onerec->getFieldValue(0);
			onerec->getFieldValue(1);
			onerec->getFieldValue(2);
		}
	}
	
	FactoryStorage->freeRecordsStream(history);

	return 0;
}