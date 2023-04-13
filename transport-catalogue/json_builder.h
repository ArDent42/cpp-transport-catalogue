#pragma once
#include <exception>
#include "json.h"
namespace json {
using namespace std::literals;

class Builder;
class Context;
class KeyContext;
class DictContext;
class ArrayContext;

class Context {
private:

public:
	json::Builder &builder_;
	Context(json::Builder &builder) :
			builder_(builder) {
	}
	KeyContext Key(const std::string &str);
	Builder& Value(const Node::Value &value);
	ArrayContext StartArray();
	DictContext StartDict();
	Builder& EndDict();
	Builder& EndArray();
	//Node Build();
};

class KeyContext: public Context {
public:
	KeyContext(json::Builder &builder) :
			Context(builder) {
	}
	DictContext Value(const Node::Value &value);
	KeyContext Key(const std::string &str) = delete;
	Builder& EndDict() = delete;
	Builder& EndArray() = delete;
	Node Build() = delete;
};

class DictContext: public Context {
public:
	DictContext(json::Builder &builder) :
			Context(builder) {
	}
	DictContext Value(const Node::Value &value) = delete;
	ArrayContext StartArray() = delete;
	DictContext StartDict() = delete;
	Builder& EndArray() = delete;
	Node Build() = delete;
};

class ArrayContext: public Context {
public:
	ArrayContext(json::Builder &builder) :
			Context(builder) {
	}
	ArrayContext Value(const Node::Value &value);
	KeyContext Key(const std::string &str) = delete;
	Node Build() = delete;
	Builder& EndDict() = delete;
};

class Builder {
public:
	Builder() {
	}
	KeyContext Key(const std::string &str);
	Builder& Value(const Node::Value &value);
	DictContext StartDict();
	ArrayContext StartArray();
	Builder& EndDict();
	Builder& EndArray();
	Node Build();
	template<typename T>
	void AddNode(const T &node);
	bool IsObjectDone();
	Node::Value& GetCurrentValue() {
		return nodes_stack_.back()->GetValue();
	}
private:
	Node root_;
	std::vector<Node*> nodes_stack_;
	bool is_empty_ = true;
};

template<typename T>
void Builder::AddNode(const T &node) {
	if (IsObjectDone()) {
		throw std::logic_error("Call .Start... after object is done"s);
	} else if (!nodes_stack_.empty() && nodes_stack_.back()->IsDict()) {
		throw std::logic_error("Call .Start... in Dict"s);
	}
	is_empty_ = false;
	if (nodes_stack_.empty()) {
		root_ = node;
		nodes_stack_.push_back(&root_);
	} else if (nodes_stack_.back()->IsArray()) {
		std::get<json::Array>(nodes_stack_.back()->GetValue()).push_back(node);
		nodes_stack_.push_back(&std::get<json::Array>(nodes_stack_.back()->GetValue()).back());
	} else {
		nodes_stack_.back()->GetValue() = node;
	}
}

}

