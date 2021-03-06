#pragma once

#include "IRecordsStream.h"
#include "ERecordTypes.h"
#include "IDataExporter.h"

#define CALL __cdecl

namespace FFParser {

	class IStorageFactory
	{
	public:
		virtual size_t CALL getNumberOfProfiles() const = 0;
		virtual const char* CALL getProfileName(size_t profile = 0) const = 0;
		virtual const char* CALL getPathToProfile(size_t profile = 0) const = 0;
		virtual const char* CALL getPathToCache(size_t profile = 0) const = 0;
		virtual IRecordsStream* CALL createRecordsStream(ERecordTypes type, size_t profile = 0) = 0;
		virtual IDataExporter* CALL getDataExporter() const = 0;

	protected:
		~IStorageFactory() {}
	};

}