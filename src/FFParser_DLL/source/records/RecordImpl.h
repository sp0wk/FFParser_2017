#pragma once

#include <vector>
#include <string>

#include "include/IRecord.h"


namespace FFParser {

	class RecordImpl : public IRecord
	{
	protected:
		using Fields = std::vector<std::string>;

		Fields _fields;

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
	};

}