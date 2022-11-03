#include <Core/Logger.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

#include "Core/Time.hpp"
#include "EngineContext.hpp"

Logger::Logger(const char* filename, const char* path) : m_fileName(filename), m_path(path)
{
	//	Save in file activated by default
	m_save = true;
}

Logger::~Logger()
{
	if (m_save)
	{
		CreateFile();
	}
}

void Logger::Write(const std::string& text)
{
	//	Display text in console
	std::cout << text << std::endl;

	//	Add text in content file
	m_content += text;
	m_content += "\n";
}


void Logger::CreateFile()
{
	if (m_content == "") return;

	//	Generate full path of the file

	std::string fullPath = std::string(m_path) + std::string(m_fileName);
	fullPath += EngineContext::Instance().time->date.GetDateString("_%d%m%y%H%M%S");
	fullPath += ".log";

	//	Open/Create the file
	std::ofstream logFile(fullPath);

	if (logFile.is_open())
	{
		//	Write inside the file
		logFile << m_content.c_str() << '\0';

		logFile.close();
	}
}


void Logger::Print(const std::string& text, const MessageType type)
{
	if (EngineContext::Instance().time)
	{
		std::lock_guard logGuard(m_logLock);

		std::string date = EngineContext::Instance().time->date.GetDateString(m_dateFormat);

		//	Calls binded events
		onPrintEvent.Invoke(date, text, static_cast<int>(type));

		//	Write in console and content string
		Write(date + " " + text);
	}
}

void Logger::Success(const std::string& text)
{
	EngineContext::Instance().logger->Print("Success : " + text, MessageType::Success);
}

void Logger::Info(const std::string& text)
{
	EngineContext::Instance().logger->Print("Info    : "  + text, MessageType::Info);
}

void Logger::Warning(const std::string& text)
{
	EngineContext::Instance().logger->Print("WARNING : " + text, MessageType::Warning);
}

void Logger::Error(const std::string& text)
{
	EngineContext::Instance().logger->Print("ERROR   : " + text, MessageType::Error);
}

void Logger::AssertError(const std::string& text)
{
	EngineContext::Instance().logger->Print(text, MessageType::Error);

	assert(nullptr);
}

