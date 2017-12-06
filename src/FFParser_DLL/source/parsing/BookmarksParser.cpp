#include "BookmarksParser.h"

#include <ctime>	//for unix epoch to local time conversion

#include <atomic>
extern std::atomic_bool STOP_PARSING_FLAG;


namespace FFParser {

	//ctor
	BookmarksParser::BookmarksParser(const std::shared_ptr<IFileAccessor>& fa, const std::shared_ptr<IDatabaseAccessor>& dba) : 
		DBParserBase(fa, dba) 
	{
		//set field names
		_field_names.reserve(4);
		_field_names.emplace_back("id");
		_field_names.emplace_back("title");
		_field_names.emplace_back("url");
		_field_names.emplace_back("dateAdded");

		//set main query
		_bookmarks_query =	"SELECT bk.id, bk.title, pl.url, bk.dateAdded \
							FROM moz_bookmarks bk \
							JOIN moz_places pl ON bk.fk = pl.id \
							WHERE bk.fk IS NOT NULL AND bk.title IS NOT NULL \
							ORDER BY bk.title \
							LIMIT ";
		//count query
		_bookmarks_count_query =	"SELECT count(*) \
									FROM moz_bookmarks bk \
									JOIN moz_places pl ON bk.fk = pl.id \
									WHERE bk.fk IS NOT NULL AND bk.title IS NOT NULL;";
	}


	//methods
	
	void BookmarksParser::parseBookmarkRecord(const std::vector<std::string>& input, std::vector<std::string>& output) const
	{
		std::time_t btime;
		std::string stime;

		output.reserve(input.size());

		//parse process or just copy data
		output.emplace_back( std::move(input[0]) );		//id
		output.emplace_back( std::move(input[1]) );		//title
		output.emplace_back( std::move(input[2]) );		//url

		btime = std::stoull(input[3]) / 1000000;
		stime = std::asctime(std::localtime(&btime));
		stime.erase(stime.length() - 1); //remove '\n'
		output.emplace_back(stime);						//dateAdded
	}
	
	
	size_t BookmarksParser::getTotalRecords(size_t profile)
	{
		if (auto file_sh = _file_accessor_ref.lock()) {
			getRecordsFromDB(file_sh->getPathToResource(EResourcePaths::DATABASE, profile), _bookmarks_count_query);

			return _db_records.size() > 0 ? std::stoi(_db_records[0][0]) : 0;
		}

		return 0;
	}


	size_t BookmarksParser::parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number)
	{
		size_t count = 0;
		std::string query = _bookmarks_query + std::to_string(from) + ", " + (number == -1 ? "-1" : std::to_string(number)) + ";";

		if (auto file_sh = _file_accessor_ref.lock()) {
			getRecordsFromDB(file_sh->getPathToResource(EResourcePaths::DATABASE, profile), query);

			output.reserve(_db_records.size());

			for (size_t i = 0; i < _db_records.size(); ++i) {

				//check global stop parsing flag
				if (STOP_PARSING_FLAG.load()) {
					break;
				}

				output.emplace_back();
				parseBookmarkRecord(_db_records[i], output[i]);
				++count;
			}
		}

		return count;
	}

}