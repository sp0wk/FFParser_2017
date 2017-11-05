#pragma once

#include <vector>
#include <string>


namespace FFParser {

	class IDatabaseAccessor
	{
	public:
		virtual int connectToDB(const std::string& db_path) = 0;
		virtual int executeQuery(const std::string& query) const = 0;
		virtual int executeQuery(const std::string& query, std::vector<std::vector<std::string>>& output) const = 0;
		virtual void disconnectFromDB() = 0;
		virtual ~IDatabaseAccessor() {}
	};

}