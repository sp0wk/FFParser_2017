#include "parsing/LoginsParser.h"


#include <boost/property_tree/json_parser.hpp>


namespace FFParser {

	//ctor
	LoginsParser::LoginsParser(const std::shared_ptr<IFileAccessor>& fa) : FileParserBase(fa)
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

		mozgluedll = LoadLibraryA((installPath + "\\mozglue.dll").c_str());
		libnss = LoadLibraryA((installPath + "\\nss3.dll").c_str());

		if (mozgluedll && libnss) {
			NSSInit = (NSS_Init)GetProcAddress(libnss, "NSS_Init");
			NSSShutdown = (NSS_Shutdown)GetProcAddress(libnss, "NSS_Shutdown");
			PK11GetInternalKeySlot = (PK11_GetInternalKeySlot)GetProcAddress(libnss, "PK11_GetInternalKeySlot");
			PK11FreeSlot = (PK11_FreeSlot)GetProcAddress(libnss, "PK11_FreeSlot");
			PK11Authenticate = (PK11_Authenticate)GetProcAddress(libnss, "PK11_Authenticate");
			PK11SDRDecrypt = (PK11SDR_Decrypt)GetProcAddress(libnss, "PK11SDR_Decrypt");
		}
	}


	size_t LoginsParser::getTotalRecords(size_t profile)
	{
		std::string profileDir;

		if (auto sh = _file_accessor_ref.lock()) {
			profileDir = sh->getPathToResource(EResourcePaths::PROFILES, profile);
		}

		try	{
			boost::property_tree::read_json(profileDir.append("\\logins.json"), _pt);

			return _pt.get_child("logins").size();
		}
		catch (const std::exception& ex) {
			//TODO:
			//log error or ignore it
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
			//TODO:
			//display error message or ignore
			return count;
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
			//TODO:
			//log error or ignore it
		}

		NSSShutdown();

		return count;
	}

}