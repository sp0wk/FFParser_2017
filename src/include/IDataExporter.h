#pragma once

#include "IRecordsStream.h"


#define CALL __cdecl

namespace FFParser {

	class IDataExporter
	{
	public:
		virtual bool CALL exportRecords(IRecordsStream* rstream, const char* output_path, bool createMD5 = true) const = 0;
		virtual bool CALL exportCacheFile(IRecord* file_record, const char* output_path) const = 0;
		//TODO:
		//virtual bool CALL exportAllRecords(const char* path, size_t profile) const = 0;
		//virtual bool CALL exportAllCache(IRecordsStream* rstream, const char* path, size_t profile) const = 0;
		//virtual bool CALL exportEverything(const char* path) const = 0;

	protected:
		~IDataExporter() {}
	};

}