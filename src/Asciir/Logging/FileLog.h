#pragma once

#include "Asciir/Core/Core.h"
#include "Asciir/Core/Timing.h"

namespace Asciir
{
	/// @brief class for writing logs asynchronously to a file.  
	/// 
	/// output log will be formatted as this:
	/// > [{size of log in bytes}][{log_level}][{time of log}][{log_source}][{log_type}] on line [{line}] in source file [{file}]:\n
	/// > args...
	class FileLog
	{
		DeltaTime m_log_start;
		std::ofstream m_log_file;
		std::mutex m_log_file_mutex;
		std::string m_name;
		bool m_save_file;
		bool is_open;
		
	public:

		/// @brief Default constructor.  
		/// for a constructor that automaticly opens a log file with a given name, see FileLog(const std::string&, bool, bool)
		FileLog();
		/// @brief passes the given arguments to open()
		/// @see open()
		FileLog(const std::string& log_name, bool save_log = true, bool append_logs = false);
		/// @brief automaticly closes the log file.
		~FileLog();

		/// @brief opens a log file with the given log name
		/// @param log_name the name of the log file
		/// @param save_log wether to keep the log file after the FileLog is destructed
		/// @param append_logs wether to overwrite an existing log file with the same name as log_name, or to append to it
		void open(const std::string& log_name, bool save_log = true, bool append_logs = false);
		/// @brief closes (and deletes if specified) the currently open log file.
		void close();

		/// @return the name of the currently open log file
		const std::string& getName() const;
		/// @return the absolute directory of the currently open log file (not including file)
		std::string getDir() const;

		/// @brief log a log message to the log file
		/// @param log_level the level of the log
		/// @param log_source the source inside the application the log was generated from (core, client, user etc...)
		/// @param log_type the type of the log (warn, info, err etc...)
		/// @param line the line the log was generated at
		/// @param file the file where the log was generated
		/// @param ...args the arguments to be logged, accepts anything that can be passed to the ostream '<<' operator.
		template<typename ... T>
		void Log(size_t log_level, const char* log_source, const char* log_type, size_t line, const char* file, const T& ... args);

	private:

		/// @brief helper function to the Log() function
		/// is simply the function called in the new thread generated by Log()
		template<typename ... T>
		void async_log(size_t log_level, const char* log_source, const char* log_type, size_t line, const char* file, std::tuple<const T& ...>& args);


		/// @brief helper function for putting a variadic number of arguments into a string stream
		/// 
		/// end function of the recursive write_tuple method.
		/// this method will be recieved when the index in the variadic argument list is equal to the argument count (no more arguments avaliable)
		template<size_t I = 0, typename ... T, std::enable_if_t<I == sizeof...(T), bool> = 0>
		void write_tuple(std::stringstream&, std::tuple<const T&...>&) {}

		/// @brief recursive function that writes a variadic number of arguments, in a tuple, to the passed string stream.  
		/// 
		/// each variadic argument will be seperated by a space ' '.
		/// 
		/// @param sstream stringstream to recieve the passed variadic arguments
		/// @param data the variadic arguments to be put into the string stream
		template<size_t I = 0, typename ... T, std::enable_if_t<I < sizeof...(T), bool> = 0>
		void write_tuple(std::stringstream& sstream, std::tuple<const T&...>& data) { sstream << std::get<I>(data) << ' '; write_tuple<I + 1, T...>(sstream, data); }
	};
}

#include "FileLog.ipp"
