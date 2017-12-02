#pragma once

#include "IRecordsStream.h"


#define CALL __cdecl

namespace FFParser {

	class IDataExporter
	{
	public:
		virtual bool CALL exportRecords(IRecordsStream* rstream, const char* output_path, bool createMD5 = true) const = 0;
		virtual bool CALL exportCacheFile(IRecord* file_record, const char* output_path, bool createMD5 = true) const = 0;
		virtual void CALL exportCache(IRecordsStream* rstream, const char* output_path, size_t profile = 0, bool createMD5 = true) const = 0;
		virtual void CALL exportProfile(const char* output_path, size_t profile, bool export_cache = true, bool cacheMD5 = true, bool recordsMD5 = true) const = 0;
		virtual void CALL exportAll(const char* output_path, bool createMD5 = true) const = 0;

	protected:
		~IDataExporter() {}
	};

}