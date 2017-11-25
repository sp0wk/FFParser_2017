#pragma once

#include <Windows.h>
#include <Wincrypt.h>

#include <string>
#include <memory>
#include <functional>
#include <boost/property_tree/ptree.hpp>

#include "FileParserBase.h"

#pragma comment(lib, "crypt32.lib")


namespace FFParser {

	class LoginsParser final : public FileParserBase
	{
	public:
		LoginsParser(const std::shared_ptr<IFileAccessor>& fa);
		virtual ~LoginsParser() = default;

		//methods
		virtual size_t getTotalRecords(size_t profile) override;
		virtual size_t parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number) override;

	private:
		boost::property_tree::ptree _pt;

	#pragma region DecryptLib

		enum SECItemType
		{
			siBuffer = 0,
			siClearDataBuffer = 1,
			siCipherDataBuffer = 2,
			siDERCertBuffer = 3,
			siEncodedCertBuffer = 4,
			siDERNameBuffer = 5,
			siEncodedNameBuffer = 6,
			siAsciiNameString = 7,
			siAsciiString = 8,
			siDEROID = 9,
			siUnsignedInteger = 10,
			siUTCTime = 11,
			siGeneralizedTime = 12
		};

		struct SECItem
		{
			SECItemType type;
			unsigned char *data;
			unsigned int len;
		};

		enum SECStatus
		{
			SECWouldBlock = -2,
			SECFailure = -1,
			SECSuccess = 0
		};


		typedef struct PK11SlotInfoStr PK11SlotInfo;
		typedef SECStatus (*NSS_Init) (const char* configdir);
		typedef SECStatus (*NSS_Shutdown) (void);
		typedef PK11SlotInfo* (*PK11_GetInternalKeySlot) (void);
		typedef void (*PK11_FreeSlot) (PK11SlotInfo* slot);
		typedef SECStatus (*PK11_Authenticate) (PK11SlotInfo* slot, int loadCerts, void* wincx);
		typedef SECStatus (*PK11SDR_Decrypt) (SECItem* data, SECItem* result, void* cx);


		NSS_Init                NSSInit;
		NSS_Shutdown            NSSShutdown;
		PK11_GetInternalKeySlot PK11GetInternalKeySlot;
		PK11_FreeSlot           PK11FreeSlot;
		PK11_Authenticate       PK11Authenticate;
		PK11SDR_Decrypt         PK11SDRDecrypt;

		//decrypt libs
		using LibFreeFunc = std::function<void(HMODULE)>;
		using LibGuard = std::unique_ptr<std::remove_pointer_t<HMODULE>, LibFreeFunc>;

		LibFreeFunc _libFreeFunc;
		LibGuard _mozgluedll;
		LibGuard _libnss;

	#pragma endregion DecryptLib

		//methods
		std::string DecryptString(const std::string& encrypted) const;
		void loadLibraries();
	};

}