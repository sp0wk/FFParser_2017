#include <iostream>
#include <windows.h>
#include <memory>
#include <functional>

#include "include/FFParser_DLL.h"

#include <boost/test/unit_test.hpp>

using namespace FFParser;



const wchar_t* dllname = L"FFParser_DLL.dll";

using GetStorageFunc = IStorageFactory* (CALL *)();
using SetErrorCallbackFunc = void (CALL *)(void (*callback) (const char* error_text, const char* error_title));


void custom_callback(const char* text, const char* title)
{
	::MessageBoxA(NULL, text, title, MB_OK | MB_ICONERROR);
	/*std::cout << "ERROR!!!:\n" <<
				 "Title - " << title << "\n" <<
				 "Text - " << text << "\n\n";*/
}


int main()
{
	//dll load
	using LibGuard = std::unique_ptr<std::remove_pointer_t<HMODULE>, std::function<void(HMODULE)>>;
	LibGuard dll_load(LoadLibrary(dllname), [](HMODULE dll) { if (dll) FreeLibrary(dll); });

	if (!dll_load.get()) {
		exit(1);
	}

	GetStorageFunc dll_getstorage = (GetStorageFunc) GetProcAddress(dll_load.get(), "GetStorage");
	SetErrorCallbackFunc dll_setErrorCB = (SetErrorCallbackFunc) GetProcAddress(dll_load.get(), "SetErrorCallback");

	//set error callback test
	dll_setErrorCB(custom_callback);

	IStorageFactory* FactoryStorage = dll_getstorage();
	
	// TEST getPathToProfile
	std::cout << FactoryStorage->getPathToProfile(0) << "\n\n";

	//get records
	IRecordsStream* recstr = FactoryStorage->createRecordsStream(ERecordTypes::HISTORY, 0);

	size_t from = 0;
	size_t number = 25;
	size_t cnt = recstr->loadRecords(from, number);
	size_t cnt2 = recstr->loadNextRecords(number);
	size_t sz = recstr->getNumberOfRecords();
	size_t total = recstr->getTotalRecords();
	
	//TEST export
	IDataExporter* exp = FactoryStorage->getDataExporter();
	exp->exportRecords(recstr, "D:\\exported.json", true);

	//TEST search
	size_t cur = recstr->currentRecord();
	size_t found = recstr->searchNextRecord("9gAg");
	size_t found2 = recstr->searchNextRecord("9gAg");
	size_t found3 = recstr->searchNextRecord("9gAg");
	size_t foundprev = recstr->searchPrevRecord("9gAg");
	size_t foundprev2 = recstr->searchPrevRecord("9gAg");
	IRecord* foundrec = recstr->getRecordByIndex(found);

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