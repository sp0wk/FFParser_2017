#pragma once

#include "IRecordsStream.h"
#include "ERecordTypes.h"


namespace FFParser {

	class IStorageFactory
	{
	public:
		virtual size_t getNumberOfProfiles() const = 0;
		virtual const char* getProfileNameByIndex(size_t index) const = 0;
		virtual IRecordsStream* createRecordsStream(ERecordTypes type, size_t profile = 0) = 0;
		virtual void freeRecordsStream(IRecordsStream* &record) = 0;
	};

}