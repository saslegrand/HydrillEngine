#pragma once

#include <string>
#include <mutex>

#include <Tools/Event.hpp>
#include <EngineDLL.hpp>

/**
@brief Log message type
*/
enum class MessageType
{
	Info	= 1 << 0,
	Success = 1 << 1,
	Warning = 1 << 2,
	Error   = 1 << 3,

	None
};

/**
@brief Logger manage logs from the engine and when closed is able to create a log file.
*/
class Logger
{
	friend class Engine;

//	Constructors

private:
	std::mutex m_logLock;

public :

	Logger(const char* filename = "LogFile", const char* path = "Logs/");
	~Logger();

//	Variables

private:

	std::string m_content = "";

	const char* m_fileName;
	const char* m_path;

	const char* m_dateFormat = "[%d/%m/%y %H:%M:%S]";

	/**
	@brief Should save log in a folder ? Default : true
	*/
	bool m_save = true;

public :

	/**
	@brief This event is called when print functions are called
	*/
	Event<const std::string&, const std::string&, int> onPrintEvent;

//	Functions

private:

	/**
	@brief Print a message in both the console and the console widget if in the editor
	
	@param text : text to print
	@param messageType : message specifier (success, info, warning, error...)
	*/
	void Print(const std::string& text, const MessageType type);

	/**
	@brief Print text in the console and add it in content string so it can be be saved in the log file
	
	@param text : text to print and save
	*/
	void Write(const std::string& text);

	/**
	@brief Create a ".log" file and write the saved content string into it
	*/
	void CreateFile();

public:
	
	/**
	@brief Print a success message in the logs.
	In the editor log widget it will appear green.
	
	@param text : text to print
	*/
	ENGINE_API static void Success(const std::string& text);

	/**
	@brief Print a simple information message in the logs.
	In the editor log widget it will appear white.
	
	@param text : text to print
	*/
	ENGINE_API static void Info(const std::string& text);

	/**
	@brief Print a warning message in the logs.
	In the editor log widget it will appear orange.
	
	@param text : text to print
	*/
	ENGINE_API static void Warning(const std::string& text);

	/**
	@brief Print an error message in the logs.
	In the editor log widget it will appear red.
	
	@param text : text to print
	*/
	ENGINE_API static void Error(const std::string& text);

	/**
	@brief Print an error message in the logs, then Assert the program.
	In the editor log widget it will appear red.
	
	@param text : text to print
	*/
	ENGINE_API static void AssertError(const std::string& text);
};