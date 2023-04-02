#include "svg.h"

namespace svg {

using namespace std::literals;

std::ostream& operator<<(std::ostream &os,
		const svg::StrokeLineCap &stroke_linecap) {
	switch (stroke_linecap) {
	case svg::StrokeLineCap::BUTT:
		os << "butt";
		break;
	case svg::StrokeLineCap::ROUND:
		os << "round";
		break;
	case svg::StrokeLineCap::SQUARE:
		os << "square";
		break;
	}
	return os;
}

std::ostream& operator<<(std::ostream &os,
		const svg::StrokeLineJoin &stroke_linejoin) {
	switch (stroke_linejoin) {
	case svg::StrokeLineJoin::ARCS:
		os << "arcs";
		break;
	case svg::StrokeLineJoin::BEVEL:
		os << "bevel";
		break;
	case svg::StrokeLineJoin::MITER:
		os << "miter";
		break;
	case svg::StrokeLineJoin::MITER_CLIP:
		os << "miter-clip";
		break;
	case svg::StrokeLineJoin::ROUND:
		os << "round";
		break;
	}
	return os;
}

std::ostream& operator<<(std::ostream &os, Color color) {
	visit(ColorOutput { os }, color);
	return os;
}

void Object::Render(const RenderContext &context) const {
	context.RenderIndent();

	// Делегируем вывод тега своим подклассам
	RenderObject(context);

	context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center) {
	center_ = center;
	return *this;
}

Circle& Circle::SetRadius(double radius) {
	radius_ = radius;
	return *this;
}

void Circle::RenderObject(const RenderContext &context) const {
	auto &out = context.out;
	out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y
			<< "\" "sv;
	out << "r=\""sv << radius_ << "\""sv;
	RenderAttrs(out);
	out << "/>"sv;
}

//--------------Polyline-------------
Polyline& Polyline::AddPoint(Point point) {
	points_.push_back(point);
	return *this;
}

void Polyline::RenderObject(const RenderContext &context) const {
	auto &out = context.out;
	out << "<polyline points=\"";
	bool is_first = true;
	for (const auto &point : points_) {
		if (is_first) {
			out << point.x << "," << point.y;
			is_first = false;
		} else {
			out << " " << point.x << "," << point.y;
		}
	}
	out << "\"";
	RenderAttrs(out);
	out << "/>";
}

//--------------Text-------------

Text& Text::SetPosition(Point pos) {
	position_ = pos;
	return *this;
}

// Задаёт смещение относительно опорной точки (атрибуты dx, dy)
Text& Text::SetOffset(Point offset) {
	offset_ = offset;
	return *this;
}

// Задаёт размеры шрифта (атрибут font-size)
Text& Text::SetFontSize(uint32_t size) {
	size_ = size;
	return *this;
}

// Задаёт название шрифта (атрибут font-family)
Text& Text::SetFontFamily(std::string font_family) {
	font_family_ = font_family;
	return *this;
}

// Задаёт толщину шрифта (атрибут font-weight)
Text& Text::SetFontWeight(std::string font_weight) {
	font_weight_ = font_weight;
	return *this;
}

// Задаёт текстовое содержимое объекта (отображается внутри тега text)
Text& Text::SetData(std::string data) {
	data_ = data;
	return *this;
}

void Text::RenderObject(const RenderContext &context) const {
	auto &out = context.out;
	out << "<text";
	RenderAttrs(out);
	out << " x=\"" << position_.x << "\" y=\"" << position_.y << "\" dx=\""
			<< offset_.x << "\" dy=\"" << offset_.y << "\" font-size=\""
			<< size_ << "\"";
	if (!font_family_.empty()) {
			out << " font-family=\"" << font_family_ << "\"";
		}
	if (!font_weight_.empty()) {
		out << " font-weight=\"" << font_weight_ << "\"";
	}
	out << ">";
	for (char s : data_) {
		if (s == '\"') {
			out << "&quot";
		} else if (s == '\'') {
			out << "&apos";
		} else if (s == '<') {
			out << "&lt;";
		} else if (s == '>') {
			out << "&gt;";
		} else if (s == '&') {
			out << "&amp;";
		} else {
			out << s;
		}
	}
	out << "</text>";
}

void Document::AddPtr(std::unique_ptr<Object> &&obj) {
	objects_.emplace_back(std::move(obj));
}

void Document::Render(std::ostream &out) const {
	out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
	out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">"
			<< std::endl;
	RenderContext context(out, 2, 2);
	for (const auto &obj : objects_) {
		obj->Render(context);
	}
	out << "</svg>";
}

}  // namespace svg

