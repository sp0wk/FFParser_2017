#pragma once

#ifdef _BUILD_DLL_
#define DLLAPI __declspec(dllexport)
#define EXP_TEMPLATE
#else
#define DLLAPI __declspec(dllimport)
#define EXP_TEMPLATE extern
#endif

#define CALL __cdecl


#include "IStorageFactory.h"



// DLL API

namespace FFParser {
	
	extern "C" DLLAPI IStorageFactory* CALL GetStorage();

}