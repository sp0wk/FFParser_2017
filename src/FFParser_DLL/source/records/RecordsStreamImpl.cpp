#include "RecordsStreamImpl.h"

#include <string>
#include <vector>


namespace FFParser {

	//ctor
	RecordsStreamImpl::RecordsStreamImpl(size_t profile, ERecordTypes type) :
		_helper_ref(HelperFacade::getInstance()),
		_profile(profile),
		_stream_type(type),
		_current_record(-1)
	{
		_helper_ref.getFieldNames(_field_names, _stream_type);
	}


	//Interface methods


	size_t RecordsStreamImpl::getTotalRecords(size_t profile) const
	{
		return _helper_ref.getTotalRecords(_stream_type, profile);
	}


	size_t RecordsStreamImpl::loadRecords(size_t number)
	{
		std::vector<std::vector<std::string>> tmp;

		_helper_ref.parseRecords(_stream_type, _profile, tmp, _records.size(), number);

		for (auto& iter : tmp) {
			_records.push_back(std::move(static_cast<RecordImpl>(iter)));
		}

		return _records.size();
	}


	size_t RecordsStreamImpl::loadRecords(size_t from, size_t number)
	{
		std::vector<std::vector<std::string>> tmp;

		_helper_ref.parseRecords(_stream_type, _profile, tmp, from, number);

		for (auto& iter : tmp) {
			_records.push_back(std::move(static_cast<RecordImpl>(iter)));
		}

		return _records.size();
	}


	size_t RecordsStreamImpl::getNumberOfRecords() const
	{
		return _records.size();
	}


	size_t RecordsStreamImpl::currentRecord() const
	{
		return _current_record;
	}


	IRecord* RecordsStreamImpl::getPrevRecord()
	{
		if (_current_record == 0 || _records.size() == 0) {
			return nullptr;
		}
		return &_records[--_current_record];
	}


	IRecord* RecordsStreamImpl::getNextRecord()
	{
		if (_current_record + 1 == _records.size()) {
			return nullptr;
		}
		return &_records[++_current_record];
	}


	IRecord* RecordsStreamImpl::getRecordByIndex(size_t index)
	{
		if (index < _records.size()) {
			_current_record = index;
			return &_records[_current_record];
		}
		return nullptr;
	}


	size_t RecordsStreamImpl::getNumberOfFields() const
	{
		return _field_names.size();
	}


	const char* RecordsStreamImpl::getFieldName(size_t index) const
	{
		if (index < _field_names.size()) {
			return _field_names[index].c_str();
		}
		return nullptr;
	}

}