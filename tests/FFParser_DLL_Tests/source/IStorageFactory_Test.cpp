#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "include/IStorageFactory.h"
#include "include/IRecordsStream.h"

using namespace FFParser;


extern IStorageFactory* storage;


//setup
#define g_ConfMode "Debug"
#define g_Arch "Win32"
#define g_profilePath "C:/Users/All/Source/Repos/FirefoxParser_2017/bin/"g_ConfMode"/"g_Arch"/TESTDATA"


//TEST CASES


//virtual size_t CALL getNumberOfProfiles() const = 0;
BOOST_AUTO_TEST_CASE(getNumberOfProfilesTest)
{
	BOOST_CHECK(storage->getNumberOfProfiles() == 1);
}


//virtual const char* CALL getProfileName(size_t profile = 0) const = 0;
BOOST_AUTO_TEST_CASE(getProfileNameTest)
{
	BOOST_CHECK_EQUAL(storage->getProfileName(0), "none");
	BOOST_CHECK(storage->getProfileName(2) == nullptr);
	BOOST_CHECK(storage->getProfileName(-1) == nullptr);
}


//virtual const char* CALL getPathToProfile(size_t profile = 0) const = 0;
BOOST_AUTO_TEST_CASE(getPathToProfileTest)
{
	BOOST_CHECK_EQUAL(storage->getPathToProfile(0), g_profilePath);
	BOOST_CHECK(storage->getPathToProfile(1) == nullptr);
}


//virtual const char* CALL getPathToCache(size_t profile = 0) const = 0;
BOOST_AUTO_TEST_CASE(getPathToCacheTest)
{
	BOOST_CHECK_EQUAL(storage->getPathToCache(0), g_profilePath"/cache");
	BOOST_CHECK(storage->getPathToProfile(1) == nullptr);
}


//virtual IRecordsStream* CALL createRecordsStream(ERecordTypes type, size_t profile = 0) = 0;
BOOST_AUTO_TEST_CASE(createRecordsStreamTest)
{
	const char* prof0 = storage->getPathToProfile(0);
	IRecordsStream* hist = storage->createRecordsStream(ERecordTypes::HISTORY, 0);
	IRecordsStream* book = storage->createRecordsStream(ERecordTypes::BOOKMARKS, 0);
	IRecordsStream* logins = storage->createRecordsStream(ERecordTypes::LOGINS, 2);
	IRecordsStream* cache = storage->createRecordsStream(ERecordTypes::HISTORY, -1);

	BOOST_CHECK(hist != nullptr);
	BOOST_CHECK(book != nullptr);
	BOOST_CHECK(logins == nullptr);
	BOOST_CHECK(cache == nullptr);

	delete hist;
	delete book;
	delete logins;
	delete cache;
}