#include "LoginsDecryptor.h"

#include <Windows.h>	//for MessageBox


namespace FFParser {

	//class members
	LoginsDecryptor::LibFreeFunc LoginsDecryptor::s_libFreeFunc = [](HMODULE lib) { if (lib != nullptr) FreeLibrary(lib); };
	std::string LoginsDecryptor::s_libsBackupPath = "FFDecryptLibs";

	//ctor
	LoginsDecryptor::LoginsDecryptor() :
		_mozgluedll(nullptr, s_libFreeFunc),
		_libnss(nullptr, s_libFreeFunc),
		_libsLoadSuccess(false),
		_initSuccess(false)
	{
	}

	//methods

	void LoginsDecryptor::loadLibraries(const std::string& installPath)
	{
		//try installed Firefox's libs

		_mozgluedll.reset( LoadLibraryA((installPath + "\\mozglue.dll").c_str()) );
		_libnss.reset( LoadLibraryA((installPath + "\\nss3.dll").c_str()) );

		if (_mozgluedll.get() && _libnss.get()) {
			NSSInit = (NSS_Init)GetProcAddress(_libnss.get(), "NSS_Init");
			NSSShutdown = (NSS_Shutdown)GetProcAddress(_libnss.get(), "NSS_Shutdown");
			PK11GetInternalKeySlot = (PK11_GetInternalKeySlot)GetProcAddress(_libnss.get(), "PK11_GetInternalKeySlot");
			PK11FreeSlot = (PK11_FreeSlot)GetProcAddress(_libnss.get(), "PK11_FreeSlot");
			PK11Authenticate = (PK11_Authenticate)GetProcAddress(_libnss.get(), "PK11_Authenticate");
			PK11SDRDecrypt = (PK11SDR_Decrypt)GetProcAddress(_libnss.get(), "PK11SDR_Decrypt");

			if (NSSInit != nullptr &&
				NSSShutdown != nullptr &&
				PK11GetInternalKeySlot != nullptr &&
				PK11FreeSlot != nullptr &&
				PK11Authenticate != nullptr &&
				PK11SDRDecrypt != nullptr) 
			{
				//Libs loaded sucessfully!
				_libsLoadSuccess = true;
			}
		}


		if (!_libsLoadSuccess) {
			// try local distributed libs

			_mozgluedll.reset( LoadLibraryA((s_libsBackupPath + "\\mozglue.dll").c_str()) );
			_libnss.reset( LoadLibraryA((s_libsBackupPath + "\\nss3.dll").c_str()) );

			if (_mozgluedll.get() && _libnss.get()) {
				NSSInit = (NSS_Init)GetProcAddress(_libnss.get(), "NSS_Init");
				NSSShutdown = (NSS_Shutdown)GetProcAddress(_libnss.get(), "NSS_Shutdown");
				PK11GetInternalKeySlot = (PK11_GetInternalKeySlot)GetProcAddress(_libnss.get(), "PK11_GetInternalKeySlot");
				PK11FreeSlot = (PK11_FreeSlot)GetProcAddress(_libnss.get(), "PK11_FreeSlot");
				PK11Authenticate = (PK11_Authenticate)GetProcAddress(_libnss.get(), "PK11_Authenticate");
				PK11SDRDecrypt = (PK11SDR_Decrypt)GetProcAddress(_libnss.get(), "PK11SDR_Decrypt");

				if (NSSInit != nullptr &&
					NSSShutdown != nullptr &&
					PK11GetInternalKeySlot != nullptr &&
					PK11FreeSlot != nullptr &&
					PK11Authenticate != nullptr &&
					PK11SDRDecrypt != nullptr) 
				{
					//Libs loaded sucessfully!
					_libsLoadSuccess = true;
				}
				else {
					//display error
					MessageBoxA(NULL, "Program was unable to load all required functions from Firefox decryption libs.", 
									  "ParserDLL error: \"Couldn't set all function pointers from Firefox decrypt libraries\"", 
									  MB_OK | MB_ICONERROR);
				}
			}
			else {
				//display error
				MessageBoxA(NULL, "Check if mozglue.dll and nss3.dll are present in \"FFDecryptLibs\" folder OR try running version of this application (32-bit or 64-bit) which match with your Firefox version", 
								  "ParserDLL error: \"Couldn't find proper Firefox decrypt libraries\"", 
								  MB_OK | MB_ICONERROR);
			}
		}
	}


	bool LoginsDecryptor::init(const std::string& profile_dir)
	{
		if (_libsLoadSuccess) {
			_initSuccess = !NSSInit(profile_dir.c_str());
			return _initSuccess;
		}
		return false;
	}


	bool LoginsDecryptor::close() 
	{ 
		if (_libsLoadSuccess) {
			_initSuccess = false;
			return !NSSShutdown();
		}
		return false;
	}


	std::string LoginsDecryptor::decryptString(const std::string& encrypted) const
	{
		BYTE byteData[8096] {0};
		DWORD dwLength = 8096;
		PK11SlotInfo* slot = 0;
		SECStatus status;
		SECItem in, out;
		std::string result = "";

		//test for failed NSSInit
		if (!_initSuccess) {
			return encrypted;
		}

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

}
