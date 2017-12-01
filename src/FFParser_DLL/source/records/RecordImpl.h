#pragma once

#include <vector>
#include <string>

#include "include/IRecord.h"
#include "include/IRecordsStream.h"


namespace FFParser {

	class RecordImpl : public IRecord
	{
	public:
		using Fields = std::vector<std::string>;

		//ctors and dtor
		RecordImpl(IRecordsStream& parent, const Fields& fields);
		RecordImpl(IRecordsStream& parent, Fields&& fields);
		virtual ~RecordImpl() = default;

		//Interface methods
		virtual const char* CALL getFieldByName(const char* name) const override;
		virtual const char* CALL getFieldValue(size_t index) const override;
		virtual size_t CALL getFieldSize(size_t index) const override;
		virtual size_t CALL findText(const char* text) const override;

	protected:
		IRecordsStream& _parent_ref;
		Fields _fields;
	};

}