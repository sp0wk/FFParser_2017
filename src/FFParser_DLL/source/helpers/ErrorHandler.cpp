#include "ErrorHandler.h"

#include <Windows.h>


namespace FFParser {

	ErrorHandler& ErrorHandler::getInstance()
	{
		static ErrorHandler s_instance;
		return s_instance;
	}
	
	//ctor
	ErrorHandler::ErrorHandler() :
		//default callback
		_custom_callback( [](const char* text, const char* title) { ::MessageBoxA(NULL, text, title, MB_OK | MB_ICONERROR); } )
	{
	}

	//methods

	void ErrorHandler::setErrorCallback(void (*callback) (const char*, const char*))
	{
		if (callback != nullptr) {
			_custom_callback = callback;
		}
	}

	void ErrorHandler::onError(const char* text, const char* title)
	{
		_custom_callback(text, title);
	}

}