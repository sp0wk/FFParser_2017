#pragma once

#include <vector>
#include <string>

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
		virtual const char* CALL getFieldValue(size_t index) const override;
		virtual size_t CALL getFieldSize(size_t index) const override;
		virtual size_t CALL findText(const char* text) const override;

	protected:
		using Fields = std::vector<std::string>;

		Fields _fields;
	};

}