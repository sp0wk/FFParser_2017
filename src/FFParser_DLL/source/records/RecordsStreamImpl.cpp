#include "RecordsStreamImpl.h"

#include <string>
#include <vector>


namespace FFParser {

	//ctor
	RecordsStreamImpl::RecordsStreamImpl(size_t profile, ERecordTypes type) :
		_helper_ref(HelperFacade::getInstance()),
		_profile(profile),
		_stream_type(type),
		_current_record(-1),
		_last_from(0)
	{
		_helper_ref.getFieldNames(_field_names, _stream_type);
	}


	//Interface methods


	size_t CALL RecordsStreamImpl::getTotalRecords() const
	{
		return _helper_ref.getTotalRecords(_stream_type, _profile);
	}


	size_t CALL RecordsStreamImpl::loadNextRecords(size_t number)
	{
		size_t count = 0;
		std::vector<std::vector<std::string>> tmp;

		count = _helper_ref.parseRecords(_stream_type, _profile, tmp, _last_from + _records.size(), number);

		for (auto& iter : tmp) {
			_records.emplace_back(std::move(iter));
		}

		return count;
	}


	size_t CALL RecordsStreamImpl::loadRecords(size_t from, size_t number)
	{
		size_t count = 0;
		std::vector<std::vector<std::string>> tmp;

		_last_from = from;
		count = _helper_ref.parseRecords(_stream_type, _profile, tmp, from, number);

		for (auto& iter : tmp) {
			_records.emplace_back(std::move(iter));
		}

		return count;
	}


	size_t CALL RecordsStreamImpl::getNumberOfRecords() const
	{
		return _records.size();
	}


	size_t CALL RecordsStreamImpl::currentRecord() const
	{
		return _current_record;
	}


	IRecord* CALL RecordsStreamImpl::getPrevRecord()
	{
		if (_current_record == 0 || _records.size() == 0) {
			return nullptr;
		}
		return &_records[--_current_record];
	}


	IRecord* CALL RecordsStreamImpl::getNextRecord()
	{
		if (_current_record + 1 == _records.size()) {
			return nullptr;
		}
		return &_records[++_current_record];
	}


	IRecord* CALL RecordsStreamImpl::getRecordByIndex(size_t index)
	{
		if (index < _records.size()) {
			_current_record = index;
			return &_records[_current_record];
		}
		return nullptr;
	}


	size_t CALL RecordsStreamImpl::getNumberOfFields() const
	{
		return _field_names.size();
	}


	const char* CALL RecordsStreamImpl::getFieldName(size_t index) const
	{
		if (index < _field_names.size()) {
			return _field_names[index].c_str();
		}
		return nullptr;
	}

}