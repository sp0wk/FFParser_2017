#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "Config_defines.h"

#include "include/IStorageFactory.h"
#include "include/IRecordsStream.h"
#include "include/IRecord.h"

using namespace FFParser;


extern IStorageFactory* storage;
extern IRecordsStream* records;


//virtual size_t CALL getTotalRecords() const = 0;
BOOST_AUTO_TEST_CASE(getTotalRecordsTest)
{
	BOOST_CHECK(records->getTotalRecords() == 2141);
}


//virtual size_t CALL loadNextRecords(size_t number = -1) = 0;
BOOST_AUTO_TEST_CASE(loadNextRecordsTest)
{
	BOOST_CHECK(records->loadNextRecords(10) == 10);
}


//virtual size_t CALL loadRecords(size_t from, size_t number) = 0;
BOOST_AUTO_TEST_CASE(loadRecordsRangeTest)
{
	BOOST_CHECK(records->loadRecords(10, 50) == 40);
}


//virtual size_t CALL getNumberOfRecords() const = 0;
BOOST_AUTO_TEST_CASE(getNumberOfRecordsTest)
{
	BOOST_CHECK(records->getNumberOfRecords() == 10+40);
}


//virtual size_t CALL currentRecord() const = 0;
BOOST_AUTO_TEST_CASE(currentRecordTest)
{
	BOOST_CHECK(records->currentRecord() == -1);
	records->getNextRecord();
	BOOST_CHECK(records->currentRecord() == 0);
}


//virtual IRecord* CALL getPrevRecord() = 0;
BOOST_AUTO_TEST_CASE(getPrevRecordTest)
{
	BOOST_CHECK(records->getPrevRecord() == nullptr);
	records->setCurrentRecord(1);
	BOOST_CHECK(records->getPrevRecord() != nullptr);
}


//virtual IRecord* CALL getNextRecord() = 0;
BOOST_AUTO_TEST_CASE(getNextRecordTest)
{
	records->setCurrentRecord(-1);
	BOOST_CHECK(records->getPrevRecord() == nullptr);
	records->setCurrentRecord(0);
	BOOST_CHECK(records->getPrevRecord() != nullptr);
}


//virtual IRecord* CALL getRecordByIndex(size_t index) = 0;
BOOST_AUTO_TEST_CASE(getRecordByIndexTest)
{
	BOOST_CHECK(records->getRecordByIndex(-1) == nullptr);
	BOOST_CHECK(records->getRecordByIndex(0) != nullptr);
	BOOST_CHECK(records->getRecordByIndex(5) != nullptr);
}


//virtual size_t CALL getStreamProfile() const = 0;
BOOST_AUTO_TEST_CASE(getStreamProfileTest)
{
	BOOST_CHECK(records->getStreamProfile() == 0);
}


//virtual ERecordTypes CALL getStreamType() const = 0;
BOOST_AUTO_TEST_CASE(getStreamTypeTest)
{
	BOOST_CHECK(records->getStreamType() == ERecordTypes::HISTORY);
}


//virtual bool CALL setCurrentRecord(size_t index) = 0;
BOOST_AUTO_TEST_CASE(setCurrentRecordTest)
{
	size_t last = records->currentRecord();
	BOOST_CHECK(records->setCurrentRecord(-1) == false);
	BOOST_CHECK(records->currentRecord() == last);
	BOOST_CHECK(records->setCurrentRecord(5) == true);
	BOOST_CHECK(records->currentRecord() == 5);
}


//virtual size_t CALL searchPrevRecord(const char* text) = 0;
BOOST_AUTO_TEST_CASE(searchPrevRecordTest)
{
	const char* searchtext = "qt";
	BOOST_CHECK(records->setCurrentRecord(0) == true);
	BOOST_CHECK(records->searchPrevRecord(searchtext) == -1);		//not found
	BOOST_CHECK(records->setCurrentRecord(50) == true);
	BOOST_CHECK(records->searchPrevRecord(searchtext) != -1);		//found
}


//virtual size_t CALL searchNextRecord(const char* text) = 0;
BOOST_AUTO_TEST_CASE(searchNextRecordTest)
{
	const char* searchtext = "qt";
	BOOST_CHECK(records->setCurrentRecord(records->getNumberOfRecords()) == true);
	BOOST_CHECK(records->searchNextRecord(searchtext) == -1);		//not found
	BOOST_CHECK(records->setCurrentRecord(0) == true);
	BOOST_CHECK(records->searchNextRecord(searchtext) != -1);		//found
}


//virtual size_t CALL getNumberOfFields() const = 0;
BOOST_AUTO_TEST_CASE(getNumberOfFieldsTest)
{
	BOOST_CHECK(records->getNumberOfFields() > 0);
}


//virtual const char* CALL getFieldName(size_t index) const = 0;
BOOST_AUTO_TEST_CASE(getFieldNameTest)
{
	BOOST_CHECK_EQUAL(records->getFieldName(0), "id");
	BOOST_CHECK(records->getFieldName(-1) == nullptr);
}
