#pragma once

#include "IRecordsStream.h"
#include "ERecordTypes.h"

#define CALL __cdecl

namespace FFParser {

	class IStorageFactory
	{
	public:
		virtual size_t CALL getNumberOfProfiles() const = 0;
		virtual const char* CALL getProfileNameByIndex(size_t index) const = 0;
		virtual IRecordsStream* CALL createRecordsStream(ERecordTypes type, size_t profile = 0) = 0;
		virtual void CALL freeRecordsStream(IRecordsStream* &record) = 0;
	};

}