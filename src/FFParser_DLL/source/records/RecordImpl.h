#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <cctype>

#include "include/IRecord.h"


namespace FFParser {

	class RecordImpl : public IRecord
	{
	public:
		//ctors and dtor
		RecordImpl() = default;
		explicit RecordImpl(size_t num_of_fields) : _fields(num_of_fields) {}
		explicit RecordImpl(const std::vector<std::string>& fields) : _fields(fields) {}
		explicit RecordImpl(std::vector<std::string>&& fields) : _fields(std::move(fields)) {}
		virtual ~RecordImpl() = default;


		//Interface methods
		virtual const char* CALL getFieldValue(size_t index) const override 
		{
			if (index < _fields.size())
				return _fields[index].c_str(); 
			return nullptr;
		}

		virtual size_t CALL getFieldSize(size_t index) const override 
		{ 
			if (index < _fields.size())
				return _fields[index].size(); 
			return 0;
		}

		size_t findText(const char* text) const
		{
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

	protected:
		using Fields = std::vector<std::string>;

		Fields _fields;
	};

}