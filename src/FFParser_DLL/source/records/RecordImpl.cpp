#include "RecordImpl.h"

#include <algorithm>
#include <cctype>


namespace FFParser {

	//Interface methods

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