#pragma once

#include "include/IRecordsStream.h"


#define CALL __cdecl

namespace FFParser {

	class IDataExporter
	{
	public:
		virtual bool CALL exportRecords(IRecordsStream* rstream, const char* filename, bool createMD5 = true) const = 0;
		//TODO:
		//virtual bool CALL exportCacheFile(IRecordsStream* rstream, size_t recnum, const char* path) const = 0;
		//virtual bool CALL exportAllRecords(const char* path, size_t profile) const = 0;
		//virtual bool CALL exportAllCache(IRecordsStream* rstream, const char* path, size_t profile) const = 0;
		//virtual bool CALL exportEverything(const char* path) const = 0;

	protected:
		~IDataExporter() {}
	};

}