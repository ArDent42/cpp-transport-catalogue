#pragma once

#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <variant>

namespace svg {

using namespace std::literals;

struct Rgb {
	Rgb() = default;
	Rgb(uint8_t r, uint8_t g, uint8_t b) :
			red(r), green(g), blue(b) {
	}
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
};

struct Rgba {
	Rgba() = default;
	Rgba(uint8_t r, uint8_t g, uint8_t b, double o) :
			red(r), green(g), blue(b), opacity(o) {
	}
	uint8_t red = 0;
	uint8_t green = 0;
	uint8_t blue = 0;
	double opacity = 1.0;
};

using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
inline const Color NoneColor { "none" };

struct ColorOutput {
	std::ostream &out;
	void operator()(std::monostate) const {
		out << "none";
	}
	void operator()(std::string color) const {
		out << color;
	}
	void operator()(Rgb color) const {
		out << "rgb("sv << static_cast<uint>(color.red) << ","sv
				<< static_cast<uint>(color.green) << ","sv
				<< static_cast<uint>(color.blue) << ")"sv;
	}
	void operator()(Rgba color) const {
		out << "rgba(" << static_cast<uint>(color.red) << ","sv
				<< static_cast<uint>(color.green) << ","sv
				<< static_cast<uint>(color.blue) << "," << color.opacity << ")";
	}
};

struct GetColor {
	std::monostate operator()(std::monostate) const {
		return std::monostate { };
	}
	std::string operator()(std::string color) const {
		return color;
	}
	svg::Rgb operator()(Rgb color) const {
		return color;
	}
	svg::Rgba operator()(Rgba color) const {
		return color;
	}
};

std::ostream& operator<<(std::ostream &os, Color color);

enum class StrokeLineCap {
	BUTT, ROUND, SQUARE
};

enum class StrokeLineJoin {
	ARCS, BEVEL, MITER, MITER_CLIP, ROUND
};

std::ostream& operator<<(std::ostream &os,
		const svg::StrokeLineJoin &stroke_linejoin);
std::ostream& operator<<(std::ostream &os,
		const svg::StrokeLineCap &stroke_linecap);

struct Point {
	Point() = default;
	Point(double x, double y) :
			x(x), y(y) {
	}
	double x = 0;
	double y = 0;
};

struct RenderContext {
	RenderContext(std::ostream &out) :
			out(out) {
	}
	RenderContext(std::ostream &out, int indent_step, int indent = 0) :
			out(out), indent_step(indent_step), indent(indent) {
	}
	RenderContext Indented() const {
		return {out, indent_step, indent + indent_step};
	}
	void RenderIndent() const {
		for (int i = 0; i < indent; ++i) {
			out.put(' ');
		}
	}
	std::ostream &out;
	int indent_step = 0;
	int indent = 0;
};

template<typename Owner>
class PathProps {
public:
	Owner& SetFillColor(Color color);
	Owner& SetStrokeColor(Color color);
	Owner& SetStrokeWidth(double width);
	Owner& SetStrokeLineCap(StrokeLineCap linecap);
	Owner& SetStrokeLineJoin(StrokeLineJoin linejoin);
protected:
	~PathProps() = default;
	void RenderAttrs(std::ostream &out) const;
private:
	Owner& AsOwner() {
		return static_cast<Owner&>(*this);
	}
	std::optional<Color> fill_color_;
	std::optional<Color> stroke_color_;
	std::optional<double> stroke_width_;
	std::optional<StrokeLineCap> stroke_linecap_;
	std::optional<StrokeLineJoin> stroke_linejoin_;
};

class Object {
public:
	void Render(const RenderContext &context) const;
	virtual ~Object() = default;
private:
	virtual void RenderObject(const RenderContext &context) const = 0;
};

class Circle: public Object, public PathProps<Circle> {
public:
	Circle& SetCenter(Point center);
	Circle& SetRadius(double radius);
private:
	void RenderObject(const RenderContext &context) const override;
	Point center_;
	double radius_ = 1.0;
};

class Polyline: public Object, public PathProps<Polyline> {
public:
	Polyline& AddPoint(Point point);
private:
	void RenderObject(const RenderContext &context) const override;
	std::vector<Point> points_;
};

class Text final : public Object, public PathProps<Text> {
public:
	Text& SetPosition(Point pos);
	Text& SetOffset(Point offset);
	Text& SetFontSize(uint32_t size);
	Text& SetFontFamily(std::string font_family);
	Text& SetFontWeight(std::string font_weight);
	Text& SetData(std::string data);
private:
	Point position_ = { 0.0, 0.0 };
	Point offset_ = { 0.0, 0.0 };
	uint32_t size_ = 1;
	std::string font_weight_;
	std::string font_family_;
	std::string data_;
	void RenderObject(const RenderContext &context) const override;
};

class ObjectContainer {
public:
	virtual void AddPtr(std::unique_ptr<Object> &&obj) = 0;
	template<typename Obj>
	void Add(Obj obj);
	virtual ~ObjectContainer() = default;
};

class Document: public ObjectContainer {
public:
	void AddPtr(std::unique_ptr<Object> &&obj) override;
	void Render(std::ostream &out) const;
private:
	std::vector<std::unique_ptr<Object>> objects_;
};

class Drawable {
public:
	virtual void Draw(ObjectContainer&) const = 0;
	virtual ~Drawable() = default;
};

template<typename Owner>
Owner& PathProps<Owner>::SetFillColor(Color color) {
	fill_color_ = std::move(color);
	return AsOwner();
}
template<typename Owner>
Owner& PathProps<Owner>::SetStrokeColor(Color color) {
	stroke_color_ = std::move(color);
	return AsOwner();
}
template<typename Owner>
Owner& PathProps<Owner>::SetStrokeWidth(double width) {
	stroke_width_ = width;
	return AsOwner();
}
template<typename Owner>
Owner& PathProps<Owner>::SetStrokeLineCap(StrokeLineCap linecap) {
	stroke_linecap_ = linecap;
	return AsOwner();
}
template<typename Owner>
Owner& PathProps<Owner>::SetStrokeLineJoin(StrokeLineJoin linejoin) {
	stroke_linejoin_ = linejoin;
	return AsOwner();
}

template<typename T>
void PathProps<T>::RenderAttrs(std::ostream &out) const {
	using namespace std::literals;
	if (fill_color_) {
		out << " fill=\""sv << *fill_color_ << "\""sv;
	}
	if (stroke_color_) {
		out << " stroke=\""sv << *stroke_color_ << "\""sv;
	}
	if (stroke_width_) {
		out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
	}
	if (stroke_linecap_) {
		out << " stroke-linecap=\""sv << stroke_linecap_.value() << "\""sv;
	}
	if (stroke_linejoin_) {
		out << " stroke-linejoin=\""sv << *stroke_linejoin_ << "\""sv;
	}
}

template<typename Obj>
void ObjectContainer::Add(Obj obj) {
	AddPtr(std::make_unique<Obj>(std::move(obj)));
}

}  // namespace svg
