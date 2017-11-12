#include "CacheFilesParser.h"


namespace FFParser {

	using std::regex_constants::icase;

	//ctor
	CacheFilesParser::CacheFilesParser(const std::shared_ptr<IFileAccessor>& fa) : 
		FileParserBase(fa),
		//patterns
		parseRegionRegex(":http", icase),
		urlRegex("(.*?)\\0", icase),
		filenameRegex(".+/(([-[:w:]](?:\\.)?)+)[^-[:w:]]?.*$"),
		fnWithExtRegex("([-[:w:]]+\\.)+[[:w:]]+"),
		contentTypeRegex("Content-Type:\\s*(.*?)\\r\\n", icase),
		contentEncodingRegex("Content-Encoding:\\s*(.*?)\\r\\n", icase),
		dateRegex("Date:\\s*(.*?)\\r\\n", icase),
		lastModifiedRegex("Last-Modified:\\s*(.*?)\\r\\n", icase),
		expiresRegex("Expires:\\s*(.*?)\\r\\n", icase),
		//file extensions
		textType("plain"),
		jsType("javascript"),
		htmlType("html"),
		cssType("css"),
		jsonType("json"),
		iconType("icon"),
		svgType("svg")
	{
		//set field names
		_field_names.reserve(9);
		_field_names.push_back("filename");
		_field_names.push_back("content_type");
		_field_names.push_back("url");
		_field_names.push_back("file_size");
		_field_names.push_back("content_encoding");
		_field_names.push_back("creation_date");
		_field_names.push_back("last_modified");
		_field_names.push_back("expires");
		_field_names.push_back("path");
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
		std::regex_search(_file_content, parseRegionMatch, parseRegionRegex);
		std::string parseRegionStr = parseRegionMatch.suffix().str();
		//cached file contents
		std::string main_content = parseRegionMatch.prefix().str();
		if (main_content.size() > 38) {
			main_content.erase(main_content.size() - 38);		//remove left over bytes from cache info
		}
		
		//file size
		std::string fileSizeStr = std::to_string(main_content.size());

		//url
		std::regex_search(parseRegionStr, urlMatch, urlRegex);
		std::string urlStr = "http" + urlMatch[1].str();

		//content type
		std::regex_search(parseRegionStr, contentTypeMatch, contentTypeRegex);
		std::string contentTypeStr = contentTypeMatch[1].str();

		//filename
		std::regex_search(urlStr, filenameMatch, filenameRegex);
		std::string filenameStr = filenameMatch[1].str();
		//.. and extension
		if (!std::regex_match(filenameStr, fnWithExtRegex))	{
			if (contentTypeStr.find(textType) != std::string::npos) filenameStr.append(".txt");
			else if (contentTypeStr.find(jsType) != std::string::npos) filenameStr.append(".js");
			else if (contentTypeStr.find(htmlType) != std::string::npos) filenameStr.append(".htm");
			else if (contentTypeStr.find(cssType) != std::string::npos) filenameStr.append(".css");
			else if (contentTypeStr.find(jsonType) != std::string::npos) filenameStr.append(".json");
			else if (contentTypeStr.find(iconType) != std::string::npos) filenameStr.append(".ico");
			else if (contentTypeStr.find(svgType) != std::string::npos) filenameStr.append(".svg");
			else filenameStr.append("." + contentTypeStr.substr(contentTypeStr.find("/") + 1));		//unknown type or content type is extension itself
		}

		//content encoding
		std::regex_search(parseRegionStr, contentEncodingMatch, contentEncodingRegex);
		std::string contentEncodingStr = contentEncodingMatch[1].str();

		//creation date
		std::regex_search(parseRegionStr, dateMatch, dateRegex);
		std::string dateStr = dateMatch[1].str();

		//last-modified date
		std::regex_search(parseRegionStr, lastModifiedMatch, lastModifiedRegex);
		std::string lastModifiedStr = lastModifiedMatch[1].str();

		//expires date
		std::regex_search(parseRegionStr, expiresMatch, expiresRegex);
		std::string expiresStr = expiresMatch[1].str();
	
	#pragma endregion CacheFileParsing


		//write parsed data to output
		output.reserve(_field_names.size());
		output.push_back(std::move(filenameStr));
		output.push_back(std::move(contentTypeStr));
		output.push_back(std::move(urlStr));
		output.push_back(std::move(fileSizeStr));
		output.push_back(std::move(contentEncodingStr));
		output.push_back(std::move(dateStr));
		output.push_back(std::move(lastModifiedStr));
		output.push_back(std::move(expiresStr));
		output.push_back(filename);
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
		size_t maxsz = filelist.size();
		size_t last = from + number;
		
		if (auto file_sh = _file_accessor_ref.lock()) {
			//get path to profile's cache folder
			path_to_cache = file_sh->getPathToResource(EResourcePaths::CACHE, profile) + "\\";
			
			//get cache file list (don't reload if it's not empty)
			if (filelist.empty()) {
				file_sh->getFileList(filelist, path_to_cache);
				maxsz = filelist.size();
			}

			//check bounds
			if (from > maxsz) {
				return 0;
			}
			if (last > maxsz) {
				last = maxsz;
				number = last - from;
			}

			//reserve memory for output
			output.reserve(number);

			//write to output
			for (size_t file_num = from; file_num < last; ++file_num) {
				output.emplace_back();
				parseCacheFile(path_to_cache + filelist[file_num], output[count]);
				++count;
			}
		}

		return count;
	}

}