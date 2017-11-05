#pragma once

#include <vector>

#include "include/IRecordsStream.h"
#include "include/ERecordTypes.h"

#include "RecordImpl.h"
#include "../helpers/HelperFacade.h"



namespace FFParser {

	class RecordsStreamImpl : public IRecordsStream
	{
	protected:
		HelperFacade& _helper_ref;

		size_t _profile;
		ERecordTypes _stream_type;
		std::vector<RecordImpl> _records;
		size_t _current_record;

		std::vector<std::string> _field_names;		// { "id", "url", "path", ... };

	public:
		//ctors and dtor
		RecordsStreamImpl(size_t profile, ERecordTypes type);
		virtual ~RecordsStreamImpl() = default;

		//Interface methods
		virtual size_t getTotalRecords(size_t profile) const override;
		virtual size_t loadRecords(size_t number) override;
		virtual size_t loadRecords(size_t from, size_t number) override;
		virtual size_t getNumberOfRecords() const override;
		virtual size_t currentRecord() const override;
		virtual IRecord* getPrevRecord() override;
		virtual IRecord* getNextRecord() override;
		virtual IRecord* getRecordByIndex(size_t index) override;
		virtual size_t getNumberOfFields() const override;
		virtual const char* getFieldName(size_t index) const override;
	};

}