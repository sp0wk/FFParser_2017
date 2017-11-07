#include "HelperFacade.h"



namespace FFParser {

	HelperFacade& HelperFacade::getInstance()
	{
		static HelperFacade s_instance;
		return s_instance;
	}


	//ctor
	HelperFacade::HelperFacade() :
		_file_accessor(new FileAccessorImpl),
		_db_accessor(new DBAccessorImpl)
	{
		_parsers[size_t(ERecordTypes::CACHEFILES)].reset(new CacheFilesParser(_file_accessor));
		_parsers[size_t(ERecordTypes::HISTORY)].reset(new HistoryParser(_file_accessor, _db_accessor));
		_parsers[size_t(ERecordTypes::BOOKMARKS)].reset(new BookmarksParser(_file_accessor, _db_accessor));
		_parsers[size_t(ERecordTypes::LOGINS)].reset(new LoginsParser(_file_accessor));
	}

	//methods

	void HelperFacade::getProfileNames(std::vector<std::string>& list) const
	{
		_file_accessor->getProfiles(list);
	}


	std::string HelperFacade::getPathToProfile(size_t profile) const
	{
		return _file_accessor->getPathToResource(EResourcePaths::PROFILEPATH, profile);
	}


	void HelperFacade::getFieldNames(std::vector<std::string>& list, ERecordTypes type) const
	{
		_parsers[size_t(type)]->getFieldNames(list);
	}


	size_t HelperFacade::getTotalRecords(ERecordTypes type, size_t profile)
	{
		return _parsers[size_t(type)]->getTotalRecords(profile);
	}


	size_t HelperFacade::parseRecords(ERecordTypes type, size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number)
	{
		return _parsers[size_t(type)]->parse(profile, output, from, number);
	}

}