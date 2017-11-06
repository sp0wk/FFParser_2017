#pragma once

#include "IRecord.h"

#define CALL __cdecl

namespace FFParser {

	class IRecordsStream
	{
	public:
		//load
		virtual size_t CALL getTotalRecords(size_t profile = 0) const = 0;
		virtual size_t CALL loadNextRecords(size_t number = 0) = 0;
		virtual size_t CALL loadRecords(size_t from, size_t number) = 0;
		//get record
		virtual size_t CALL getNumberOfRecords() const = 0;
		virtual size_t CALL currentRecord() const = 0;
		virtual IRecord* CALL getPrevRecord() = 0;
		virtual IRecord* CALL getNextRecord() = 0;
		virtual IRecord* CALL getRecordByIndex(size_t index) = 0;
		//fields
		virtual size_t CALL getNumberOfFields() const = 0;
		virtual const char* CALL getFieldName(size_t index) const = 0;
	};

}