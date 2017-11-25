#pragma once


#include <vector>
#include <string>
#include <memory>

#include <regex>

#include "FileParserBase.h"
#include "accessors/IFileAccessor.h"



namespace FFParser {

	class CacheFilesParser final : public FileParserBase
	{
	public:
		CacheFilesParser(const std::shared_ptr<IFileAccessor>& fa);
		virtual ~CacheFilesParser() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) override;
		virtual size_t parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) override;

	private:
		std::vector<std::string> _filelist;		//cache files list

		//search patterns
		std::regex _parseRegionRegex;
		std::regex _urlRegex;
		std::regex _filenameRegex;
		std::regex _fnWithExtRegex;
		std::regex _contentTypeRegex;
		std::regex _contentEncodingRegex;
		std::regex _dateRegex;
		std::regex _lastModifiedRegex;
		std::regex _expiresRegex;

		//generic file extension types
		std::string _textType = "plain";
		std::string _jsType = "javascript";
		std::string _htmlType = "html";
		std::string _cssType = "css";
		std::string _jsonType = "json";
		std::string _iconType = "icon";
		std::string _svgType = "svg";

		//methods
		void parseCacheFile(const std::string& filename, std::vector<std::string>& output);
	};

}