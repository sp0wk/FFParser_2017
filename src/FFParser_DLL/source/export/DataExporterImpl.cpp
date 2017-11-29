#include "DataExporterImpl.h"

#include "helpers/ErrorHandler.h"

#include <chrono>
#include <iomanip>

#include <boost/property_tree/json_parser.hpp>


namespace FFParser {

	//ctor
	DataExporterImpl::DataExporterImpl(IStorageFactory& storage) :
		_storage_ref(storage)
	{
	}


	//Interface methods

	bool CALL DataExporterImpl::exportRecords(IRecordsStream* rstream, const char* filename, bool bCreateMD5) const
	{
		using boost::property_tree::ptree;
		using boost::property_tree::write_json;
		
		ptree pt;

		if (rstream == nullptr || filename == nullptr) {
			ErrorHandler::getInstance().onError("Passed parameter is NULL", "ParserDLL error: \"Export records error\"");
			return false;
		}

		//try to create export file
		std::ofstream file(filename, std::ios::out);
		if (!file.is_open()) {
			//handle error
			std::string error = "Couldn't create export file:\n";
			error.append(filename);
			ErrorHandler::getInstance().onError(error.c_str(), "ParserDLL error: \"Create file error\"");

			return false;
		}
		
		//create json ptree
		createExportPtree(rstream, pt);

		//write ptree to export file
		try {
			write_json(file, pt, true);
		}
		catch (const std::exception& ex) {
			//handle error
			ErrorHandler::getInstance().onError(ex.what(), "ParserDLL error: \"File write error\"");
			return false;
		}

		file.close();

		//create md5 if needed
		if (bCreateMD5) {
			createMD5fromFile(filename);
		}

		return true;
	}


	//methods

	void DataExporterImpl::createExportPtree(IRecordsStream* rstream, boost::property_tree::ptree& pt) const
	{
		using boost::property_tree::ptree;

		ptree pt_records;

		size_t maxsz = rstream->getNumberOfRecords();
		size_t field_count = rstream->getNumberOfFields();
		
		//1. write profile name
		pt.put("profile", _storage_ref.getProfileName(rstream->getStreamProfile()));

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
	}


	bool DataExporterImpl::createMD5fromFile(const char* filename) const
	{
		//TODO:
		//open file, calculate md5 and write to file "filename".md5
		return true;
	}


	std::string DataExporterImpl::calculateMD5(unsigned char* data, size_t data_size) const
	{
		//TODO:
		//calculate data's md5
		return "";
	}

}