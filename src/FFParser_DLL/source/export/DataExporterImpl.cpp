#include "DataExporterImpl.h"

#include "helpers/ErrorHandler.h"
#include <cassert>

#include <fstream>
#include <sstream>
#include <vector>

#include <chrono>
#include <iomanip>

#include <boost/property_tree/json_parser.hpp>

//for gzip decompressing
#include <streambuf>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>

#include <wincrypt.h>	//for MD5
#include <regex>


namespace FFParser {

	//ctor
	DataExporterImpl::DataExporterImpl(IStorageFactory& storage) :
		_storage_ref(storage)
	{
	}


	//Interface methods

	bool CALL DataExporterImpl::exportRecords(IRecordsStream* rstream, const char* output_path, bool bCreateMD5) const
	{
		using boost::property_tree::ptree;
		using boost::property_tree::write_json;
		
		ptree pt;
		std::string filename;

		if (rstream == nullptr) {
			ErrorHandler::getInstance().onError("Passed parameter is NULL", "ParserDLL error: \"Export records error\"");
			return false;
		}

		//create json ptree and generate file name
		filename = output_path != nullptr ? output_path : "";
		filename.append("/");
		filename.append(createExportPtree(rstream, pt));

		//try to create export file
		std::ofstream file(filename, std::ios::out);
		if (!file.is_open()) {
			//handle error
			std::string error = "Couldn't create export file:\n";
			error.append(filename);
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Export records error\"");
			return false;
		}

		//write ptree to export file
		try {
			write_json(file, pt, true);
		}
		catch (const std::exception& ex) {
			//handle error
			ErrorHandler::getInstance().onError(ex.what(), "ParserDLL error: \"Export records error\"");
			return false;
		}

		file.close();

		//create md5 if needed
		if (bCreateMD5) {
			createMD5fromFile(filename.c_str());
		}

		return true;
	}


	bool CALL DataExporterImpl::exportCacheFile(IRecord* file_record, const char* output_path) const
	{
		if (file_record == nullptr) {
			ErrorHandler::getInstance().onError("Passed parameter is NULL", "ParserDLL error: \"Export cache file error\"");
			return false;
		}

		//get required fields
		const char* path_to_file = file_record->getFieldByName("path");
		const char* filename = file_record->getFieldByName("filename");
		const char* size_str = file_record->getFieldByName("file_size");
		const char* encoding = file_record->getFieldByName("content_encoding");

		if (!path_to_file || !filename) {
			ErrorHandler::getInstance().onError("Passed record doesn't have all required fields", "ParserDLL error: \"Export cache file error\"");
			return false;
		}

		//set full path
		std::string full_path_str;
		if (output_path != nullptr && output_path[0] != '\0') {
			full_path_str = output_path;
			full_path_str.append("/");
		}
		full_path_str.append(filename);

		//set content length
		size_t converted_size = -1;
		if (size_str) {
			char* end;
			converted_size = std::strtoul(size_str, &end, 10);
		}

		//check encoding
		bool gzip_enc = ( encoding != nullptr && !strcmp(encoding, "gzip") );

		//try to open cache file
		std::ifstream file(path_to_file, std::ios::ate | std::ifstream::binary);
		if (!file.is_open()) {
			//handle error
			std::string error = "Couldn't open cache file:\n";
			error.append(path_to_file);
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Export cache file error\"");
			return false;
		}

		//read file to buffer
		size_t fsz = static_cast<size_t>(file.tellg());
		file.seekg(0, file.beg);
		if (converted_size < fsz) fsz = converted_size;

		std::vector<char> buffer(fsz);

		if (!file.read(buffer.data(), fsz)) {
			file.close();
			//handle error
			std::string error = "Error occured while reading file:\n";
			error.append(output_path);
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Export cache file error\"");
			return false;
		}

		file.close();

		//decompress file
		std::string decompressed;
		if (gzip_enc) {
			decompressed = decompressGzip(buffer);
		}

		//check if file already exists
		for (int i = 1; std::ifstream(full_path_str, std::ios::in); ++i) {
			//change name
			auto pos = full_path_str.find_last_of('.');
			assert(pos != std::string::npos);
			std::string ins = std::string(" (") + std::to_string(i) + std::string(")");
			full_path_str.insert(pos, ins);
		}

		//create file
		std::ofstream export_file(full_path_str, std::ios::out | std::ios::binary);
		if (!export_file.is_open()) {
			//handle error
			std::string error = "Couldn't create file:\n";
			error.append(full_path_str);
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Export cache file error\"");
			return false;
		}

		//write data to file
		bool writeSuccess = false;

		if (!decompressed.empty()) {	//data was compressed
			writeSuccess = !!export_file.write(decompressed.c_str(), decompressed.size());
		}
		else {
			writeSuccess = !!export_file.write(buffer.data(), buffer.size());
		}

		if (!writeSuccess) {
			export_file.close();
			//handle error
			std::string error = "Error occured while writing file:\n";
			error.append(full_path_str);
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Export cache file error\"");
			return false;
		}

		export_file.close();

		return true;
	}

	//methods

	std::string DataExporterImpl::createExportPtree(IRecordsStream* rstream, boost::property_tree::ptree& pt) const
	{
		using boost::property_tree::ptree;

		std::ostringstream filename;
		ptree pt_records;

		size_t maxsz = rstream->getNumberOfRecords();
		size_t field_count = rstream->getNumberOfFields();
		
		//1. write profile name
		const char* pname = _storage_ref.getProfileName(rstream->getStreamProfile());
		pt.put("profile", pname);

		//2. write current date
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);
		pt.put("creationDate", std::put_time(&tm, "%d.%m.%Y %H:%M:%S"));

		//3. write records type
		std::string rtype;
		switch (rstream->getStreamType())
		{
			case ERecordTypes::CACHEFILES:	rtype = "Cache"; break;
			case ERecordTypes::HISTORY:		rtype = "History"; break;
			case ERecordTypes::BOOKMARKS:	rtype = "Bookmarks"; break;
			case ERecordTypes::LOGINS:		rtype = "Logins"; break;
		}
		pt.put("recordsType", rtype);

		//4. write to records ptree
		for (size_t i = 0; i < maxsz; ++i) {
			ptree pt_record;

			for (size_t fieldnum = 0; fieldnum < field_count; ++fieldnum) {				
				pt_record.put(rstream->getFieldName(fieldnum), rstream->getRecordByIndex(i)->getFieldValue(fieldnum));
			}

			pt_records.push_back(std::make_pair("", pt_record));
		}

		pt.add_child("records", pt_records);

		//generate export file name
		filename << pname << "_" << rtype << "_" << std::put_time(&tm, "%Y%m%d_%H%M%S") << ".json";

		return filename.str();
	}


	bool DataExporterImpl::createMD5fromFile(const char* filename) const
	{
		if ((filename != nullptr && filename[0] != '\0') == false) {
			//handle error
			ErrorHandler::getInstance().onError("Passed file name is empty", "ParserDLL error: \"Generate MD5 error\"");
			return false;
		}

		//try to open file
		std::ifstream file(filename, std::ios::ate | std::ios::binary);
		if (!file.is_open()) {
			//handle error
			std::string error = "Couldn't open file:\n";
			error.append(filename);
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Generate MD5 error\"");
			return false;
		}

		//read file to buffer
		size_t fsz = static_cast<size_t>(file.tellg());
		file.seekg(0, file.beg);

		std::vector<char> buffer(fsz);
		if (!file.read(buffer.data(), fsz)) {
			file.close();
			//handle error
			std::string error = "Error occured while reading file:\n";
			error.append(filename);
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Generate MD5 error\"");
			return false;
		}
		
		file.close();

		//calculate MD5 from file data
		std::string fileHashStr = calculateMD5(reinterpret_cast<BYTE*>(buffer.data()), buffer.size());
		if (fileHashStr.empty()) {
			return false;
		}
		
		//create md5 file
		std::string md5filename = filename;
		md5filename.append(".md5");

		std::ofstream md5file(md5filename, std::ios::out);
		if (!md5file.is_open()) {
			//handle error
			std::string error = "Couldn't create MD5 file:\n";
			error.append(md5filename);
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Create file error\"");
			return false;
		}

		//write computed hash to file
		md5file << fileHashStr;
		md5file << " *";

		//write file name w/o path
		std::string fn = filename;
		std::smatch m;
		std::regex fn_rgx("(?:[-[:w:]]+\\.)+[[:w:]]+");
		std::regex_search(fn, m, fn_rgx);

		md5file << m[0].str();
		md5file.close();
		

		return true;
	}


	std::string DataExporterImpl::calculateMD5(const unsigned char* data, size_t data_size) const
	{
		HCRYPTPROV hProv = NULL;
		HCRYPTPROV hHash = NULL;
		DWORD cbHashSize = 0;
		DWORD dwCount = sizeof(DWORD);
		
		std::vector<BYTE> buffer;
		std::ostringstream oss;


		//initialization
		if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
			//handle error
			std::string error = "CryptAcquireContext failed: " + std::to_string(GetLastError());
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Generate MD5 error\"");
			return "";
		}
			
		if (!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash)) {
			CryptReleaseContext(hProv, 0);
			//handle error
			std::string error = "CryptCreateHash failed: " + std::to_string(GetLastError());
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Generate MD5 error\"");
			return "";
		}

		//create hash from data
		if (!CryptHashData(hHash, (BYTE*)data, data_size, 0)) {
			CryptDestroyHash(hHash);
			CryptReleaseContext(hProv, 0);
			//handle error
			std::string error = "CryptCreateHash failed: " + std::to_string(GetLastError());
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Generate MD5 error\"");
			return "";
		}
		
		//get required size for hash
		if(!CryptGetHashParam(hHash, HP_HASHSIZE, (BYTE*)&cbHashSize, &dwCount, 0)) {
			CryptDestroyHash(hHash);
			CryptReleaseContext(hProv, 0);
			//handle error
			std::string error = "CryptGetHashParam failed: " + std::to_string(GetLastError());
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Generate MD5 error\"");
			return "";
		}
		
		//reserve buffer for hash
		buffer.resize(cbHashSize);

		//write hash to buffer
		if (!CryptGetHashParam(hHash, HP_HASHVAL, &buffer[0], &cbHashSize, 0)) {
			CryptDestroyHash(hHash);
			CryptReleaseContext(hProv, 0);
			//handle error
			std::string error = "CryptGetHashParam failed: " + std::to_string(GetLastError());
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Generate MD5 error\"");
			return "";
		}
			
		//output hash to stream
		for (auto iter = buffer.begin(); iter != buffer.end(); ++iter) {
			oss.fill('0');
			oss.width(2);
			oss << std::hex << static_cast<int>(*iter);
		}
		
		//cleanup
		CryptDestroyHash(hHash);
		CryptReleaseContext(hProv, 0);

		return oss.str();
	}


	std::string DataExporterImpl::decompressGzip(const std::vector<char>& buffer) const
	{
		using namespace boost::iostreams;

		std::string decompressed;
		filtering_ostream out;

		try {
			out.push(gzip_decompressor());
			out.push(boost::iostreams::back_inserter(decompressed));
			out.write(buffer.data(), buffer.size());
			close(out);
		}
		catch (const std::exception& e) {
			//handle error
			std::string error = "File decompress error:\n";
			error.append(e.what());
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Export cache file error\"");
			decompressed.clear();
		}

		return decompressed;
	}

}