#pragma once


#include <vector>
#include <string>
#include <memory>
//read file headers
#include <fstream>
#include <streambuf>

#include <Windows.h>	//for MessageBox

#include "ParserBase.h"


namespace FFParser {

	class FileParserBase : public ParserBase
	{
	protected:
		std::string _file_content;

		size_t readFileToString(const std::string& file_path)
		{
			//open file
			std::ifstream file(file_path, std::ios::in | std::ifstream::binary);

			if (!file.is_open()) {
				//display error
				std::string error = "Couldn't open cache file:\n" + file_path;
				MessageBoxA(NULL, error.c_str(), "ParserDLL error: \"File open error\"", MB_OK | MB_ICONERROR);
				return 0;
			}

			//read file
			_file_content.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
			
			return _file_content.size();
		}

	public:
		//ctor and dtor
		FileParserBase(const std::shared_ptr<IFileAccessor>& fa) : ParserBase(fa) {}
		virtual ~FileParserBase() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) = 0;
		virtual size_t parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) = 0;
	};

}