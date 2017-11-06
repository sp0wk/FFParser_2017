#pragma once


#include <vector>
#include <string>
#include <memory>

#include "DBParserBase.h"
#include "../accessors/IDatabaseAccessor.h"



namespace FFParser {

	class BookmarksParser final : public DBParserBase
	{
	private:
		std::string _bookmarks_query;
		std::string _bookmarks_count_query;

		void parseBookmarkRecord(const std::vector<std::string>& input, std::vector<std::string>& output) const;

	public:
		//ctor and dtor
		BookmarksParser(const std::shared_ptr<IFileAccessor>& fa, const std::shared_ptr<IDatabaseAccessor>& dba);
		virtual ~BookmarksParser() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) override;
		virtual size_t parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) override;
	};

}