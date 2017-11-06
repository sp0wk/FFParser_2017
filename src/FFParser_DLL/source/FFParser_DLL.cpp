#define _BUILD_DLL_


#include "include/FFParser_DLL.h"
#include "storage/StorageFactoryImpl.h"


namespace FFParser {

	DLLAPI IStorageFactory* CALL GetStorage()
	{
		return &StorageFactoryImpl::getInstance();
	}

}