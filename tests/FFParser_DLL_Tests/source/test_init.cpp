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

using SetErrorCallbackFunc = void (CALL *)(void (*callback) (const char* error_text, const char* error_title));
void empty_callback(const char* text, const char* title) {}


//record for IRecord_Test
std::unique_ptr<IRecordsStream> one_rec;


// initialization function:
bool init_unit_test()
{
	if (dll_load == nullptr) {
		return false;
	}

	dll_getstorage = (GetStorageFunc) GetProcAddress(dll_load.get(), "GetStorage");
	SetErrorCallbackFunc dll_setErrorCB = (SetErrorCallbackFunc) GetProcAddress(dll_load.get(), "SetErrorCallback");

	if (!dll_getstorage) {
		return false;
	}

	//set empty error callback for testing
	dll_setErrorCB(empty_callback);

	storage = dll_getstorage();

	one_rec.reset( storage->createRecordsStream(ERecordTypes::HISTORY, 0) );


	return true;
}



// entry point:
int main(int argc, char* argv[])
{
	return boost::unit_test::unit_test_main( &init_unit_test, argc, argv );
}