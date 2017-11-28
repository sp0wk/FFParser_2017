#pragma once

#include <vector>

#include "include/IRecordsStream.h"

#include "RecordImpl.h"
#include "helpers/HelperFacade.h"



namespace FFParser {

	class RecordsStreamImpl : public IRecordsStream
	{
	public:
		//ctors and dtor
		RecordsStreamImpl(size_t profile, ERecordTypes type);
		virtual ~RecordsStreamImpl() = default;

		//Interface methods
		virtual size_t CALL getTotalRecords() const override;
		virtual size_t CALL loadNextRecords(size_t number) override;
		virtual size_t CALL loadRecords(size_t from, size_t number) override;
		virtual size_t CALL getNumberOfRecords() const override;
		virtual size_t CALL currentRecord() const override;
		virtual IRecord* CALL getPrevRecord() override;
		virtual IRecord* CALL getNextRecord() override;
		virtual IRecord* CALL getRecordByIndex(size_t index) override;
		virtual bool CALL setCurrentRecord(size_t index) override;
		virtual size_t CALL getStreamProfile() const override;
		virtual ERecordTypes CALL getStreamType() const override;
		virtual size_t CALL searchPrevRecord(const char* text) override;
		virtual size_t CALL searchNextRecord(const char* text) override;
		virtual size_t CALL getNumberOfFields() const override;
		virtual const char* CALL getFieldName(size_t index) const override;

	protected:
		HelperFacade& _helper_ref;

		size_t _profile;
		ERecordTypes _stream_type;
		std::vector<RecordImpl> _records;
		size_t _current_record;
		size_t _last_from;			//for next records loading

		std::vector<std::string> _field_names;		// { "id", "url", "path", ... };
	};

}