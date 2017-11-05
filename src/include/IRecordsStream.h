#pragma once

#include "IRecord.h"


namespace FFParser {

	class IRecordsStream
	{
	public:
		//load
		virtual size_t getTotalRecords(size_t profile = 0) const = 0;
		virtual size_t loadRecords(size_t number = 0) = 0;
		virtual size_t loadRecords(size_t from, size_t number) = 0;
		//get record
		virtual size_t getNumberOfRecords() const = 0;
		virtual size_t currentRecord() const = 0;
		virtual IRecord* getPrevRecord() = 0;
		virtual IRecord* getNextRecord() = 0;
		virtual IRecord* getRecordByIndex(size_t index) = 0;
		//fields
		virtual size_t getNumberOfFields() const = 0;
		virtual const char* getFieldName(size_t index) const = 0;
	};

}