#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "Config_defines.h"

#include "include/IStorageFactory.h"
#include "include/IRecordsStream.h"

using namespace FFParser;


extern IStorageFactory* storage;


//TEST CASES


//virtual size_t CALL getNumberOfProfiles() const = 0;
BOOST_AUTO_TEST_CASE(getNumberOfProfilesTest)
{
	BOOST_CHECK(storage->getNumberOfProfiles() == 2);
}


//virtual const char* CALL getProfileName(size_t profile = 0) const = 0;
BOOST_AUTO_TEST_CASE(getProfileNameTest)
{
	BOOST_CHECK_EQUAL(storage->getProfileName(0), "default");
	BOOST_CHECK_EQUAL(storage->getProfileName(1), "sms");

	BOOST_CHECK(storage->getProfileName(2) == nullptr);
	BOOST_CHECK(storage->getProfileName(-1) == nullptr);
}


//virtual const char* CALL getPathToProfile(size_t profile = 0) const = 0;
BOOST_AUTO_TEST_CASE(getPathToProfileTest)
{
	const char* prof0 = storage->getPathToProfile(0);
	BOOST_CHECK(prof0 == g_profilesPath"csl5ggnd.default" || prof0 == g_appPath);

	const char* prof1 = storage->getPathToProfile(1);
	BOOST_CHECK(prof1 == "D:\\ffprofile\\profile2" || prof1 == nullptr);
}


//virtual const char* CALL getPathToCache(size_t profile = 0) const = 0;
BOOST_AUTO_TEST_CASE(getPathToCacheTest)
{
	const char* prof0_c = storage->getPathToProfile(0);
	BOOST_CHECK(strcmp(prof0_c, g_profilesPath"csl5ggnd.default\\cache2\\entries") == 0 || strcmp(prof0_c, g_appPath"\\cache") == 0);

	const char* prof1_c = storage->getPathToProfile(1);
	BOOST_CHECK(strcmp(prof1_c, "D:\\ffprofile\\profile2\\cache2\\entries") == 0 || prof1_c == nullptr);
}


//virtual IRecordsStream* CALL createRecordsStream(ERecordTypes type, size_t profile = 0) = 0;
BOOST_AUTO_TEST_CASE(createRecordsStreamTest)
{
	const char* prof0 = storage->getPathToProfile(0);
	IRecordsStream* hist = storage->createRecordsStream(ERecordTypes::HISTORY, 0);
	IRecordsStream* book = storage->createRecordsStream(ERecordTypes::BOOKMARKS, 1);
	IRecordsStream* logins = storage->createRecordsStream(ERecordTypes::LOGINS, 2);
	IRecordsStream* cache = storage->createRecordsStream(ERecordTypes::HISTORY, -1);

	BOOST_CHECK(hist != nullptr);
	BOOST_CHECK(book != nullptr);
	BOOST_CHECK(logins == nullptr);
	BOOST_CHECK(cache == nullptr);
}