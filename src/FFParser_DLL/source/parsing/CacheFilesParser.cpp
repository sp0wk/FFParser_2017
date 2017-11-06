#include "CacheFilesParser.h"


namespace FFParser {

	//ctor
	CacheFilesParser::CacheFilesParser(const std::shared_ptr<IFileAccessor>& fa) : FileParserBase(fa)
	{
		//TODO:
		//set fields and pattern
	}


	//methods

	void CacheFilesParser::parseCacheFile(const std::string& filename, std::vector<std::string>& output) const
	{
		//TODO:
		//parse file
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
		
		//TODO:
		//parse cache

		return count;
	}

}