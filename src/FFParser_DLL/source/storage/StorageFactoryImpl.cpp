#include "StorageFactoryImpl.h"

#include <algorithm>


// StorageFactoryImpl

namespace FFParser {

	StorageFactoryImpl& StorageFactoryImpl::getInstance()
	{
		static StorageFactoryImpl s_instance;
		return s_instance;
	}

	//ctor
	StorageFactoryImpl::StorageFactoryImpl() : _helper_ref(HelperFacade::getInstance())
	{
		//load profiles
		_helper_ref.getProfileNames(_profiles);
	}


	// Interface methods


	size_t CALL StorageFactoryImpl::getNumberOfProfiles() const
	{
		return _profiles.size();
	}


	const char* CALL StorageFactoryImpl::getProfileNameByIndex(size_t index) const
	{
		return index < _profiles.size() ? _profiles[index].c_str() : nullptr;
	}


	IRecordsStream* CALL StorageFactoryImpl::createRecordsStream(ERecordTypes type, size_t profile)
	{
		IRecordsStream* newstream = nullptr;

		if (profile < _profiles.size()) {
			newstream = new RecordsStreamImpl(profile, type);
			_storage.push_back(std::unique_ptr<IRecordsStream>(newstream));		//save to storage
		}
		
		return newstream;
	}


	void CALL StorageFactoryImpl::freeRecordsStream(IRecordsStream* &record)
	{
		_storage.remove_if( [&](std::unique_ptr<IRecordsStream>& elem) { return record == elem.get(); } );
		record = nullptr;
	}

}