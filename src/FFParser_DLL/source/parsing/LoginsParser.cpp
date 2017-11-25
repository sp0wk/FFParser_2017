#include "LoginsParser.h"

#include <Windows.h>	//for MessageBox

#include <boost/property_tree/json_parser.hpp>


namespace FFParser {

	//ctor
	LoginsParser::LoginsParser(const std::shared_ptr<IFileAccessor>& fa) : 
		FileParserBase(fa)
	{
		//set field names
		_field_names.reserve(3);
		_field_names.emplace_back("hostname");
		_field_names.emplace_back("username");
		_field_names.emplace_back("password");

		//get install path and load libs
		if (auto sh = _file_accessor_ref.lock()) {
			_decryptor.loadLibraries(sh->getPathToResource(EResourcePaths::INSTALLDIR));
		}
		
	}

	//methods

	size_t LoginsParser::getTotalRecords(size_t profile)
	{
		std::string profileDir;

		if (auto sh = _file_accessor_ref.lock()) {
			profileDir = sh->getPathToResource(EResourcePaths::PROFILES, profile);
		}

		try	{
			if (_pt.empty()) {
				boost::property_tree::read_json(profileDir.append("\\logins.json"), _pt);
			}

			return _pt.get_child("logins").size();
		}
		catch (const std::exception& ex) {
			//display error
			MessageBoxA(NULL, ex.what(), "ParserDLL error: \"Error during LOGINS.JSON parsing occured\"", MB_OK | MB_ICONERROR);
		}

		return 0;
	}


	size_t LoginsParser::parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number)
	{
		size_t count = 0;
		std::string profileDir;
		size_t limit_counter = 0;
		bool nolimit = false;
		size_t max = from + number;

		if (number == 0) {
			nolimit = true;
		}

		if (auto sh = _file_accessor_ref.lock()) {
			profileDir = sh->getPathToResource(EResourcePaths::PROFILEPATH, profile);
		}

		//initialize decryption
		_decryptor.init(profileDir.c_str());

		try	{
			if (_pt.empty()) {
				boost::property_tree::read_json(profileDir.append("\\logins.json"), _pt);
			}

			//iterate through all logins
			for (auto search_result : _pt.get_child("logins")) {
				//limit returned records
				if (limit_counter >= from && (limit_counter < max || nolimit)) {
					std::vector<std::string> tmp;
					
					tmp.emplace_back(search_result.second.get<std::string>("hostname"));
					tmp.emplace_back(_decryptor.decryptString(search_result.second.get<std::string>("encryptedUsername")));
					tmp.emplace_back(_decryptor.decryptString(search_result.second.get<std::string>("encryptedPassword")));

					output.emplace_back(std::move(tmp));
					++count;
				}

				++limit_counter;
			}
		}
		catch (const std::exception& ex) {
			//display error
			MessageBoxA(NULL, ex.what(), "ParserDLL error: \"Error during LOGINS.JSON parsing occured\"", MB_OK | MB_ICONERROR);
		}

		_decryptor.close();

		return count;
	}

}