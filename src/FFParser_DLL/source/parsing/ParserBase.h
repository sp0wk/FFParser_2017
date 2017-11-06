#pragma once


#include <vector>
#include <string>
#include <memory>

#include "../accessors/IFileAccessor.h"


namespace FFParser {

	class ParserBase
	{
	protected:
		std::vector<std::string> _field_names;
		std::weak_ptr<IFileAccessor> _file_accessor_ref;

	public:
		//ctor and dtor
		ParserBase(const std::shared_ptr<IFileAccessor>& fa) : _file_accessor_ref(fa) {}
		virtual ~ParserBase() = default;

		//methods
		void getFieldNames(std::vector<std::string>& output) const
		{
			output.reserve(_field_names.size());
			for (auto& iter : _field_names) {
				output.push_back(iter);
			}
		}

		virtual size_t getTotalRecords(size_t profile) = 0;
		virtual size_t parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) = 0;
	};

}