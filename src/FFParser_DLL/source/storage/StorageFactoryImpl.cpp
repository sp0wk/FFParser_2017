#include "StorageFactoryImpl.h"

#include "accessors/ProfileInfo.h"

#include <algorithm>



namespace FFParser {

	StorageFactoryImpl& StorageFactoryImpl::getInstance()
	{
		static StorageFactoryImpl s_instance;
		return s_instance;
	}

	//ctor
	StorageFactoryImpl::StorageFactoryImpl() : 
		_helper_ref(HelperFacade::getInstance()),
		_exporter(new DataExporterImpl(*this))
	{
	}


	// Interface methods


	size_t CALL StorageFactoryImpl::getNumberOfProfiles() const
	{
		return _helper_ref.getNumberOfProfiles();
	}


	const char* CALL StorageFactoryImpl::getProfileName(size_t profile) const
	{
		if (profile < _helper_ref.getNumberOfProfiles()) {
			return _helper_ref.getProfile(profile).name.c_str();
		}
		return nullptr;
	}


	const char* CALL StorageFactoryImpl::getPathToProfile(size_t profile) const
	{
		if (profile < _helper_ref.getNumberOfProfiles()) {
			return _helper_ref.getProfile(profile).path.c_str();
		}
		return nullptr;
	}


	const char* CALL StorageFactoryImpl::getPathToCache(size_t profile) const
	{
		if (profile < _helper_ref.getNumberOfProfiles()) {
			return _helper_ref.getProfile(profile).cache_path.c_str();
		}
		return nullptr;
	}


	IRecordsStream* CALL StorageFactoryImpl::createRecordsStream(ERecordTypes type, size_t profile)
	{
		RecordsStreamImpl* newstream = nullptr;

		if (profile < _helper_ref.getNumberOfProfiles()) {
			_storage.emplace_back(new RecordsStreamImpl(profile, type));	//save to storage
			newstream = _storage.back().get();
		}
		
		return newstream;
	}


	void CALL StorageFactoryImpl::freeRecordsStream(IRecordsStream* &record)
	{
		_storage.remove_if( [&](std::unique_ptr<RecordsStreamImpl>& elem) { return record == elem.get(); } );
		record = nullptr;
	}

	IDataExporter* CALL StorageFactoryImpl::getDataExporter() const
	{
		return _exporter.get();
	}

}