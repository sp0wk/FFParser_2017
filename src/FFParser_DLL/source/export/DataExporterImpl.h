#pragma once

#include "include/IDataExporter.h"
#include "include/IStorageFactory.h"

#include <vector>
#include <string>
#include <boost/property_tree/ptree.hpp>



namespace FFParser {

	class DataExporterImpl : public IDataExporter
	{
	public:
		//ctors and dtor
		DataExporterImpl(IStorageFactory& storage);
		virtual ~DataExporterImpl() = default;

		//methods
		virtual bool CALL exportRecords(IRecordsStream* rstream, const char* output_path, bool createMD5) const override;
		virtual bool CALL exportCacheFile(IRecord* file_record, const char* output_path) const override;

	private:
		IStorageFactory& _storage_ref;
		
		//methods
		std::string createExportPtree(IRecordsStream* rstream, boost::property_tree::ptree& pt) const;
		bool createMD5fromFile(const char* filename) const;
		std::string calculateMD5(const unsigned char* data, size_t data_size) const;
		std::string decompressGzip(const std::vector<char>& buffer) const;
	};

}