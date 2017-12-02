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
		struct FileTypeAndExt
		{
			std::string type;
			std::string ext;
		};

		using FileTypesArray = std::vector<FileTypeAndExt>;

		//ctors and dtor
		CacheFilesParser(const std::shared_ptr<IFileAccessor>& fa);
		virtual ~CacheFilesParser() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) override;
		virtual size_t parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) override;

	private:
		std::vector<std::string> _filelist;		//cache files list

		//search patterns
		static std::regex s_parseRegionRegex;
		static std::regex s_urlRegex;
		static std::regex s_filenameRegex;
		static std::regex s_fnWithExtRegex;
		static std::regex s_contentTypeRegex;
		static std::regex s_contentLengthRegex;
		static std::regex s_contentEncodingRegex;
		static std::regex s_dateRegex;
		static std::regex s_lastModifiedRegex;
		static std::regex s_expiresRegex;

		//generic file types and extensions
		static const FileTypesArray s_fileTypes;

		//methods
		void parseCacheFile(const std::string& filename, std::vector<std::string>& output);
	};

}