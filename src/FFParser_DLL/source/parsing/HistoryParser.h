#pragma once


#include <vector>
#include <string>
#include <memory>

#include "DBParserBase.h"
#include "../accessors/IDatabaseAccessor.h"


namespace FFParser {

	class HistoryParser final : public DBParserBase
	{
	private:
		std::string _history_query;
		std::string _history_count_query;

		void parseHistoryRecord(const std::vector<std::string>& input, std::vector<std::string>& output);

	public:
		//ctor and dtor
		HistoryParser(const std::shared_ptr<IFileAccessor>& fa, const std::shared_ptr<IDatabaseAccessor>& dba);
		virtual ~HistoryParser() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) override;
		virtual void parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) override;
	};

}