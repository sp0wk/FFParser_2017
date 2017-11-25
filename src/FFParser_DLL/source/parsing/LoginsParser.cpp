#include "LoginsParser.h"

#include <Windows.h>	//for MessageBox

#include <boost/property_tree/json_parser.hpp>


namespace FFParser {

	//ctor
	LoginsParser::LoginsParser(const std::shared_ptr<IFileAccessor>& fa) : 
		FileParserBase(fa),
		_libFreeFunc( [](HMODULE lib) { if (lib != nullptr) FreeLibrary(lib); } ),
		_mozgluedll(nullptr, _libFreeFunc),
		_libnss(nullptr, _libFreeFunc)
	{
		//set field names
		_field_names.reserve(3);
		_field_names.push_back("hostname");
		_field_names.push_back("username");
		_field_names.push_back("password");

		//load decrypt libs
		loadLibraries();
	}


	//methods

	std::string LoginsParser::DecryptString(const std::string& encrypted) const
	{
		BYTE byteData[8096] {0};
		DWORD dwLength = 8096;
		PK11SlotInfo *slot = 0;
		SECStatus status;
		SECItem in, out;
		std::string result = "";

		if (CryptStringToBinaryA(encrypted.c_str(), encrypted.length(), CRYPT_STRING_BASE64, byteData, &dwLength, 0, 0)) {
			slot = (*PK11GetInternalKeySlot) ();
			
			if (slot != NULL){
				// see if we can authenticate
				status = PK11Authenticate(slot, true, NULL);

				if (status == SECSuccess){
					in.data = byteData;
					in.len = dwLength;
					out.data = 0;
					out.len = 0;
					status = (*PK11SDRDecrypt) (&in, &out, NULL);

					if (status == SECSuccess) {
						memcpy(byteData, out.data, out.len);
						byteData[out.len] = 0;
						result = std::string((char*)byteData);
					}
					else {
						//"Decryption failed";
					}
				}
				else {
					//"Authentication failed";
				}

				(*PK11FreeSlot) (slot);
			}
			else {
				//"Get Internal Slot failed";
			}
		}

		return result;
	}



	void LoginsParser::loadLibraries()
	{
		std::string installPath;

		//get path
		if (auto sh = _file_accessor_ref.lock()) {
			installPath = sh->getPathToResource(EResourcePaths::INSTALLDIR);
		}

		_mozgluedll.reset( LoadLibraryA((installPath + "\\mozglue.dll").c_str()) );
		_libnss.reset( LoadLibraryA((installPath + "\\nss3.dll").c_str()) );

		if (!_mozgluedll.get() || !_libnss.get()) {	//if libs from Firefox installation path failed
			//try destributed libs
			_mozgluedll.reset( LoadLibrary(L"FFDecryptLibs\\mozglue.dll") );
			_libnss.reset( LoadLibrary(L"FFDecryptLibs\\nss3.dll") );

			if (!_mozgluedll.get() || !_libnss.get()) {
				//display error
				MessageBoxA(NULL, "Check if mozglue.dll and nss3.dll are present in \"FFDecryptLibs\" folder OR try running version of this application (32 or 64bit) which match with your Firefox version", 
								  "ParserDLL error: \"Couldn't find proper Firefox decrypt libraries\"", 
								  MB_OK | MB_ICONERROR);
				return;
			}
		}

		NSSInit = (NSS_Init)GetProcAddress(_libnss.get(), "NSS_Init");
		NSSShutdown = (NSS_Shutdown)GetProcAddress(_libnss.get(), "NSS_Shutdown");
		PK11GetInternalKeySlot = (PK11_GetInternalKeySlot)GetProcAddress(_libnss.get(), "PK11_GetInternalKeySlot");
		PK11FreeSlot = (PK11_FreeSlot)GetProcAddress(_libnss.get(), "PK11_FreeSlot");
		PK11Authenticate = (PK11_Authenticate)GetProcAddress(_libnss.get(), "PK11_Authenticate");
		PK11SDRDecrypt = (PK11SDR_Decrypt)GetProcAddress(_libnss.get(), "PK11SDR_Decrypt");
	}


	size_t LoginsParser::getTotalRecords(size_t profile)
	{
		std::string profileDir;

		if (auto sh = _file_accessor_ref.lock()) {
			profileDir = sh->getPathToResource(EResourcePaths::PROFILES, profile);
		}

		try	{
			if (_pt.empty()) {
				boost::property_tree::read_json(profileDir.append("\\logins.json"), _pt);
			}

			return _pt.get_child("logins").size();
		}
		catch (const std::exception& ex) {
			//display error
			MessageBoxA(NULL, ex.what(), "ParserDLL error: \"Error during LOGINS.JSON parsing occured\"", MB_OK | MB_ICONERROR);
		}

		return 0;
	}


	size_t LoginsParser::parse(size_t profile, std::vector<std::vector<std::string>>& output, size_t from, size_t number)
	{
		size_t count = 0;
		std::string profileDir;
		size_t limit_counter = 0;
		bool nolimit = false;
		size_t max = from + number;


		//test for failed lib
		if (NSSInit == nullptr) {
			return 0;
		}

		if (number == 0) {
			nolimit = true;
		}

		if (auto sh = _file_accessor_ref.lock()) {
			profileDir = sh->getPathToResource(EResourcePaths::PROFILEPATH, profile);
		}

		NSSInit(profileDir.c_str());


		try	{
			if (_pt.empty()) {
				boost::property_tree::read_json(profileDir.append("\\logins.json"), _pt);
			}

			//iterate through all logins
			for (auto search_result : _pt.get_child("logins")) {
				//limit returned records
				if (limit_counter >= from && (limit_counter < max || nolimit)) {
					std::vector<std::string> tmp;
					tmp.push_back(search_result.second.get<std::string>("hostname"));
					tmp.push_back(DecryptString(std::string(search_result.second.get<std::string>("encryptedUsername"))));
					tmp.push_back(DecryptString(std::string(search_result.second.get<std::string>("encryptedPassword"))));

					output.push_back(std::move(tmp));
					++count;
				}

				++limit_counter;
			}
		}
		catch (const std::exception& ex) {
			//display error
			MessageBoxA(NULL, ex.what(), "ParserDLL error: \"Error during LOGINS.JSON parsing occured\"", MB_OK | MB_ICONERROR);
		}

		NSSShutdown();

		return count;
	}

}