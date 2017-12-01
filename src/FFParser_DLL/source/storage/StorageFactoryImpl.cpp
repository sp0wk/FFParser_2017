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
		_exporter(new DataExporterImpl(*this))
	{
	}


	// Interface methods


	size_t CALL StorageFactoryImpl::getNumberOfProfiles() const
	{
		return _helper.getNumberOfProfiles();
	}


	const char* CALL StorageFactoryImpl::getProfileName(size_t profile) const
	{
		if (profile < _helper.getNumberOfProfiles()) {
			return _helper.getProfile(profile).name.c_str();
		}
		return nullptr;
	}


	const char* CALL StorageFactoryImpl::getPathToProfile(size_t profile) const
	{
		if (profile < _helper.getNumberOfProfiles()) {
			return _helper.getProfile(profile).path.c_str();
		}
		return nullptr;
	}


	const char* CALL StorageFactoryImpl::getPathToCache(size_t profile) const
	{
		if (profile < _helper.getNumberOfProfiles()) {
			return _helper.getProfile(profile).cache_path.c_str();
		}
		return nullptr;
	}


	IRecordsStream* CALL StorageFactoryImpl::createRecordsStream(ERecordTypes type, size_t profile)
	{
		if (profile < _helper.getNumberOfProfiles()) {
			return new RecordsStreamImpl(_helper, profile, type);
		}
		return nullptr;
	}


	IDataExporter* CALL StorageFactoryImpl::getDataExporter() const
	{
		return _exporter.get();
	}

}