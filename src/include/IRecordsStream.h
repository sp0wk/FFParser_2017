#pragma once

#include "IRecord.h"
#include "ERecordTypes.h"

#define CALL __cdecl

namespace FFParser {

	class IRecordsStream
	{
	public:
		//load
		virtual size_t CALL getTotalRecords() const = 0;
		virtual size_t CALL loadNextRecords(size_t number = -1) = 0;
		virtual size_t CALL loadRecords(size_t from, size_t number) = 0;
		//getters
		virtual size_t CALL getNumberOfRecords() const = 0;
		virtual size_t CALL currentRecord() const = 0;
		virtual IRecord* CALL getPrevRecord() = 0;
		virtual IRecord* CALL getNextRecord() = 0;
		virtual IRecord* CALL getRecordByIndex(size_t index) = 0;
		virtual size_t CALL getStreamProfile() const = 0;
		virtual ERecordTypes CALL getStreamType() const = 0;
		//setters
		virtual bool CALL setCurrentRecord(size_t index) = 0;
		//search
		virtual size_t CALL searchPrevRecord(const char* text) = 0;
		virtual size_t CALL searchNextRecord(const char* text) = 0;
		//fields
		virtual size_t CALL getNumberOfFields() const = 0;
		virtual const char* CALL getFieldName(size_t index) const = 0;

	protected:
		~IRecordsStream() {}
	};

}