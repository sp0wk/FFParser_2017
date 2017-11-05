#pragma once


#include <vector>
#include <string>
#include <memory>

#include "ParserBase.h"
#include "../accessors/IDatabaseAccessor.h"



namespace FFParser {

	class DBParserBase : public ParserBase
	{
	protected:
		std::weak_ptr<IDatabaseAccessor> _db_accessor_ref;
		std::vector<std::vector<std::string>> _db_records;

		void getRecordsFromDB(const std::string& db_path, std::string& query)
		{
			if (auto db_sh = _db_accessor_ref.lock()) {
				_db_records.clear();
				db_sh->connectToDB(db_path);
				db_sh->executeQuery(query, _db_records);
				db_sh->disconnectFromDB();
			}
		}

	public:
		//ctor and dtor
		DBParserBase(const std::shared_ptr<IFileAccessor>& fa, const std::shared_ptr<IDatabaseAccessor>& dba) :
			ParserBase(fa),
			_db_accessor_ref(dba)
		{
		}
		virtual ~DBParserBase() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) = 0;
		virtual void parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) = 0;
	};

}