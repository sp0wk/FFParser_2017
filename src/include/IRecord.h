#pragma once

#define CALL __cdecl

namespace FFParser {

	class IRecord
	{
	public:
		virtual const char* CALL getFieldValue(size_t index) const = 0;
		virtual size_t CALL getFieldSize(size_t index) const = 0;
	};

}