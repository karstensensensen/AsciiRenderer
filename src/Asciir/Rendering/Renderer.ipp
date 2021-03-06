#include "Renderer.h"
#include "Asciir/Maths/Lines.h"

namespace Asciir
{
	// in order to be able to copy the s
	template<typename TShader, std::enable_if_t<std::is_base_of_v<Shader2D, TShader>, bool>>
	void Renderer::submit(Ref<TShader> shader, Transform transform)
	{
		ShaderData data{ Ref<Shader2D>(shader), transform };

		// calculate visible quad
		if (data.shader->size() != TermVert(-1, -1))
		{
			// TODO: optimize this if necessary
			Quad texture_quad = Quad(data.shader->size());

			Coord top_left_coord(size());
			Coord bottom_right_coord(0, 0);

			for (const Coord& vert : texture_quad.getVerts())
			{
				Coord transformed_vert = data.transform.applyTransform(vert);
				top_left_coord.x = std::min(top_left_coord.x, transformed_vert.x);
				top_left_coord.y = std::min(top_left_coord.y, transformed_vert.y);

				bottom_right_coord.x = std::max(bottom_right_coord.x, transformed_vert.x);
				bottom_right_coord.y = std::max(bottom_right_coord.y, transformed_vert.y);
			}

			// make sure the area is inside the terminal

			top_left_coord.x = top_left_coord.x < 0 ? 0 : floor(top_left_coord.x);
			top_left_coord.y = top_left_coord.y < 0 ? 0 : floor(top_left_coord.y);

			bottom_right_coord.x = ceil(bottom_right_coord.x);
			bottom_right_coord.x = bottom_right_coord.x >= (long long)size().x ? size().x : bottom_right_coord.x;
			bottom_right_coord.y = ceil(bottom_right_coord.y);
			bottom_right_coord.y = bottom_right_coord.y >= (long long)size().y ? size().y : bottom_right_coord.y;

			data.visible = Quad::fromCorners(top_left_coord, bottom_right_coord);
		}

		submitToQueue(data);
	}

	template<typename T, std::enable_if_t<is_vertices_vtype_v<Coord, T>, bool>>
	Coords Renderer::projectCoordsToTerminal(const T& coords)
	{
		RealVertex term_size = size();

		std::array<Line, 4> terminal_box = {
			Line::fromPoints({ term_size.x,	term_size.y }, { 0, term_size.y }), // top line		<-------↑
			Line::fromPoints({ 0, term_size.y }, { 0, 0 }),						// left line	|		|
			Line::fromPoints({ 0, 0 }, { term_size.x, 0 }),						// bottom line	|		|
			Line::fromPoints({ term_size.x,	0 }, { term_size.x, term_size.y })	// right line	↓------->
		};

		std::array<Line, 2> terminal_diagonals = {
			Line::fromPoints(term_size, {0, 0}),					// lower right -> upper right
			Line::fromPoints({term_size.x, 0}, {0, term_size.y})    // upper right -> lower left
		};

		std::array<bool, 4> has_corner = { false };

		// get number of verticies in new mesh

		size_t res_size = 0;
		Coord last_coord = coords[coords.size() - 1];

		for (size_t i = 0; i < (size_t)coords.size(); i++)
		{
			const Coord& coord = coords[i];
			bool corner = false;

			for (size_t j = 0; j < terminal_diagonals.size(); j++)
			{
				AR_CORE_NOTIFY(terminal_diagonals[j].visible(coord), ' ', terminal_diagonals[j].visible(last_coord));
				if (terminal_diagonals[j].visible(coord) != terminal_diagonals[j].visible(last_coord) && Line::notVisibleByAll(terminal_box, coord) && Line::notVisibleByAll(terminal_box, last_coord))
				{
					Coord intersect = terminal_diagonals[j].intersect(Line::fromPoints(coord, last_coord));
					int visiblity = terminal_diagonals[j + 1 < terminal_diagonals.size() ? j + 1 : 0].notVisible(intersect);
					if (!has_corner[j + visiblity * 2])
					{
						res_size++;
						has_corner[j + visiblity * 2] = true;
					}
				}
			}

			if (corner) continue;

			if (Line::visibleByAll(terminal_box, coord))
				res_size++;

			if (Line::visibleByAll(terminal_box, coord) != Line::visibleByAll(terminal_box, last_coord))
				res_size++;

			last_coord = coord;
		}

		Coords result(res_size);
		size_t res_indx = 0;
		has_corner.fill(false);

		for (size_t i = 0; i < (size_t)coords.size(); i++)
		{
			Coord coord = coords[i];
			bool is_corner = false;

			if (Line::visibleByAll(terminal_box, last_coord))
			{
				result[res_indx] = last_coord;
				res_indx++;
			}

			for (size_t j = 0; j < terminal_diagonals.size(); j++)
			{
				AR_CORE_NOTIFY(terminal_diagonals[j].visible(coord), ' ', terminal_diagonals[j].visible(last_coord));
				if (terminal_diagonals[j].visible(coord) != terminal_diagonals[j].visible(last_coord) && Line::notVisibleByAll(terminal_box, coord) && Line::notVisibleByAll(terminal_box, last_coord))
				{
					Coord intersect = terminal_diagonals[j].intersect(Line::fromPoints(coord, last_coord));
					int visiblity = terminal_diagonals[j + 1 < terminal_diagonals.size() ? j + 1 : 0].notVisible(intersect);
					if (!has_corner[j + visiblity * 2])
					{
						has_corner[j + visiblity * 2] = true;
						result[res_indx] = { (long long)(j * term_size.x), (long long)(i * term_size.y) };
						res_indx++;
						coord = last_coord;
						is_corner = true;
						break;
					}
				}
			}

			if (is_corner) continue;

			for (size_t j = 0; j < terminal_box.size(); j++)
			{
				if (terminal_box[j].notVisible(last_coord) && terminal_box[j + 1 < terminal_box.size() ? j + 1 : 0].notVisible(last_coord))
					break;
				else if (terminal_box[j].notVisible(coord) && terminal_box[j + 1 < terminal_box.size() ? j + 1 : 0].notVisible(coord) &&
					(terminal_box[j].visible(last_coord) || terminal_box[j + 1 < terminal_box.size() ? j + 1 : 0].visible(last_coord)))
				{
					result[res_indx] = terminal_box[j + 1 < terminal_box.size() ? j + 1 : 0].offset;
					res_indx++;
					break;
				}
				else if (terminal_box[j].visible(coord) != terminal_box[j].visible(last_coord))
				{
					result[res_indx] = terminal_box[j].intersect(Line::fromPoints(coord, last_coord));
					res_indx++;
					break;
				}
			}

			last_coord = coord;
		}

		return result;
	}
}