#include "DBAccessorImpl.h"


namespace FFParser {

	//ctor

	DBAccessorImpl::DBAccessorImpl() :
		_db(nullptr, [](sqlite3* db) { if (db != nullptr) sqlite3_close(db); })		//close db on ptr destruction
	{
	}


	//Interface methods

	int DBAccessorImpl::connectToDB(const std::string& db_path)
	{
		int res = sqlite3_open(db_path.c_str(), &_raw_db);
		
		//if open failed
		if (res) {
			sqlite3_close(_raw_db);
		}
		else {
			_db.reset(_raw_db);		//set RAII pointer
		}

		return res;
	}


	int DBAccessorImpl::executeQuery(const std::string& query) const
	{
		int status;
		char* error;

		if (_db.get() == nullptr) {
			return -1;
		}

		status = sqlite3_exec(_db.get(), query.c_str(), NULL, 0, &error);

		if (status) {
			//TODO:
			//error can be logged here
			sqlite3_free(error);
		}

		return status;
	}


	int DBAccessorImpl::executeQuery(const std::string& query, std::vector<std::vector<std::string>>& output) const
	{
		int status;
		int columns;
		sqlite3_stmt* stmt = nullptr;


		if (_db.get() == nullptr) {
			return -1;
		}

		status = sqlite3_prepare_v2(_db.get(), query.c_str(), query.length(), &stmt, NULL);

		if (status) {
			//TODO:
			//log error or ignore
			return status;
		}

		columns = sqlite3_column_count(stmt);

		//for every row
		while (sqlite3_step(stmt) == SQLITE_ROW) {
			std::vector<std::string> tmp(columns);

			//for every column
			for (int i = 0; i < columns; ++i) {
				tmp[i] = (const char*)sqlite3_column_text(stmt, i);
			}

			output.push_back(std::move(tmp));
		}

		sqlite3_finalize(stmt);

		return status;
	}


	void DBAccessorImpl::disconnectFromDB()
	{
		if (_db.get() != nullptr) {
			sqlite3_close(_db.get());
		}
		_db.reset();
	}

}