#include "BookmarksParser.h"

#include <ctime>	//for unix epoch to local time conversion

namespace FFParser {

	//ctor
	BookmarksParser::BookmarksParser(const std::shared_ptr<IFileAccessor>& fa, const std::shared_ptr<IDatabaseAccessor>& dba) : DBParserBase(fa, dba) 
	{
		//set field names
		_field_names.reserve(4);
		_field_names.push_back("id");
		_field_names.push_back("title");
		_field_names.push_back("url");
		_field_names.push_back("dateAdded");

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
		output.push_back( std::move(input[0]) );		//id
		output.push_back( std::move(input[1]) );		//title
		output.push_back( std::move(input[2]) );		//url

		btime = std::stoull(input[3]) / 1000000;
		stime = std::asctime(std::localtime(&btime));
		stime.erase(stime.length() - 1); //remove '\n'
		output.push_back(stime);						//dateAdded
	}
	
	
	size_t BookmarksParser::getTotalRecords(size_t profile)
	{
		if (auto file_sh = _file_accessor_ref.lock()) {
			getRecordsFromDB(file_sh->getPathToResource(EResourcePaths::DATABASE, profile), _bookmarks_count_query);

			return std::stoi(_db_records[0][0]);
		}

		return 0;
	}


	size_t BookmarksParser::parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number)
	{
		size_t count = 0;
		std::string query = _bookmarks_query + std::to_string(from) + ", " + (number ? std::to_string(number) : "-1") + ";";

		if (auto file_sh = _file_accessor_ref.lock()) {
			getRecordsFromDB(file_sh->getPathToResource(EResourcePaths::DATABASE, profile), query);

			output.reserve(_db_records.size());

			for (size_t i = 0; i < _db_records.size(); ++i) {
				output.emplace_back();
				parseBookmarkRecord(_db_records[i], output[i]);
				++count;
			}
		}

		return count;
	}

}