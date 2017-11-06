#pragma once


#include <vector>
#include <string>
#include <memory>

#include "ParserBase.h"


namespace FFParser {

	class FileParserBase : public ParserBase
	{
	protected:
		std::string _file_content;

		size_t readFileToString(const std::string& file_path) const
		{
			//TODO:
			//read file to content and return number of read bytes
			return 0;
		}

	public:
		//ctor and dtor
		FileParserBase(const std::shared_ptr<IFileAccessor>& fa) : ParserBase(fa) {}
		virtual ~FileParserBase() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) = 0;
		virtual size_t parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) = 0;
	};

}