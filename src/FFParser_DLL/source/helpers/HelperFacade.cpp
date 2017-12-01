#include "HelperFacade.h"



namespace FFParser {

	//ctor
	HelperFacade::HelperFacade() :
		_file_accessor(new FileAccessorImpl),
		_db_accessor(new DBAccessorImpl)
	{
		//create parsers for every profile
		size_t profiles = _file_accessor->getNumberOfProfiles();
		_parsers.resize(profiles);

		for (size_t i = 0; i < profiles; ++i) {
			_parsers[i].resize(size_t(ERecordTypes::NUMBER_OF_TYPES));
			_parsers[i][size_t(ERecordTypes::CACHEFILES)].reset(new CacheFilesParser(_file_accessor));
			_parsers[i][size_t(ERecordTypes::HISTORY)].reset(new HistoryParser(_file_accessor, _db_accessor));
			_parsers[i][size_t(ERecordTypes::BOOKMARKS)].reset(new BookmarksParser(_file_accessor, _db_accessor));
			_parsers[i][size_t(ERecordTypes::LOGINS)].reset(new LoginsParser(_file_accessor));
		}
	}

	//methods

	size_t HelperFacade::getNumberOfProfiles() const
	{
		return _file_accessor->getNumberOfProfiles();
	}


	const ProfileInfo& HelperFacade::getProfile(size_t profile) const
	{
		return _file_accessor->getProfile(profile);
	}


	void HelperFacade::getFieldNames(std::vector<std::string>& list, ERecordTypes type) const
	{
		_parsers[0][size_t(type)]->getFieldNames(list);
	}


	size_t HelperFacade::getTotalRecords(ERecordTypes type, size_t profile)
	{
		return _parsers[profile][size_t(type)]->getTotalRecords(profile);
	}


	size_t HelperFacade::parseRecords(ERecordTypes type, size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number)
	{
		return _parsers[profile][size_t(type)]->parse(profile, output, from, number);
	}

}