#pragma once

#include "Asciir/Core/Core.h"
#include "Asciir/Maths/Vertices.h"

namespace Asciir
{
	static constexpr size_t ATTR_COUNT = 8;

	typedef unsigned short ATTRI;

	static constexpr ATTRI BOLD = 0;
	static constexpr ATTRI ITALIC = 1;
	static constexpr ATTRI UNDERLINE = 2;
	static constexpr ATTRI BLINK = 3;
	static constexpr ATTRI STRIKE = 4;
	static constexpr ATTRI TOP = 7;
	static constexpr ATTRI OVERLINED = 7;
	static constexpr ATTRI BOTTOM = 2;
#ifdef AR_WIN
	static constexpr ATTRI LEFT = 5;
	static constexpr ATTRI RIGHT = 6;
#else
	static constexpr ATTRI FRAMED = 5;
	static constexpr ATTRI ENCIRCLED = 6;
#endif

	static constexpr size_t ATTR_MAX_SIZE = 48;

	struct Color
	{
		unsigned char red, green, blue, alpha;

		Color();
		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a = UCHAR_MAX);
		Color(unsigned char gray, unsigned char a = UCHAR_MAX);
		Color(const Color& other);

		Color inverse() const;

		bool operator==(const Color& other) const;
		bool operator!=(const Color& other) const;
		bool operator<(const Color& other) const;
		bool operator>(const Color& other) const;
		bool operator<=(const Color& other) const;
		bool operator>=(const Color& other) const;

		// blends the two colors taking the alpha value into account
		// uses the current color as the background and calculates the alpha result afterwards
		Color& blend(const Color& other);
		// blends the two colors taking the alpha value into account
		static Color blend(const Color& background, const Color& color);
	};

	std::ostream& operator<<(std::ostream& stream, const Color& c);

	typedef Color RGB24;

	struct RGB8
	{
		unsigned char red, green, blue;

		RGB8(unsigned char r, unsigned char g, unsigned char b);
		RGB8();

		Color getColor();
		operator Color();
	};

	struct RGB4
	{
		bool red, green, blue, intensity;

		RGB4();
		RGB4(bool r, bool g, bool b, bool i = false);

		Color getColor();
		operator Color();
	};

	struct GRAY8
	{
		unsigned char gray;

		GRAY8(unsigned char g);
		GRAY8();

		Color getColor();
		operator Color();
	};

	/*
	* windows does not support
	*
	* Blink
	* Strike
	* Framed
	* Encircled
	* Overlined (= Top)
	*
	* only windows supports
	*
	* Left
	* Right
	* Top (= Overlined)
	* Bottom
	*
	* All of the above = Framed
	*/

	class TerminalRenderer;

	//
	// 
	
	//

	/// @brief class for storing and modifying the ansi attributes of an ascii character
	/// 
	/// also generates the corresponding ansi code that should be printed to the terminal in order to apply the attributes
	/// 
	/// The generated ansi codes assume no other attributes have been applied to the terminal inbetween modifications (not true if resuilt is of string type)
	/// 
	/// This means if an attribute contains the attributes (Color: Orange, Text: Bold) generates the ascii code, and outputs it to the terminal
	/// and is then later modified to contain the attributes (Color: Orange, Text: Underlined).
	/// The next call to ansiCode will remove the bold attribute and apply the underlined attribute whilst doing nothing to the color, as it assumes it is still Orange.
	/// 
	/// a call to clear() will assume unknown modifications have happened, and generate the complete ansi code applying all the attributes whilst clearing all other.
	/// 
	/// @attention only some information is preserved on a newline in streams, so the is_newline parameter must be set accordingly in order to accomidate for this, when calling ansiCode().
	///
	class AsciiAttr
	{
	protected:
		Color m_foreground;
		Color m_background;

		Color m_last_foreground;
		Color m_last_background;

		// used for calculating the only needed modifications to the next ansi code
		std::array<bool, ATTR_COUNT> last_attributes;

		TermVert m_pos;

		bool m_should_move = false;
		bool m_cleared = true;

	public:

		std::array<bool, ATTR_COUNT> attributes;

		AsciiAttr() = default;
		virtual ~AsciiAttr();

		void setForeground(const Color& color);
		Color getForeground();
		void setBackground(const Color& color);
		Color getBackground();
		void setColor(const Color& foreground, const Color& background);

		void clear();
		void clearFormat();
		void clearColor();

		void setAttribute(const ATTRI& attribute, bool val);

		void move(TermVert pos);

#ifdef AR_WIN

		void setBoxed(bool val);
		void setLR(bool val);
		void setTB(bool val);

#endif

		virtual std::string ansiCode() = 0;
		virtual void ansiCode(std::string& dst) = 0;
		virtual void ansiCode(std::ostream& stream, bool is_newline = false) = 0;
		virtual void ansiCode(TerminalRenderer& dst, bool is_newline = false) = 0;

		void moveCode(std::string& dst);
		void moveCode(std::ostream& stream);
		void moveCode(TerminalRenderer& dst);

		void setTitle(const std::string& name);

		virtual Coord terminalPos() const = 0;
		virtual TermVert terminalSize() const = 0;
		virtual TermVert maxTerminalSize() const = 0;
		virtual Size2D fontSize() const = 0;
	};

	std::ostream& operator<<(std::ostream& stream, AsciiAttr& other);
}
