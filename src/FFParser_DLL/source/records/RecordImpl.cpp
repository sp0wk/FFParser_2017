#include "RecordImpl.h"

#include "helpers/ErrorHandler.h"
#include <cassert>

#include <algorithm>
#include <cctype>


namespace FFParser {

	//ctors

	RecordImpl::RecordImpl(IRecordsStream& parent, const std::vector<std::string>& fields) :
		_parent_ref(parent),
		_fields(fields)
	{
		assert(parent.getNumberOfFields() == _fields.size());
	}

	RecordImpl::RecordImpl(IRecordsStream& parent, std::vector<std::string>&& fields) :
		_parent_ref(parent),
		_fields(std::move(fields))
	{
		assert(parent.getNumberOfFields() == _fields.size());
	}


	//Interface methods

	const char* CALL RecordImpl::getFieldByName(const char* name) const
	{
		if (!(name != nullptr && name[0] != '\0')) {
			//handle error
			ErrorHandler::getInstance().onError("Passed field name is empty", "ParserDLL error: \"Get record field by name error\"");
			return nullptr;
		}

		size_t nfields = _parent_ref.getNumberOfFields();

		//find field name
		for (size_t i = 0; i < nfields; ++i) {
			if (!strcmp(_parent_ref.getFieldName(i), name)) {
				//found
				return _fields[i].c_str();		//return corresponding field data
			}
		}

		return nullptr;
	}


	const char* CALL RecordImpl::getFieldValue(size_t index) const 
	{
		if (index < _fields.size())
			return _fields[index].c_str();
		return nullptr;
	}


	size_t CALL RecordImpl::getFieldSize(size_t index) const 
	{ 
		if (index < _fields.size())
			return _fields[index].size(); 
		return 0;
	}


	size_t CALL RecordImpl::findText(const char* text) const
	{
		if (!(text != nullptr && text[0] != '\0')) {
			//handle error
			ErrorHandler::getInstance().onError("Text to search is empty", "ParserDLL error: \"Record text search error\"");
			return -1;
		}

		const char* text_end = text + strlen(text);

		for (size_t i = 0; i < _fields.size(); ++i) {
			const std::string& str = _fields[i];

			//search substring
			auto res = std::search(str.cbegin(), str.cend(), text, text_end,
			[](char str1, char substr1)
			{
				return tolower(str1) == tolower(substr1);
			});

			if (res != str.cend()) {
				//match found in field i
				return i;
			}
		}

		return -1;
	}

}