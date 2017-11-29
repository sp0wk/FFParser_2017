#pragma once

#include <memory>
#include <vector>
#include <list>

#include "include/IStorageFactory.h"

#include "records/RecordsStreamImpl.h"
#include "helpers/HelperFacade.h"
#include "export/DataExporterImpl.h"


// Singleton Factory

namespace FFParser {

	class StorageFactoryImpl final : public IStorageFactory
	{
	public:
		static StorageFactoryImpl& getInstance();

		//Interface methods
		virtual size_t CALL getNumberOfProfiles() const override;
		virtual const char* CALL getProfileName(size_t profile) const override;
		virtual const char* CALL getPathToProfile(size_t profile) const override;
		virtual const char* CALL getPathToCache(size_t profile = 0) const override;
		virtual IRecordsStream* CALL createRecordsStream(ERecordTypes type, size_t profile) override;
		virtual void CALL freeRecordsStream(IRecordsStream* &record) override;
		virtual IDataExporter* CALL getDataExporter() const override;

	private:
		HelperFacade& _helper_ref;

		//list of created streams
		std::list<std::unique_ptr<RecordsStreamImpl>> _storage;

		//exporter
		std::unique_ptr<DataExporterImpl> _exporter;

		//private ctor and dtor
		StorageFactoryImpl();
		~StorageFactoryImpl() = default;
	};

}