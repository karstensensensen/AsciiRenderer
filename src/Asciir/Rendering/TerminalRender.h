#pragma once

#include "AsciiAttributes.h"
#include "Vertices.h"
#include "Matrix.h"

namespace Asciir
{

	enum class DrawMode
	{
		Line,
		Filled
	};

	struct Tile
	{
		Color background_color = BLACK8;
		Color color = WHITE8;
		char symbol = ' ';
	};

	struct TRUpdateInfo
	{
		bool new_size = false;
		bool new_pos = false;
		bool new_name = false;
	};

	class TerminalRender
	{
	protected:
		Matrix<Tile> m_tiles;
		Coord m_pos;
		Tile m_tile_state = Tile();
		std::string m_title;

	public:
		TerminalRender(const std::string& title, size_t buffer_size);

		void color(Color color);
		void backgroundColor(Color color);
		void symbol(char symbol);

		void drawVertices(const TermVerts& vertices, DrawMode mode = DrawMode::Line);
		void drawLine(const TermVert& a, const TermVert& b);
		void setState(Tile tile);
		Tile getState() const;
		Tile& getState();
		void drawTile(const TermVert& pos);
		Tile& getTile(const TermVert& pos);
		void setTitle(const std::string & title);
		std::string getTitle() const;

		void resize(TermVert size);

		TRUpdateInfo update();
		TRUpdateInfo draw();

		TermVert size() const;
		TermVert maxSize() const;

		Coord pos() const;

		void pushBuffer(const std::string& data);
		void pushBuffer(char c);
		void flushBuffer();

		std::array<bool, ATTR_COUNT>& attributes();

	protected:
		AsciiAttr m_terminal_out;
		std::string m_buffer;
		bool m_should_resize = false;
		bool m_should_rename = true;

		friend AsciiAttr;

		TerminalRender& operator<<(const std::string& data);
		TerminalRender& operator<<(char data);
	};
	
}
