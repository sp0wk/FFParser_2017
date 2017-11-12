#pragma once


#include <vector>
#include <string>
#include <memory>

#include <regex>

#include "FileParserBase.h"
#include "../accessors/IFileAccessor.h"



namespace FFParser {

	class CacheFilesParser final : public FileParserBase
	{
	private:
		std::vector<std::string> filelist;		//cache files list

		//search patterns
		std::regex parseRegionRegex;
		std::regex urlRegex;
		std::regex filenameRegex;
		std::regex fnWithExtRegex;
		std::regex contentTypeRegex;
		std::regex contentEncodingRegex;
		std::regex dateRegex;
		std::regex lastModifiedRegex;
		std::regex expiresRegex;

		//generic file extension types
		std::string textType = "plain";
		std::string jsType = "javascript";
		std::string htmlType = "html";
		std::string cssType = "css";
		std::string jsonType = "json";
		std::string iconType = "icon";
		std::string svgType = "svg";

		//methods
		void parseCacheFile(const std::string& filename, std::vector<std::string>& output);

	public:
		CacheFilesParser(const std::shared_ptr<IFileAccessor>& fa);
		virtual ~CacheFilesParser() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) override;
		virtual size_t parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) override;
	};

}