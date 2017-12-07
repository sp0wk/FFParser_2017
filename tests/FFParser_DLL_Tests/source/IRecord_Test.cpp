#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "Config_defines.h"

#include "include/IRecordsStream.h"
#include "include/IRecord.h"

using namespace FFParser;


extern IRecordsStream* records;
IRecord* field  = records->getRecordByIndex(0);



//virtual const char* CALL getFieldByName(const char* name) const = 0;
BOOST_AUTO_TEST_CASE(getFieldByNameTest)
{
	BOOST_CHECK(field->getFieldByName("id") != nullptr);					//found
	BOOST_CHECK(field->getFieldByName("___SOME_TEXT____") == nullptr);		//not found
}


//virtual const char* CALL getFieldValue(size_t index) const = 0;
BOOST_AUTO_TEST_CASE(getFieldValueTest)
{
	BOOST_CHECK(field->getFieldValue(0) != nullptr);		//found
	BOOST_CHECK(field->getFieldValue(-1) == nullptr);		//index out of range
}


//virtual size_t CALL getFieldSize(size_t index) const = 0;
BOOST_AUTO_TEST_CASE(getFieldSizeTest)
{
	BOOST_CHECK(field->getFieldSize(0) > 0);
	BOOST_CHECK(field->getFieldSize(-1) == 0);
}


//virtual size_t CALL findText(const char* text) const = 0;
BOOST_AUTO_TEST_CASE(findTextTest)
{
	BOOST_CHECK(field->findText("qt") != 0);					//found
	BOOST_CHECK(field->findText("__SOME_TEXT___") == -1);		//not found
}

