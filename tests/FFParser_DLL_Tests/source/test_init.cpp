#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "include/FFParser_DLL.h"
#include <memory>
#include <functional>
#include <Windows.h>


using namespace FFParser;

//dll load
const wchar_t* dllname = L"FFParser_DLL.dll";
using LibGuard = std::unique_ptr<std::remove_pointer_t<HMODULE>, std::function<void(HMODULE)>>;
LibGuard dll_load(LoadLibrary(dllname), [](HMODULE dll) { if (dll) FreeLibrary(dll); });


using GetStorageFunc = IStorageFactory* (CALL *)();
GetStorageFunc dll_getstorage;

IStorageFactory* storage;
IRecordsStream* records;


// initialization function:
bool init_unit_test()
{
	if (dll_load == nullptr) {
		return false;
	}

	dll_getstorage = (GetStorageFunc) GetProcAddress(dll_load.get(), "GetStorage");

	if (!dll_getstorage) {
		return false;
	}

	storage = dll_getstorage();
	records = storage->createRecordsStream(ERecordTypes::HISTORY, 0);


	return true;
}



// entry point:
int main(int argc, char* argv[])
{
	return boost::unit_test::unit_test_main( &init_unit_test, argc, argv );
}