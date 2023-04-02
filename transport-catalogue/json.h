#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <iostream>
namespace json {

class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;
using Number = std::variant<int, double>;

class ParsingError: public std::runtime_error {
public:
	using runtime_error::runtime_error;
};

class Node {
public:
	using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;
	Node() = default;
	Node(Array array) :
			value_(std::move(array)) {
	}
	Node(Dict map) :
			value_(std::move(map)) {
	}
	Node(int value) :
			value_(value) {
	}
	Node(std::string value) :
			value_(std::move(value)) {
	}
	Node(bool value) :
			value_(value) {
	}
	Node(std::nullptr_t) :
			value_() {
	}
	Node(double value) :
			value_(value) {
	}
	bool IsInt() const {
		return std::holds_alternative<int>(value_);
	}
	bool IsDouble() const {
		return IsInt() || IsPureDouble();
	}
	bool IsPureDouble() const {
		return std::holds_alternative<double>(value_);
	}
	bool IsBool() const {
		return std::holds_alternative<bool>(value_);
	}
	bool IsString() const {
		return std::holds_alternative<std::string>(value_);
	}
	bool IsNull() const {
		return std::holds_alternative<std::nullptr_t>(value_);
	}
	bool IsArray() const {
		return std::holds_alternative<Array>(value_);
	}
	bool IsMap() const {
		return std::holds_alternative<Dict>(value_);
	}
	const Array& AsArray() const {
		if (IsArray()) {
			return std::get<Array>(value_);
		} else {
			throw std::logic_error("No Array");
		}
	}
	const Dict& AsMap() const {
		if (IsMap()) {
			return std::get<Dict>(value_);
		} else {
			throw std::logic_error("No map");
		}
	}
	int AsInt() const {
		if (IsInt()) {
			return std::get<int>(value_);
		} else {
			throw std::logic_error("No int");
		}
	}
	const std::string& AsString() const {
		if (IsString()) {
			return std::get<std::string>(value_);
		} else {
			throw std::logic_error("No String");
		}
	}
	bool AsBool() const {
		if (IsBool()) {
			return std::get<bool>(value_);
		} else {
			throw std::logic_error("No Bool");
		}
	}
	double AsDouble() const {
		if (IsPureDouble()) {
			return std::get<double>(value_);
		} else if (IsInt()) {
			return AsInt();
		} else {
			throw std::logic_error("No Array");
		}
	}
	const Value& GetValue() const {
		return value_;
	}
	bool operator==(const Node &rhs) const {
		return value_ == rhs.GetValue();
	}
	bool operator!=(const Node &rhs) const {
		return !(*this == rhs);
	}

private:
	Value value_;
};

class Document {
public:
	explicit Document(Node root) :
			root_(root) {
	}
	const Node& GetRoot() const {
		return root_;
	}
private:
	Node root_;
};

struct PrintContext {
	std::ostream &out;
	int indent_step = 4;
	int indent = 0;

	void PrintIndent() const {
		for (int i = 0; i < indent; ++i) {
			out.put(' ');
		}
	}
	PrintContext Indented() const {
		return {out, indent_step, indent_step + indent};
	}
};

Node LoadArray(std::istream &input);
std::string LoadString(std::istream &input);
Node LoadNull(std::istream &input);
Node LoadBool(std::istream &input);
Node LoadDict(std::istream &input);
Number LoadNumber(std::istream &input);
void PrintValue(std::nullptr_t, const PrintContext &ctx);
void PrintValue(bool b, const PrintContext &ctx);
void PrintValue(Array array, const PrintContext &ctx);
void PrintValue(const std::string &str, const PrintContext &ctx);
void PrintValue(Dict nodes, const PrintContext &ctx);
template<typename Value>
void PrintValue(Value value, const PrintContext &ctx) {
	ctx.out << value;
}
inline bool operator==(const Document &lhs, const Document &rhs) {
	return lhs.GetRoot() == rhs.GetRoot();
}
inline bool operator!=(const Document &lhs, const Document &rhs) {
	return !(lhs == rhs);
}
void PrintNode(const Node &node, const PrintContext &ctx);
void Print(const Document &doc, std::ostream &output);
Document Load(std::istream &input);
}
