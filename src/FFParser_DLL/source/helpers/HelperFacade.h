#pragma once

#include <memory>
#include <string>
#include <vector>
#include <array>

#include "include/ERecordTypes.h"

#include "../accessors/IFileAccessor.h"
#include "../accessors/IDatabaseAccessor.h"
#include "../accessors/FileAccessorImpl.h"
#include "../accessors/DBAccessorImpl.h"

#include "../parsing/CacheFilesParser.h"
#include "../parsing/HistoryParser.h"
#include "../parsing/BookmarksParser.h"
#include "../parsing/LoginsParser.h"



// Singleton helper

namespace FFParser {

	class HelperFacade final
	{
	private:
		//accessors
		std::shared_ptr<IFileAccessor> _file_accessor;
		std::shared_ptr<IDatabaseAccessor> _db_accessor;

		//parsers
		std::vector<std::vector<std::unique_ptr<ParserBase>>> _parsers;

		//private ctor and dtor
		HelperFacade();
		~HelperFacade() = default;

	public:
		static HelperFacade& getInstance();

		void getProfileNames(std::vector<std::string>& list) const;
		std::string getPathToProfile(size_t profile) const;
		void getFieldNames(std::vector<std::string>& list, ERecordTypes type) const;
		size_t getTotalRecords(ERecordTypes type, size_t profile);
		size_t parseRecords(ERecordTypes type, size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number);
	};

}