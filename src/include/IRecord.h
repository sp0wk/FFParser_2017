#pragma once


namespace FFParser {

	class IRecord
	{
	public:
		virtual const char* getFieldValue(size_t index) const = 0;
		virtual size_t getFieldSize(size_t index) const = 0;
	};

}