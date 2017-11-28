#pragma once

#include <memory>
#include <functional>
#include <string>
#include <vector>



namespace FFParser {

	class ErrorHandler final
	{
	public:
		static ErrorHandler& getInstance();

		void setErrorCallback(void (*callback) (const char*, const char*));
		void onError(const char* title, const char* text);

	private:
		//private ctor and dtor
		ErrorHandler();
		~ErrorHandler() = default;

		std::function<void(const char*, const char*)> _custom_callback;
	};

}