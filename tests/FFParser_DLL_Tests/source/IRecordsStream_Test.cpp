#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "include/IStorageFactory.h"
#include "include/IRecordsStream.h"
#include "include/IRecord.h"

using namespace FFParser;


extern IStorageFactory* storage;


//setup
#define g_recordType ERecordTypes::HISTORY
#define g_profile 0
#define g_totalRecords 4512



//TEST CASES


//virtual size_t CALL getTotalRecords() const = 0;
BOOST_AUTO_TEST_CASE(getTotalRecordsTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->getTotalRecords() == g_totalRecords);
}


//virtual size_t CALL loadNextRecords(size_t number = -1) = 0;
BOOST_AUTO_TEST_CASE(loadNextRecordsTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->loadNextRecords(0) == 0);
	BOOST_CHECK(records->loadNextRecords(1) == 1);
	BOOST_CHECK(records->loadNextRecords(2) == 2);
	BOOST_CHECK(records->loadNextRecords(15) > 0);

	size_t loaded = records->getNumberOfRecords();
	BOOST_CHECK(records->loadNextRecords() == records->getTotalRecords() - loaded);
}


//virtual size_t CALL loadRecords(size_t from, size_t number) = 0;
BOOST_AUTO_TEST_CASE(loadRecordsRangeTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->loadRecords(0, 0) == 0);
	BOOST_CHECK(records->loadRecords(0, 1) == 1);
	BOOST_CHECK(records->loadRecords(10, 0) == 0);
	BOOST_CHECK(records->loadRecords(records->getTotalRecords(), 1) == 0);
	BOOST_CHECK(records->loadRecords(records->getTotalRecords() - 1, 1) == 1);

	size_t c1 = records->loadRecords(10, 50);
	BOOST_CHECK(c1 > 1 && c1 <= 51);

	records->loadRecords(c1 + 2, -1);	//load all
	BOOST_CHECK_EQUAL(records->getNumberOfRecords(), records->getTotalRecords());
}


//virtual size_t CALL getNumberOfRecords() const = 0;
BOOST_AUTO_TEST_CASE(getNumberOfRecordsTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->getNumberOfRecords() == 0);

	records->loadNextRecords();	//load all
	/*size_t count = 0;
	while (records->getNextRecord()) ++count;*/

	BOOST_CHECK(records->getNumberOfRecords() == records->getTotalRecords());
}


//virtual size_t CALL currentRecord() const = 0;
BOOST_AUTO_TEST_CASE(currentRecordTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->currentRecord() == -1);

	records->loadNextRecords(1);
	BOOST_CHECK(records->currentRecord() == -1);
	
	records->getNextRecord();
	BOOST_CHECK(records->currentRecord() == 0);
}


//virtual IRecord* CALL getPrevRecord() = 0;
BOOST_AUTO_TEST_CASE(getPrevRecordTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->getPrevRecord() == nullptr);
	
	records->loadNextRecords(2);
	BOOST_CHECK(records->getPrevRecord() == nullptr);

	records->setCurrentRecord(0);
	BOOST_CHECK(records->getPrevRecord() == nullptr);

	records->setCurrentRecord(1);
	BOOST_CHECK(records->getPrevRecord() != nullptr);
}


//virtual IRecord* CALL getNextRecord() = 0;
BOOST_AUTO_TEST_CASE(getNextRecordTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->getNextRecord() == nullptr);

	records->loadNextRecords(2);
	BOOST_CHECK(records->getNextRecord() != nullptr);
	BOOST_CHECK(records->getNextRecord() != nullptr);

	records->loadNextRecords();
	records->setCurrentRecord(records->getNumberOfRecords() - 1);
	BOOST_CHECK(records->getNextRecord() == nullptr);
}


//virtual IRecord* CALL getRecordByIndex(size_t index) = 0;
BOOST_AUTO_TEST_CASE(getRecordByIndexTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->getRecordByIndex(-1) == nullptr);
	BOOST_CHECK(records->getRecordByIndex(0) == nullptr);
	records->loadNextRecords(5);
	BOOST_CHECK(records->getRecordByIndex(0) != nullptr);
	BOOST_CHECK(records->getRecordByIndex(4) != nullptr);
	BOOST_CHECK(records->getRecordByIndex(5) == nullptr);
}


//virtual size_t CALL getStreamProfile() const = 0;
BOOST_AUTO_TEST_CASE(getStreamProfileTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->getStreamProfile() == 0);
}


//virtual ERecordTypes CALL getStreamType() const = 0;
BOOST_AUTO_TEST_CASE(getStreamTypeTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->getStreamType() == ERecordTypes::HISTORY);
}


//virtual bool CALL setCurrentRecord(size_t index) = 0;
BOOST_AUTO_TEST_CASE(setCurrentRecordTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->setCurrentRecord(-1) == false);
	BOOST_CHECK(records->setCurrentRecord(0) == false);

	records->loadNextRecords(6);
	BOOST_CHECK(records->setCurrentRecord(6) == false);
	BOOST_CHECK(records->setCurrentRecord(5) == true);
}


//virtual size_t CALL searchPrevRecord(const char* text) = 0;
BOOST_AUTO_TEST_CASE(searchPrevRecordTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));
	records->loadNextRecords(50);

	const char* searchtext = "qt";

	BOOST_CHECK(records->searchPrevRecord(searchtext) == -1);		//out of range
	records->setCurrentRecord(10);
	BOOST_CHECK(records->searchPrevRecord(searchtext) == -1);		//not found
	records->setCurrentRecord(40);
	BOOST_CHECK(records->searchPrevRecord(searchtext) != -1);		//found
}


//virtual size_t CALL searchNextRecord(const char* text) = 0;
BOOST_AUTO_TEST_CASE(searchNextRecordTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));
	records->loadNextRecords(50);

	const char* searchtext = "qt";

	records->setCurrentRecord(records->getNumberOfRecords() - 1);
	BOOST_CHECK(records->searchNextRecord(searchtext) == -1);		//out of range
	records->setCurrentRecord(45);
	BOOST_CHECK(records->searchNextRecord(searchtext) == -1);		//not found
	records->setCurrentRecord(0);
	BOOST_CHECK(records->searchNextRecord(searchtext) != -1);		//found
}


//virtual size_t CALL getNumberOfFields() const = 0;
BOOST_AUTO_TEST_CASE(getNumberOfFieldsTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK(records->getNumberOfFields() > 0);
}


//virtual const char* CALL getFieldName(size_t index) const = 0;
BOOST_AUTO_TEST_CASE(getFieldNameTest)
{
	std::unique_ptr<IRecordsStream> records(storage->createRecordsStream(g_recordType, g_profile));

	BOOST_CHECK_EQUAL(records->getFieldName(0), "id");
	BOOST_CHECK(records->getFieldName(-1) == nullptr);
}
