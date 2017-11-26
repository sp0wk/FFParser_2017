#define _BUILD_DLL_


#include "include/FFParser_DLL.h"
#include "storage/StorageFactoryImpl.h"
#include "ErrorHandler.h"


namespace FFParser {

	DLLAPI IStorageFactory* CALL GetStorage()
	{
		return &StorageFactoryImpl::getInstance();
	}

	DLLAPI void CALL SetErrorCallback(void (*error_callback) (const char* error_text, const char* error_title))
	{
		ErrorHandler::getInstance().setErrorCallback(error_callback);
	}
}