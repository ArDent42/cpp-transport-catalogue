#include "json.h"

using namespace std;

namespace json {
namespace {

Node LoadNode(istream &input);

Node LoadArray(istream &input) {
	Array result;
	for (char c; input >> c && c != ']';) {
		if (c != ',') {
			input.putback(c);
		}
		result.push_back(LoadNode(input));
	}
	if (!input) {
		throw ParsingError("Failed to read array from stream"s);
	}
	return Node(move(result));
}

std::string LoadString(std::istream &input) {
	using namespace std::literals;

	auto it = std::istreambuf_iterator<char>(input);
	auto end = std::istreambuf_iterator<char>();
	std::string s;
	while (true) {
		if (it == end) {
			// Поток закончился до того, как встретили закрывающую кавычку?
			throw ParsingError("String parsing error");
		}
		const char ch = *it;
		if (ch == '"') {
			// Встретили закрывающую кавычку
			++it;
			break;
		} else if (ch == '\\') {
			// Встретили начало escape-последовательности
			++it;
			if (it == end) {
				// Поток завершился сразу после символа обратной косой черты
				throw ParsingError("String parsing error");
			}
			const char escaped_char = *(it);
			// Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
			switch (escaped_char) {
			case 'n':
				s.push_back('\n');
				break;
			case 't':
				s.push_back('\t');
				break;
			case 'r':
				s.push_back('\r');
				break;
			case '"':
				s.push_back('"');
				break;
			case '\\':
				s.push_back('\\');
				break;
			default:
				// Встретили неизвестную escape-последовательность
				throw ParsingError(
						"Unrecognized escape sequence \\"s + escaped_char);
			}
		} else if (ch == '\n' || ch == '\r') {
			// Строковый литерал внутри- JSON не может прерываться символами \r или \n
			throw ParsingError("Unexpected end of line"s);
		} else {
			// Просто считываем очередной символ и помещаем его в результирующую строку
			s.push_back(ch);
		}
		++it;
	}

	return s;
}

Node LoadNull(std::istream &input) {
	std::string str;
	while (std::isalpha(input.peek())) {
		str.push_back(input.get());
	}
	if (str == "null"sv) {
		return Node { nullptr };
	} else {
		throw ParsingError("Failed to parse string as null"s);
	}
}

Node LoadBool(std::istream &input) {
	std::string str;
	while (std::isalpha(input.peek())) {
		str.push_back(input.get());
	}
	if (str == "true") {
		return Node { true };
	} else if (str == "false") {
		return Node { false };
	} else {
		throw ParsingError("Failed to parse string as bool"s);
	}
}

Node LoadDict(istream &input) {
	Dict result;
	for (char c; input >> c && c != '}';) {
		if (c == '"') {
			std::string key = LoadString(input);
			if (input >> c && c == ':') {
				result[std::move(key)] = LoadNode(input);
			}
		}
	}
	if (!input) {
		throw ParsingError("Dic pars error"s);
	}
	return Node(move(result));
}

Number LoadNumber(std::istream &input) {
	using namespace std::literals;

	std::string parsed_num;

	// Считывает в parsed_num очередной символ из input
	auto read_char = [&parsed_num, &input] {
		parsed_num += static_cast<char>(input.get());
		if (!input) {
			throw ParsingError("Failed to read number from stream"s);
		}
	};

	// Считывает одну или более цифр в parsed_num из input
	auto read_digits = [&input, read_char] {
		if (!std::isdigit(input.peek())) {
			throw ParsingError("A digit is expected"s);
		}
		while (std::isdigit(input.peek())) {
			read_char();
		}
	};

	if (input.peek() == '-') {
		read_char();
	}
	// Парсим целую часть числа
	if (input.peek() == '0') {
		read_char();
		// После 0 в JSON не могут идти другие цифры
	} else {
		read_digits();
	}

	bool is_int = true;
	// Парсим дробную часть числа
	if (input.peek() == '.') {
		read_char();
		read_digits();
		is_int = false;
	}

	// Парсим экспоненциальную часть числа
	if (int ch = input.peek(); ch == 'e' || ch == 'E') {
		read_char();
		if (ch = input.peek(); ch == '+' || ch == '-') {
			read_char();
		}
		read_digits();
		is_int = false;
	}

	try {
		if (is_int) {
			// Сначала пробуем преобразовать строку в int
			try {
				return std::stoi(parsed_num);
			} catch (...) {
				// В случае неудачи, например, при переполнении,
				// код ниже попробует преобразовать строку в double
			}
		}
		return std::stod(parsed_num);
	} catch (...) {
		throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
	}
}

Node LoadNode(std::istream &input) {
	char c;
	if (!(input >> c)) {
		throw ParsingError(""s);
	}
	if (c == '[') {
		return LoadArray(input);
	} else if (c == '{') {
		return LoadDict(input);
	} else if (c == '"') {
		return Node { LoadString(input) };
	} else if (c == 't' || c == 'f') {
		input.putback(c);
		return LoadBool(input);
	} else if (c == 'n') {
		input.putback(c);
		return LoadNull(input);
	} else {
		input.putback(c);
		Number number = LoadNumber(input);
		if (std::holds_alternative<int>(number)) {
			return Node { std::get<int>(number) };
		}
		return Node { std::get<double>(number) };
	}
}

//void PrintValue(int value, const PrintContext &ctx) {
//	ctx.out << value;
//}
//
//void PrintValue(double value, const PrintContext &ctx) {
//	ctx.out << value;
//}
}
void PrintValue(std::nullptr_t, const PrintContext &ctx) {
	ctx.out << "null"sv;
}

void PrintValue(bool b, const PrintContext &ctx) {
	ctx.out << std::boolalpha << b;
}

void PrintValue(Array array, const PrintContext &ctx) {
	ctx.out << "[" << std::endl;
	bool is_first = true;
	for (const Node &node : array) {
		if (is_first) {
			is_first = false;
		} else {
			ctx.out << "," << std::endl;
		}
		ctx.Indented().PrintIndent();
		PrintNode(node, ctx.Indented());
	}
	ctx.out << std::endl;
	ctx.PrintIndent();
	ctx.out << "]";
}

void PrintValue(const std::string &str, const PrintContext &ctx) {
	ctx.out << '"';
	for (const char c : str) {
		switch (c) {
		case '\r':
			ctx.out << R"(\r)";
			break;
		case '\n':
			ctx.out << R"(\n)";
			break;
		case '"':
			ctx.out << R"(\")";
			break;
		case '\\':
			ctx.out << R"(\\)";
			break;
		default:
			ctx.out << c;
			break;
		}
	}
	ctx.out << '"';
}

void PrintValue(Dict nodes, const PrintContext &ctx) {
	ctx.out << "{" << std::endl;
	bool is_first = true;
	for (const auto& [key, node] : nodes) {
		if (is_first) {
			is_first = false;
		} else {
			ctx.out << "," << std::endl;
		}
		ctx.Indented().PrintIndent();
		PrintValue(key, ctx);
		ctx.out << ": ";
		PrintNode(node, ctx.Indented());
	}
	ctx.out << std::endl;
	ctx.PrintIndent();
	ctx.out << "}";
}

void PrintNode(const Node &node, const PrintContext &ctx) {
	std::visit([&ctx](const auto &value) {
		PrintValue(value, ctx);
	},
	node.GetValue());
}

void Print(const Document &doc, std::ostream &output) {
	PrintNode(doc.GetRoot(), PrintContext { output });
}

Document Load(istream &input) {
	return Document(LoadNode(input));
}

}  // namespace json
