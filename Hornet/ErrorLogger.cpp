// Errorlogger.cpp
#include "errorlogger.h"

const char ErrorLogger::s_filename[]="error.log";
ErrorLogger ErrorLogger::instance;
int ErrorLogger::LineCount=0;

ErrorLogger::ErrorLogger()
{
#ifdef _DEBUG
	file.open("error.log");
	SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION,SDL_LOG_PRIORITY_VERBOSE);
#endif
}

ErrorLogger::~ErrorLogger()
{
#ifdef _DEBUG
	file.close();
#endif
}

// Writes the given text followed by a new line
// Will not write if LineCount < MAXLINES
void ErrorLogger::Write(const char text[])
{
#ifdef _DEBUG
	if(LineCount<MAXLINES)
	{
		instance.file << text<<"\n";
		SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, text);
		if(++LineCount == MAXLINES)
		{
			instance.file << "\nErrorLogger limit reached. Who taught you to program?\n";
			SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "\nErrorLogger limit reached. Who taught you to program?\n");
		}
	}
	instance.file.flush();
#endif
}

void ErrorLogger::Write(const std::string text)
{
		Write(text.c_str());
}

// Writes the given number
// Will not write if LineCount < MAXLINES
void ErrorLogger::Write(double num)
{
	Write(std::to_string(num));
}

