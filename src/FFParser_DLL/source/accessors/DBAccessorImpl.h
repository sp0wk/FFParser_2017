#pragma once


#include <memory>
#include <functional>
#include <vector>
#include <string>

#include "IDatabaseAccessor.h"

#include "../sqlite3/sqlite3.h"


namespace FFParser {

	class DBAccessorImpl : public IDatabaseAccessor
	{
	private:
		std::unique_ptr<sqlite3*, std::function<void(sqlite3**)>> _db;

	public:
		//ctor and dtor
		DBAccessorImpl();
		virtual ~DBAccessorImpl() = default;

		//Interface methods
		virtual int connectToDB(const std::string& db_path) override;
		virtual int executeQuery(const std::string& query) const override;
		virtual int executeQuery(const std::string& query, std::vector<std::vector<std::string>>& output) const override;
		virtual void disconnectFromDB() override;
	};

}