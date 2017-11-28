#include "CacheFilesParser.h"


namespace FFParser {

	using std::regex_constants::icase;

	//class members

	//patterns
	std::regex CacheFilesParser::s_parseRegionRegex(":http", icase);
	std::regex CacheFilesParser::s_urlRegex("(.*?)\\0", icase);
	std::regex CacheFilesParser::s_filenameRegex(".+/(([-[:w:]](?:\\.)?)+)[^-[:w:]]?.*$");
	std::regex CacheFilesParser::s_fnWithExtRegex("([-[:w:]]+\\.)+[[:w:]]+");
	std::regex CacheFilesParser::s_contentTypeRegex("Content-Type:\\s*(.*?)\\r\\n", icase);
	std::regex CacheFilesParser::s_contentEncodingRegex("Content-Encoding:\\s*(.*?)\\r\\n", icase);
	std::regex CacheFilesParser::s_dateRegex("Date:\\s*(.*?)\\r\\n", icase);
	std::regex CacheFilesParser::s_lastModifiedRegex("Last-Modified:\\s*(.*?)\\r\\n", icase);
	std::regex CacheFilesParser::s_expiresRegex("Expires:\\s*(.*?)\\r\\n", icase);
	//file extensions
	std::string CacheFilesParser::s_textType("plain");
	std::string CacheFilesParser::s_jsType("javascript");
	std::string CacheFilesParser::s_htmlType("html");
	std::string CacheFilesParser::s_cssType("css");
	std::string CacheFilesParser::s_jsonType("json");
	std::string CacheFilesParser::s_iconType("icon");
	std::string CacheFilesParser::s_svgType("svg");


	//ctor
	CacheFilesParser::CacheFilesParser(const std::shared_ptr<IFileAccessor>& fa) : 
		FileParserBase(fa)
	{
		//set field names
		_field_names.reserve(9);
		_field_names.emplace_back("filename");
		_field_names.emplace_back("content_type");
		_field_names.emplace_back("url");
		_field_names.emplace_back("file_size");
		_field_names.emplace_back("content_encoding");
		_field_names.emplace_back("creation_date");
		_field_names.emplace_back("last_modified");
		_field_names.emplace_back("expires");
		_field_names.emplace_back("path");
	}

	//methods

	void CacheFilesParser::parseCacheFile(const std::string& filename, std::vector<std::string>& output)
	{
		std::smatch parseRegionMatch;
		std::smatch urlMatch;
		std::smatch filenameMatch;
		std::smatch contentTypeMatch;
		std::smatch contentEncodingMatch;
		std::smatch dateMatch;
		std::smatch lastModifiedMatch;
		std::smatch expiresMatch;

		//read cache file to content string
		if ( !readFileToString(filename) ) {
			return;
		}

	#pragma region CacheFileParsing

		//find parse region in file
		std::regex_search(_file_content, parseRegionMatch, s_parseRegionRegex);
		std::string parseRegionStr = parseRegionMatch.suffix().str();
		//cached file contents
		std::string main_content = parseRegionMatch.prefix().str();
		if (main_content.size() > 38) {
			main_content.erase(main_content.size() - 38);		//remove left over bytes from cache info
		}
		
		//file size
		std::string fileSizeStr = std::to_string(main_content.size());

		//url
		std::regex_search(parseRegionStr, urlMatch, s_urlRegex);
		std::string urlStr = "http" + urlMatch[1].str();

		//content type
		std::regex_search(parseRegionStr, contentTypeMatch, s_contentTypeRegex);
		std::string contentTypeStr = contentTypeMatch[1].str();

		//filename
		std::regex_search(urlStr, filenameMatch, s_filenameRegex);
		std::string filenameStr = filenameMatch[1].str();
		//.. and extension
		if (!std::regex_match(filenameStr, s_fnWithExtRegex)) {
			if (contentTypeStr.find(s_textType) != std::string::npos) filenameStr.append(".txt");
			else if (contentTypeStr.find(s_jsType) != std::string::npos) filenameStr.append(".js");
			else if (contentTypeStr.find(s_htmlType) != std::string::npos) filenameStr.append(".htm");
			else if (contentTypeStr.find(s_cssType) != std::string::npos) filenameStr.append(".css");
			else if (contentTypeStr.find(s_jsonType) != std::string::npos) filenameStr.append(".json");
			else if (contentTypeStr.find(s_iconType) != std::string::npos) filenameStr.append(".ico");
			else if (contentTypeStr.find(s_svgType) != std::string::npos) filenameStr.append(".svg");
			else filenameStr.append("." + contentTypeStr.substr(contentTypeStr.find("/") + 1));		//unknown type or content type is extension itself
		}

		//content encoding
		std::regex_search(parseRegionStr, contentEncodingMatch, s_contentEncodingRegex);
		std::string contentEncodingStr = contentEncodingMatch[1].str();

		//creation date
		std::regex_search(parseRegionStr, dateMatch, s_dateRegex);
		std::string dateStr = dateMatch[1].str();

		//last-modified date
		std::regex_search(parseRegionStr, lastModifiedMatch, s_lastModifiedRegex);
		std::string lastModifiedStr = lastModifiedMatch[1].str();

		//expires date
		std::regex_search(parseRegionStr, expiresMatch, s_expiresRegex);
		std::string expiresStr = expiresMatch[1].str();
	
	#pragma endregion CacheFileParsing

		//write parsed data to output
		output.reserve(_field_names.size());
		output.emplace_back(std::move(filenameStr));
		output.emplace_back(std::move(contentTypeStr));
		output.emplace_back(std::move(urlStr));
		output.emplace_back(std::move(fileSizeStr));
		output.emplace_back(std::move(contentEncodingStr));
		output.emplace_back(std::move(dateStr));
		output.emplace_back(std::move(lastModifiedStr));
		output.emplace_back(std::move(expiresStr));
		output.emplace_back(filename);
	}


	size_t CacheFilesParser::getTotalRecords(size_t profile)
	{
		if (auto file_sh = _file_accessor_ref.lock()) {
			return file_sh->getFilesCount(file_sh->getPathToResource(EResourcePaths::CACHE, profile));
		}
		return 0;
	}


	size_t CacheFilesParser::parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number)
	{
		size_t count = 0;
		std::string path_to_cache;
		size_t maxsz = _filelist.size();
		size_t last = from + number;
		
		if (auto file_sh = _file_accessor_ref.lock()) {
			//get path to profile's cache folder
			path_to_cache = file_sh->getPathToResource(EResourcePaths::CACHE, profile) + "\\";
			
			//get cache file list (don't reload if it's not empty)
			if (_filelist.empty()) {
				file_sh->getFileList(_filelist, path_to_cache);
				maxsz = _filelist.size();
			}

			//check bounds
			if (from >= maxsz) {
				return 0;
			}
			if (last >= maxsz || number == 0) {
				last = maxsz;
				number = last - from;
			}

			//reserve memory for output
			output.reserve(number);

			//write to output
			for (size_t file_num = from; file_num < last; ++file_num) {
				output.emplace_back();
				parseCacheFile(path_to_cache + _filelist[file_num], output[count]);
				++count;
			}
		}

		return count;
	}

}