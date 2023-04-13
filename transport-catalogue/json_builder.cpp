#include <exception>
#include "json.h"
#include "json_builder.h"
namespace json {

using namespace std::literals;

DictContext Context::StartDict() {
	return builder_.StartDict();
}
ArrayContext Context::StartArray() {
	return builder_.StartArray();
}

KeyContext Context::Key(const std::string &str) {
	return builder_.Key(str);
}

Builder& Context::EndDict() {
	return builder_.EndDict();
}

Builder& Context::EndArray() {
	return builder_.EndArray();
}

ArrayContext ArrayContext::Value(const Node::Value &value) {
	builder_.Value(value);
	return ArrayContext{builder_};
}

DictContext KeyContext::Value(const Node::Value &value) {
	builder_.Value(value);
	return DictContext{builder_};
}

KeyContext Builder::Key(const std::string &str) {
	if (IsObjectDone()) {
		throw std::logic_error("Call .Key after object is done"s);
	} else if (!(nodes_stack_.back()->IsDict())) {
		throw std::logic_error("Call .Key outside of Dict or after .Key"s);
	}
	if (nodes_stack_.back()->IsDict()) {
		std::get<json::Dict>(nodes_stack_.back()->GetValue())[str];
		nodes_stack_.push_back(&std::get<json::Dict>(nodes_stack_.back()->GetValue())[str]);
	}
	return KeyContext{*this};
}
Builder& Builder::Value(const Node::Value &value) {
	if (IsObjectDone()) {
		throw std::logic_error("Call .Value after object is done"s);
	} else if (!nodes_stack_.empty() && nodes_stack_.back()->IsDict()) {
		throw std::logic_error("Call .Value in Dict"s);
	}

	if (is_empty_ && nodes_stack_.empty()) {
		root_ = json::Node { value };
	} else if (nodes_stack_.back()->IsArray()) {
		std::get<json::Array>(nodes_stack_.back()->GetValue()).push_back(json::Node { value });
	} else {
		nodes_stack_.back()->GetValue() = value;
		nodes_stack_.pop_back();
	}
	is_empty_ = false;
	return *this;
}
DictContext Builder::StartDict() {
	AddNode(json::Dict { });
	return DictContext(*this);
}

ArrayContext Builder::StartArray() {
	AddNode(json::Array { });
	return ArrayContext{*this};
}

Builder& Builder::EndDict() {
	if (IsObjectDone()) {
		throw std::logic_error("Call .EndDict after object is done"s);
	} else if (nodes_stack_.back()->IsArray()) {
		throw std::logic_error("Call .EndDict in array"s);
	}
	if (!nodes_stack_.back()->IsDict()) {
		nodes_stack_.pop_back();
	}
	nodes_stack_.pop_back();
	return *this;
}
Builder& Builder::EndArray() {
	if (IsObjectDone()) {
		throw std::logic_error("Call .EndDict after object is done"s);
	} else if (nodes_stack_.back()->IsDict()) {
		throw std::logic_error("Call .EndArray in dict"s);
	}
	nodes_stack_.pop_back();
	return *this;
}
json::Node Builder::Build() {
	if (is_empty_ || !nodes_stack_.empty()) {
		throw std::logic_error("Bulding empty or not done object"s);
	}
	return root_;
}
bool Builder::IsObjectDone() {
	return !is_empty_ && nodes_stack_.empty();
}
}

