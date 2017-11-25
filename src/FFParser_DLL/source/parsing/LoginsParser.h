#pragma once

#include <string>
#include <memory>

#include <boost/property_tree/ptree.hpp>

#include "FileParserBase.h"
#include "LoginsDecryptor.h"


namespace FFParser {

	class LoginsParser final : public FileParserBase
	{
	public:
		LoginsParser(const std::shared_ptr<IFileAccessor>& fa);
		virtual ~LoginsParser() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) override;
		virtual size_t parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) override;

	private:
		boost::property_tree::ptree _pt;
		LoginsDecryptor _decryptor;
	};

}