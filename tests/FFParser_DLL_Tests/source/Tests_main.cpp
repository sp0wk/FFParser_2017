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
	
	// TEST getPathToProfile
	std::cout << FactoryStorage->getPathToProfile(0) << "\n\n";

	//get history
	IRecordsStream* recstr = FactoryStorage->createRecordsStream(ERecordTypes::LOGINS, 0);
	
	size_t from = 100;
	size_t number = 2;
	size_t cnt = recstr->loadRecords(0, number);
	size_t cnt2 = recstr->loadNextRecords(number);
	size_t sz = recstr->getNumberOfRecords();
	size_t total = recstr->getTotalRecords();


	// TEST getRecordByIndex
	IRecord* onerec = recstr->getRecordByIndex(0);
	if (onerec != nullptr) {
		std::cout 
			<< onerec->getFieldValue(0) << " "
			<< onerec->getFieldValue(1) << " "
			<< onerec->getFieldValue(2) << " "
			<< onerec->getFieldValue(3) << " "
			<< onerec->getFieldValue(4) << " "
			<< onerec->getFieldValue(5) << " "
			<< onerec->getFieldValue(6) << " "
			<< onerec->getFieldValue(7) << " "
			<< onerec->getFieldValue(8) << " "
			<< "\n\n";
	}
	
	//TEST getNextRecord
	for (size_t i = 1; i < sz; ++i) {
		onerec = recstr->getNextRecord();
		if (onerec != nullptr) {
			std::cout 
				<< onerec->getFieldValue(0) << " "
				<< onerec->getFieldValue(1) << " "
				<< onerec->getFieldValue(2) << " "
				<< onerec->getFieldValue(3) << " "
				<< onerec->getFieldValue(4) << " "
				<< onerec->getFieldValue(5) << " "
				<< onerec->getFieldValue(6) << " "
				<< onerec->getFieldValue(7) << " "
				<< onerec->getFieldValue(8) << " "
				<< "\n\n";
		}
	}
	
	system("pause");

	return 0;
}