#pragma once


#include <vector>
#include <string>
#include <memory>

#include "FileParserBase.h"
#include "../accessors/IFileAccessor.h"



namespace FFParser {

	class CacheFilesParser final : public FileParserBase
	{
	private:
		//TODO:
		//add regex pattern

		void parseCacheFile(const std::string& filename, std::vector<std::string>& output) const;

	public:
		CacheFilesParser(const std::shared_ptr<IFileAccessor>& fa);
		virtual ~CacheFilesParser() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) override;
		virtual void parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) override;
	};

}