// Errorlogger.h
// Hornet engine version 2023d
// Chris Rook
// modified 30/4/2023
// Changed to normal char instead of wchar_t
// Removed the macros for control of logging - the system now debugs as long as _DEBUG is define,
// which is defined automatically for Visual Studio in Debug builds.
// Effectively, this means that you will get an error log when using Visual Studion and Debug.
// Otherwise, you get nothing. Sorry. You can always define DEBUG yourself.
// 
// Added string functions
// modified 6/12/2023
// added a new line character to file output in Write

// NOTE: There is a lot of VisualStudio and Windows code in here to help
// debugging, but this should be easy to adapt to a non-VS environment.

#pragma once

#include "SDL_Log.h"
#include <fstream>
#include <string>

// We used to be able to get helpful messages from DirectX, but this is now
// much more complicated.
// To avoid a major re-write, I am using this placeholder for now.
#define ERRORSTRING(x) L"Error not specified"


// The error logger allows the user to write error messages to a log file
// Due to changes in DirectX messaging, some features of the log system
// have been removed from earlier versions.
// See errorlogger.cpp for the file name of the log file (probably "error.log")
class ErrorLogger
{
private:
	static ErrorLogger instance;		// Errorlogger is a singleton
										      // Programmer note - this singleton seems a bit confused.
										      // It works, so leave it for now, but it needs to be tidied up.
	ErrorLogger();
	~ErrorLogger();
	std::ofstream file;				   // The file written to
	const static char s_filename[];	// The file name
	const static int MAXLINES=100;	// Maximum number of lines that will be written
										      // - to prevent a massive file.
	static int LineCount;				// Current number of lines that have been written.
										      // Programmer note- why is this a static?

public:
	static void Write(const char text[]);			 // Writes the given text followed by a new line
												            // Will not write if LineCount < MAXLINES
	static void Write(const std::string text);	   // Writes the given text followed by a new line
														// Will not write if LineCount < MAXLINES
	static void Write(double num);				   // Writes the given number followed by a new line
												            // Will not write if LineCount < MAXLINES
};
