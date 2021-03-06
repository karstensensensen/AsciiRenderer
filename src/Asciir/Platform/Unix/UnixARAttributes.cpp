#include "arpch.h"
#include "UnixARAttributes.h"
#include "Asciir/Rendering/AsciiAttributes.h"
#include "Asciir/Rendering/RenderConsts.h"
#include "Asciir/Rendering/TerminalRenderer.h"
#include "Asciir/Logging/Log.h"

namespace Asciir
{
	UnixARAttr::UnixARAttr()
	{
		// X11 initialization

		m_display = XOpenDisplay(NULL);

		m_window = DefaultRootWindow(m_display);
		int revert;

		XGetInputFocus(m_display, &m_focus_win, &revert);

		XSelectInput(m_display, m_focus_win, KeyPressMask | KeyReleaseMask | FocusChangeMask);

		// disable echo

		/*
		termios term;
		tcgetattr(STDIN_FILENO, &term);
		term.c_lflag |= ~ECHO;
		tcsetattr(STDIN_FILENO, TCSANOW, &term);
		*/

		clearColour();
	}

	UnixARAttr::~UnixARAttr()
	{
		XCloseDisplay(m_display);
	}

	std::string UnixARAttr::ansiCode()
	{
		size_t size = 3 + (5 + 4 * 3) * 2;

		for (size_t i = 1; i < 5; i++)
		{
			size += 2ULL * attributes[i];
		}

		std::string escseq;
		escseq.reserve(size);

		ansiCode(escseq);

		return escseq;
	}

	void UnixARAttr::ansiCode(std::string& dst)
	{
		// cursor

		moveCode(dst);

		// formatting
		dst += AR_ANSIS_CSI;
		dst += "0";

		if (attributes[ITALIC])
			dst += ";3";

		if (attributes[UNDERLINE])
			dst += ";4";

		if (attributes[BLINK])
			dst += ";5";

		if (attributes[STRIKE])
			dst += ";9";

		// foreground color

		if (attributes[BOLD])
		{
			unsigned char red = m_foreground.red + AR_BOLD_DIFF;
			unsigned char green = m_foreground.green + AR_BOLD_DIFF;
			unsigned char blue = m_foreground.blue + AR_BOLD_DIFF;

			red = red > m_foreground.red ? red : 255;
			green = green > m_foreground.green ? green : 255;
			blue = blue > m_foreground.blue ? blue : 255;

			dst += ";38;2;";
			dst += std::to_string(red);
			dst += ";";
			dst += std::to_string(green);
			dst += ";";
			dst += std::to_string(blue);
			dst += ";";
		}
		else
		{
			dst += ";38;2;";
			dst += std::to_string(m_foreground.red);
			dst += ";";
			dst += std::to_string(m_foreground.green);
			dst += ";";
			dst += std::to_string(m_foreground.blue);
			dst += ";";
		}

		// background color
		dst += "48;2;";
		dst += std::to_string(m_background.red);
		dst += ";";
		dst += std::to_string(m_background.green);
		dst += ";";
		dst += std::to_string(m_background.blue);
		dst += 'm';
	}

	void UnixARAttr::ansiCode(std::ostream& stream, bool is_newline)
	{
		// if nothing has changed do not modify the stream
		bool has_changed = false;
		for (size_t i = 0; i < ATTR_COUNT; i++)
			if (attributes[i] != last_attributes[i])
			{
				has_changed = true;
				break;
			}


		if (!has_changed && m_foreground != m_last_foreground)
			has_changed = true;

		if (!has_changed && m_background != m_last_background)
			has_changed = true;

		if (m_cleared)
			has_changed = true;


		if (!has_changed)
			return;
		// cursor
		moveCode(stream);

		// formatting
		stream << AR_ANSIS_CSI;

		if (attributes[ITALIC] && (!last_attributes[ITALIC] || m_cleared))
			stream << ";3";
		else if (!attributes[ITALIC] && (last_attributes[ITALIC] || m_cleared))
			stream << ";23";

		if (attributes[UNDERLINE] && (!last_attributes[UNDERLINE] || m_cleared))
			stream << ";4";
		else if (!attributes[UNDERLINE] && (last_attributes[UNDERLINE] || m_cleared))
			stream << ";24";

		if (attributes[BLINK] && (!last_attributes[BLINK] || m_cleared))
			stream << ";5";
		else if (!attributes[BLINK] && (last_attributes[BLINK] || m_cleared))
			stream << ";25";

		if (attributes[STRIKE] && (!last_attributes[STRIKE] || m_cleared))
			stream << ";9";
		else if (!attributes[STRIKE] && (last_attributes[STRIKE] || m_cleared))
			stream << ";29";

		// foreground color
		// background color needs to be updated if the foreground color is changed
		if (attributes[BOLD] && ((!last_attributes[BOLD] || m_foreground != m_last_foreground) || m_cleared || is_newline))
		{
			unsigned char red = m_foreground.red + AR_BOLD_DIFF;
			unsigned char green = m_foreground.green + AR_BOLD_DIFF;
			unsigned char blue = m_foreground.blue + AR_BOLD_DIFF;

			red = red > m_foreground.red ? red : 255;
			green = green > m_foreground.green ? green : 255;
			blue = blue > m_foreground.blue ? blue : 255;

			stream << ";38;2;";
			stream << std::to_string(red);
			stream << ";";
			stream << std::to_string(green);
			stream << ";";
			stream << std::to_string(blue);

			stream << ";48;2;";
			stream << std::to_string(m_background.red);
			stream << ';';
			stream << std::to_string(m_background.green);
			stream << ';';
			stream << std::to_string(m_background.blue);
		}
		else if (m_foreground != m_last_foreground || m_cleared || is_newline)
		{
			stream << ";38;2;";
			stream << std::to_string(m_foreground.red);
			stream << ";";
			stream << std::to_string(m_foreground.green);
			stream << ";";
			stream << std::to_string(m_foreground.blue);

			stream << ";48;2;";
			stream << std::to_string(m_background.red);
			stream << ';';
			stream << std::to_string(m_background.green);
			stream << ';';
			stream << std::to_string(m_background.blue);
		}
		else if (m_background != m_last_background || m_cleared || is_newline)
		{
			// only background color
			stream << ";48;2;";
			stream << std::to_string(m_background.red);
			stream << ';';
			stream << std::to_string(m_background.green);
			stream << ';';
			stream << std::to_string(m_background.blue);
		}

		stream << 'm';
		last_attributes = attributes;
		m_last_foreground = m_foreground;
		m_last_background = m_background;

		m_cleared = false;
	}

	void UnixARAttr::ansiCode(TerminalRenderer& dst, bool is_newline)
	{
		// if nothing has changed do not modify the stream
		bool has_changed = false;
		for (size_t i = 0; i < ATTR_COUNT; i++)
			if (attributes[i] != last_attributes[i])
			{
				has_changed = true;
				break;
			}


		if (!has_changed && m_foreground != m_last_foreground)
			has_changed = true;

		if (!has_changed && m_background != m_last_background)
			has_changed = true;

		if (m_cleared)
			has_changed = true;


		if (!has_changed)
			return;
		// cursor
		moveCode(dst);

		// formatting
		dst.pushBuffer(AR_ANSIS_CSI);

		if (attributes[ITALIC] && (!last_attributes[ITALIC] || m_cleared))
			dst.pushBuffer(";3");
		else if (!attributes[ITALIC] && (last_attributes[ITALIC] || m_cleared))
			dst.pushBuffer(";23");

		if (attributes[UNDERLINE] && (!last_attributes[UNDERLINE] || m_cleared))
			dst.pushBuffer(";4");
		else if (!attributes[UNDERLINE] && (last_attributes[UNDERLINE] || m_cleared))
			dst.pushBuffer(";24");

		if (attributes[BLINK] && (!last_attributes[BLINK] || m_cleared))
			dst.pushBuffer(";5");
		else if (!attributes[BLINK] && (last_attributes[BLINK] || m_cleared))
			dst.pushBuffer(";25");

		if (attributes[STRIKE] && (!last_attributes[STRIKE] || m_cleared))
			dst.pushBuffer(";9");
		else if (!attributes[STRIKE] && (last_attributes[STRIKE] || m_cleared))
			dst.pushBuffer(";29");

		// foreground color
		// background color needs to be updated if the foreground color is changed
		if (attributes[BOLD] && ((!last_attributes[BOLD] || m_foreground != m_last_foreground) || m_cleared || is_newline))
		{
			unsigned char red = m_foreground.red + AR_BOLD_DIFF;
			unsigned char green = m_foreground.green + AR_BOLD_DIFF;
			unsigned char blue = m_foreground.blue + AR_BOLD_DIFF;

			red = red > m_foreground.red ? red : 255;
			green = green > m_foreground.green ? green : 255;
			blue = blue > m_foreground.blue ? blue : 255;

			dst.pushBuffer(";38;2;");
			dst.pushBuffer(std::to_string(red));
			dst.pushBuffer(";");
			dst.pushBuffer(std::to_string(green));
			dst.pushBuffer(";");
			dst.pushBuffer(std::to_string(blue));

			dst.pushBuffer(";48;2;");
			dst.pushBuffer(std::to_string(m_background.red));
			dst.pushBuffer(';');
			dst.pushBuffer(std::to_string(m_background.green));
			dst.pushBuffer(';');
			dst.pushBuffer(std::to_string(m_background.blue));
		}
		else if (m_foreground != m_last_foreground || m_cleared || is_newline)
		{
			dst.pushBuffer(";38;2;");
			dst.pushBuffer(std::to_string(m_foreground.red));
			dst.pushBuffer(";");
			dst.pushBuffer(std::to_string(m_foreground.green));
			dst.pushBuffer(";");
			dst.pushBuffer(std::to_string(m_foreground.blue));
			
			dst.pushBuffer(";48;2;");
			dst.pushBuffer(std::to_string(m_background.red));
			dst.pushBuffer(';');
			dst.pushBuffer(std::to_string(m_background.green));
			dst.pushBuffer(';');
			dst.pushBuffer(std::to_string(m_background.blue));
		}
		else if (m_background != m_last_background || m_cleared || is_newline)
		{
			// only background color
			dst.pushBuffer(";48;2;");
			dst.pushBuffer(std::to_string(m_background.red));
			dst.pushBuffer(';');
			dst.pushBuffer(std::to_string(m_background.green));
			dst.pushBuffer(';');
			dst.pushBuffer(std::to_string(m_background.blue));
		}

		dst.pushBuffer('m');
		last_attributes = attributes;
		m_last_foreground = m_foreground;
		m_last_background = m_background;

		m_cleared = false;
	}

	Coord UnixARAttr::terminalPos() const
	{
		// Coord pos;
		// XWindowAttributes win_info;

		// XGetWindowAttributes(m_display, m_window, &win_info);

		// return Coord(win_info.x, win_info.y);
		return Coord();
	}

	//TODO: Very temprorary implementation

	#include <fcntl.h> 
	#include <string.h>
	#include <errno.h>

	TermVert UnixARAttr::terminalSize() const
	{
		winsize size;

		int result = ioctl(0, TIOCGWINSZ, &size);

		AR_INFO(size.ws_col, size.ws_row);
		
		size.ws_col = 178;
		size.ws_row = 47;

		return TermVert(size.ws_col, size.ws_row);
	}

	TermVert UnixARAttr::maxTerminalSize() const
	{
		// no limits for maximum terminal size on unix systems (?)
		constexpr TInt max_int = std::numeric_limits<TInt>::max();
		return TermVert(max_int, max_int);
	}
}
