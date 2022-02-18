#pragma once

#include <AsciirHeaders.h>

namespace Asciir
{
	/// @brief Class for printing out and formatting log files generated by an asciir program
	class LogViewer
	{
		std::unique_ptr<AsciiAttr> m_log_attributes;
		std::filesystem::path m_log_dir;
		std::ifstream m_log_file;
		std::vector<Color> m_colors;
		size_t m_pos = 0;

	public:
		// 
		// 
		// 

		/// @brief takes the path of the log file, as well as the color coding for each log level
		/// 
		/// log file can be appended to whilst it is open by a LogViewer
		/// index of color = corresponding log level
		/// 
		/// @param log_dir 
		/// @param colors 
		LogViewer(std::filesystem::path log_dir, const std::vector<Color>& colors);
		~LogViewer();

		/// @brief returns the current position in the log file
		size_t pos();
		/// @brief returns the size of the log file
		size_t size();

		void open();
		void close();

		/// @brief returns true if there are currently lines avaliable in the log file
		bool hasLogs();

		/// @brief puts a formatted line from the log file to the output stream
		bool logLineOut(std::ostream& stream);

		/// @brief sets the log viewer back to the top of the log file and clears the passed stream
		// TODO: omit passing a stream
		void reset(std::ostream& stream);
	};
}
