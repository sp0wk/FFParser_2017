#include "HistoryParser.h"

#include <ctime>	//for unix epoch to local time conversion

namespace FFParser {

	//ctor
	HistoryParser::HistoryParser(const std::shared_ptr<IFileAccessor>& fa, const std::shared_ptr<IDatabaseAccessor>& dba) : DBParserBase(fa, dba) 
	{
		//set field names
		_field_names.reserve(4);
		_field_names.push_back("id");
		_field_names.push_back("url");
		_field_names.push_back("title");
		_field_names.push_back("visit_date");

		//set main query
		_history_query =	"SELECT hv.id, mz.url, mz.title, hv.visit_date \
							FROM moz_historyvisits hv \
							JOIN moz_places mz ON hv.place_id = mz.id \
							WHERE mz.title IS NOT NULL \
							ORDER BY hv.id DESC \
							LIMIT ";
		//set count query
		_history_count_query = "SELECT count(*) FROM moz_historyvisits WHERE title IS NOT NULL;";
	}


	//methods

	void HistoryParser::parseHistoryRecord(const std::vector<std::string>& input, std::vector<std::string>& output)
	{
		std::time_t htime;
		
		output.reserve(input.size());

		//parse process or just copy data

		output.push_back( std::move(input[0]) );		//id
		output.push_back( std::move(input[1]) );		//url
		output.push_back( std::move(input[2]) );		//title

		htime = std::stoull(input[3]) / 1000000;
		output.push_back( std::asctime(std::localtime(&htime)) );		//visit_date
	}
	

	size_t HistoryParser::getTotalRecords(size_t profile)
	{
		if (auto file_sh = _file_accessor_ref.lock()) {
			getRecordsFromDB(file_sh->getPathToResource(EResourcePaths::DATABASE, profile), _history_count_query);

			return std::stoi(_db_records[0][0]);
		}

		return 0;
	}


	void HistoryParser::parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number)
	{
		std::string query = _history_query + std::to_string(from) + ", " + (number ? std::to_string(number) : "-1") + ";";

		if (auto file_sh = _file_accessor_ref.lock()) {
			getRecordsFromDB(file_sh->getPathToResource(EResourcePaths::DATABASE, profile), query);

			output.reserve(_db_records.size());

			for (size_t i = 0; i < _db_records.size(); ++i) {
				parseHistoryRecord(_db_records[i], output[i]);
			}
		}
	}

}