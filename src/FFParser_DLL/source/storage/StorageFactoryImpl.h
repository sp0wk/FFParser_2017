#pragma once

#include <memory>
#include <vector>
#include <list>

#include "include/IStorageFactory.h"

#include "../records/RecordsStreamImpl.h"
#include "../helpers/HelperFacade.h"



// Singleton Factory

namespace FFParser {

	class StorageFactoryImpl final : public IStorageFactory
	{
	private:
		HelperFacade& _helper_ref;

		std::vector<std::string> _profiles;

		//list of created streams
		std::list<std::unique_ptr<IRecordsStream>> _storage;

		//private ctor and dtor
		StorageFactoryImpl();
		~StorageFactoryImpl() = default;

	public:
		static StorageFactoryImpl& getInstance();

		//Interface methods
		virtual size_t getNumberOfProfiles() const override;
		virtual const char* getProfileNameByIndex(size_t index) const override;
		virtual IRecordsStream* createRecordsStream(ERecordTypes type, size_t profile) override;
		virtual void freeRecordsStream(IRecordsStream* &record) override;
	};

}