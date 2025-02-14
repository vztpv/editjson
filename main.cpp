
#include "mimalloc-new-delete.h"

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "claujson.h"


static claujson::Document json;


struct Token {
	uint64_t start = 0;
	uint64_t length = 0;
	uint64_t line = 0;
};

inline bool is_space(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

class Tokenizer {
private:
	char* str = nullptr;
	uint64_t sz = 0;
	uint64_t start = 0;
	uint64_t last = 0;
	uint64_t line = 1; // line start from 1.
	uint8_t table[256] = { 1, 0, }; // is_space?
public:
	Tokenizer(char* str, uint64_t sz) : str(str), sz(sz) {
		//
	}

	bool init() {
		// table[x] == 1 <- space
		// table[x] == 2 <- delimeter
		// 
		table['\0'] = 1;
		for (int i = 0; i < 128; ++i) {
			table[i] = is_space(i) ? 1 : 0;
		}
		table['('] = 2;
		table[')'] = 2;
		table['{'] = 2;
		table['}'] = 2;
		table['['] = 2;
		table[']'] = 2;
		table[':'] = 2;
		table[','] = 2; // comma 
		table['='] = 2;
		table['+'] = 2;
		table['-'] = 2;
		table['*'] = 2;
		table['/'] = 2;
		table['&'] = 2;
		table['|'] = 2;

		{
			int state = 0;
			for (uint64_t i = 0; i < sz; ++i) {
				if (state == 0 && str[i] == '#') {
					state = 1;
					str[i] = ' ';
				}
				else if (state == 1 && (str[i] == '\n' || i == sz - 1)) { // check '\0'? i < sz + 1 ?
					state = 0;
				}
				else if (state == 1) {
					str[i] = ' ';
				}
				else if (state == 0 && str[i] == '\"') {
					state = 2;
				}
				else if (state == 2 && str[i] == '\\') {
					state = 3;
				}
				else if (state == 2 && str[i] == '\"') {
					state = 0;
				}
				else if (state == 3) {
					state = 2;
				}
			}

			return state == 0;
		}
	}

	Token next() {
		Token result;
		int state = 0;

		// pass spaces..
		while (start < sz && table[str[start]] == 1) {
			if (str[start] == '\n') {
				++line;
			}
			++start;
		}
		last = start;

		// find token.
		while (last < sz) {
			unsigned char ch = str[last];

			if (ch == '\n') {
				++line;
			}

			if (state == 0 && ch == '\"') {
				state = 1;
			}
			else if (state == 1 && ch == '\"') {
				state = 0;
			}
			else if (state == 1 && ch == '\\') {
				++last;
			}

			else if (state == 0 && table[ch] == 1) { // check space
				result.start = start;
				result.length = last - start;
				result.line = line;

				start = last + 1;
				last = start;
				break;
			}
			else if (state == 0 && table[ch] == 2) { // check delimeter
				result.start = start;
				result.length = last - start;
				result.line = line;

				if (start == last) {
					result.length = 1;
					start = last + 1;
					last = start;
					break;
				}
				start = last;
				last = start;
				break;
			}

			++last;
		}

		return result;
	}
};



class Statement {
public:
	virtual void execute() {
		//
	}
};
class Expression {
public:
	int id = 0; // 
public:
	Expression(int id = 0) {
		//
	}
public:
	virtual Expression* execute() {
		return nullptr;
	}
};

class Primitive : public Expression {
public:
	claujson::_Value data;
public:
	Primitive() : Expression(1) {
		//
	}
public:
	virtual Expression* execute() {
		return this;
	}
};


// -todo
// == && || + - * / // mod <- %  
class BinaryOperator : public Expression {
public:

public:

};

// input : json, output : json?
class Func : public Statement {
public:
	Expression* input; // json
	std::vector<Statement*> body;
public:
	virtual void execute() {
		//
	}
};
class IfStatement : public Statement {
public:
	Expression* cond;
	std::vector<Statement*> body;
	std::vector<Statement*> else_body;
public:
	virtual void execute() {
		//
	}
};
class WhileStatement : public Statement {
public:
	Expression* cond;
	std::vector<Statement*> body;
public:
	virtual void execute() {
		//
	}
};
class PrintStatement : public Statement {
public:
	Expression* data;
public:
	virtual void execute() {
		auto* x = data->execute();

		// primitive?
		if (x->id == 1) {
			std::cout << (dynamic_cast<Primitive*>(x)->data) << "\n";
		}
	}
};

// pattern? type? view?
class Pattern {
public:
	claujson::_Value json_data;
};

class Program {
public:
	std::unordered_map<std::string, Func*> func; // find func["main"sv] <- start point?
	std::unordered_map<std::string, Pattern*> pattern; //
};

class Scanner {
private:
	char* buf = nullptr;
	uint64_t sz = 0;
	Tokenizer tokenizer;
public:
	enum class ScanType : uint32_t {
		NONE = 0, JSON_VALUE = 128,

		Identifier, //

		Pattern,
		Func, // Function, 
		Ref,
		Option, // %int ~~

		Return,

		While, Break, Continue,
		If, Else,
		
		Print, PrintLine,
		Write,
		Goto,
		Enter,
		Quit,
		Next,

		Modular, // mod

		Assignment = '=',
		Comma = ',',
		Add = '+', Subtract = '-',
		Multiply = '*', Divide = '/', // Modulo,

		Equal, NotEqual,
		LessThan, GreaterThan,
		LessOrEqual, GreaterOrEqual,

		LogicalAnd, LogicalOr,

		Colon = ':',
		LeftParen = '(', RightParen = ')',
		LeftBrace = '{', RightBrace = '}',
		LeftBraket = '[', RightBraket = ']',
	};
	enum class SubType {
		None, Int, UInt, Float, String, Any, Bool, Null
	};
	class ScanData {
	public:
		ScanType type = ScanType::NONE;
		SubType subtype = SubType::None;

		uint64_t line = 0;

		claujson::_Value json_data;
		
		std::vector<claujson::_Value> json_data_vec; // chk?   x.name 

		std::vector<Pattern> pattern_data;
	};

public:
	Scanner(char* str, uint64_t sz) : buf(str), sz(sz), tokenizer(str, sz) {
		tokenizer.init();
	}
private:
	ScanData nextData;
public:
	ScanData Next() {
		ScanData token;

		auto x = tokenizer.next();

		// chk none token?
		if (x.length == 0) {
			return token;
		}

		token.line = x.line;

		if (x.length == 1) {
			switch (buf[x.start]) {
			case '=':
				if (x.start + 1 < sz && buf[x.start + 1] == '=') {
					token.type = ScanType::Equal;
				}
				else {
					token.type = ScanType::Assignment;
				}
				break;
			case '<':
				if (x.start + 1 < sz && buf[x.start + 1] == '=') {
					token.type = ScanType::LessOrEqual;
				}
				else {
					token.type = ScanType::LessThan;
				}
				break;
			case '>':
				if (x.start + 1 < sz && buf[x.start + 1] == '=') {
					token.type = ScanType::GreaterThan;
				}
				else {
					token.type = ScanType::GreaterOrEqual;
				}
				break;
			case '|':
				if (x.start + 1 < sz && buf[x.start + 1] == '|') {
					token.type = ScanType::LogicalOr;
				}
				else {
					// error
					throw "not valid token : |";
				}
				break;
			case '&':
				if (x.start + 1 < sz && buf[x.start + 1] == '&') {
					token.type = ScanType::LogicalAnd;
				}
				else {
					// error
					throw "not valid token : &";
				}
				break;
			case ',':
			case '+':
			case '-':
			case '*':
			case '/':
			case ':':
			case '(':
			case ')':
			case '{':
			case '}':
			case '[':
			case ']':
				token.type = static_cast<ScanType>(buf[x.start]);
				break;
			default:
				token.type = ScanType::Identifier;
				token.json_data = claujson::_Value(std::string_view(buf + x.start, x.length));
				break;
			}

		}
		else {
			bool pass = false;
			switch (buf[x.start]) {
				// builtin_function start from .(dot)
			case '.':
				if (buf[x.start + 1] == 'p' && strncmp(buf + x.start, ".pattern", 8) == 0) {
					token.type = ScanType::Pattern; pass = true;
				}
				else if (buf[x.start + 1] == 'f' && strncmp(buf + x.start, ".func", 5) == 0) {
					token.type = ScanType::Func; pass = true;
				}
				else if (buf[x.start + 1] == 'p' && strncmp(buf + x.start, ".print", 6) == 0) {
					token.type = ScanType::Print; pass = true;
				}
				else if (buf[x.start + 1] == 'p' && strncmp(buf + x.start, ".println", 8) == 0) {
					token.type = ScanType::PrintLine; pass = true;
				}
				else if (buf[x.start + 1] == 'w' && strncmp(buf + x.start, ".write", 6) == 0) {
					token.type = ScanType::Write; pass = true;
				}
				// modular...
				else if (buf[x.start + 1] == 'm' && strncmp(buf + x.start, ".mod", 4) == 0) {
					token.type = ScanType::Modular; pass = true;
				}
				else if (buf[x.start + 1] == 'g' && strncmp(buf + x.start, ".goto", 5) == 0) {
					token.type = ScanType::Goto; pass = true;
				}
				else if (buf[x.start + 1] == 'e' && strncmp(buf + x.start, ".enter", 6) == 0) {
					token.type = ScanType::Enter; pass = true;
				}
				else if (buf[x.start + 1] == 'q' && strncmp(buf + x.start, ".quit", 5) == 0) {
					token.type = ScanType::Quit; pass = true;
				}
				else if (buf[x.start + 1] == 'n' && strncmp(buf + x.start, ".next", 5) == 0) {
					token.type = ScanType::Next; pass = true;
				}
				
				/// todo - Array, Set, Object(, Map?)
				// 
				break;
			case '\"':
				// check quoted string! and utf8 valid
			{
				claujson::_Value data;
				if (!claujson::convert_string(claujson::StringView(buf + x.start, x.length), data)) {
					throw "convert string in json? fail"; // unicode? /uXXXX ?
				}
				token.type = ScanType::JSON_VALUE;
				token.json_data = std::move(data); pass = true;
				break;
			}

			// check number.. 
			case '0':
			case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			{
				claujson::_Value number;
				if (!claujson::convert_number(claujson::StringView(buf + x.start, x.length), number)) {
					throw "convert number in json? fail"; //
				}
				token.type = ScanType::JSON_VALUE;
				token.json_data = std::move(number); pass = true;
				break;
			}
			break;

			case 't':
				if (4 == x.length && strncmp(buf + x.start, "true", 4) == 0) {
					token.type = ScanType::JSON_VALUE;
					token.json_data = claujson::_Value(true);
					pass = true;
				}
				break;
			case 'f':
				if (5 == x.length && strncmp(buf + x.start, "false", 5) == 0) {
					token.type = ScanType::JSON_VALUE;
					token.json_data = claujson::_Value(false);
					pass = true;
				}
				break;
			case 'n':
				if (4 == x.length && strncmp(buf + x.start, "null", 4) == 0) {
					token.type = ScanType::JSON_VALUE;
					token.json_data = claujson::_Value(nullptr);
					pass = true;
				}
				break;
			case 'r': // ref, return
				if (3 == x.length && strncmp(buf + x.start, "ref", 3) == 0) {
					token.type = ScanType::Ref;
					pass = true;
				}
				else if (6 == x.length && strncmp(buf + x.start, "return", 6) == 0) {
					token.type = ScanType::Return;
					pass = true;
				}
				break;
			case 'i':
				if (2 == x.length && strncmp(buf + x.start, "if", 2) == 0) {
					token.type = ScanType::If;
					pass = true;
				}
				break;
			case 'e':
				if (4 == x.length && strncmp(buf + x.start, "else", 4) == 0) {
					token.type = ScanType::Else;
					pass = true;
				}
				break;
			case 'w':
				if (5 == x.length && strncmp(buf + x.start, "while", 5) == 0) {
					token.type = ScanType::While;
					pass = true;
				}
				break;
			// option..

			// check Option <- %int %float %uint %string %bool? %true %false %null 
			//				<- %any 
			case '%':
				if (4 == x.length && strncmp(buf + x.start, "%int", 4) == 0) {
					token.type = ScanType::Option;
					token.subtype = SubType::Int;
					pass = true;
				}
				break;
				if (5 == x.length && strncmp(buf + x.start, "%uint", 5) == 0) {
					token.type = ScanType::Option;
					token.subtype = SubType::UInt;
					pass = true;
				}
				break;
				if (6 == x.length && strncmp(buf + x.start, "%float", 6) == 0) {
					token.type = ScanType::Option;
					token.subtype = SubType::Float;
					pass = true;
				}
				break;
				if (5 == x.length && strncmp(buf + x.start, "%bool", 5) == 0) {
					token.type = ScanType::Option;
					token.subtype = SubType::Bool;
					pass = true;
				}
				break;
				if (7 == x.length && strncmp(buf + x.start, "%string", 7) == 0) {
					token.type = ScanType::Option;
					token.subtype = SubType::String;
					pass = true;
				}
				break;
				if (5 == x.length && strncmp(buf + x.start, "%null", 5) == 0) {
					token.type = ScanType::Option;
					token.subtype = SubType::Null;
					pass = true;
				}
				break;
				if (3 == x.length && strncmp(buf + x.start, "%any", 3) == 0) {
					token.type = ScanType::Option;
					token.subtype = SubType::Any;
					pass = true;
				}
				break;
			}

			if (!pass) {
				token.type = ScanType::Identifier;
				token.json_data = claujson::_Value(std::string_view(buf + x.start, x.length));
			}
		}

		// find dot, split two string? 
		if (token.type == ScanType::Identifier) {
			auto& x = token.json_data.get_string();
			uint64_t offset = 0;
			uint64_t start = 0;
			while ((offset = x.find('.', start)) != claujson::String::npos) { // find dot(.)
				claujson::_Value after("123");
				after.get_string() = x.substr(start, offset - start);
				token.json_data_vec.push_back(std::move(after));
				start = offset + 1;
			}
			// after find, meet end of string?
			if (start < x.size()) {
				token.json_data_vec.push_back(claujson::_Value(x.substr(start, x.size() - start)));
			}
		}
		// find dots, split several _Value? %root.0."countries"
		if (token.type == ScanType::Option) {
			// %int%float 
			// %root .0 ."counties"
		}

		return token;
	}
};

class Parser {
public:
public:
	Statement* ParseStatement() {

	}
	Statement* ParsePrint() {

	}
	Expression* ParseExpression() {

	}
};

#include <ctime>

int main(int argc, char* argv[])
{
	claujson::log.console();
	claujson::log.info();

	{
		claujson::parser p;
		p.parse(argv[2], json, 0);
		int a, b, c;
		a = clock();
		std::ifstream inFile(argv[1], std::ios::binary); // chk for "twitter.json"
		if (inFile) {
			inFile.seekg(0, std::ios::end);
			uint64_t sz = inFile.tellg();
			char* buf = new char[sz + 1];
			buf[sz] = '\0';
			inFile.seekg(0);
			inFile.read(buf, sz);
			inFile.close();

			b = clock();
			Token token;
			Tokenizer test(buf, sz);
			if (!test.init()) {
				claujson::log << claujson::warn << "does not have valid token+\n";
				return -1;
			}
			while ((token = test.next()).length > 0) {
				//claujson::log << claujson::info << token.start << " " << token.length << " " << token.line << "\n";
				claujson::log << claujson::info << std::string_view(buf + token.start, token.length) << "\n";
			}
			std::cout << token.start << " ";
			delete[] buf;
		}
		c = clock();
		std::cout << b - a << " " << c - b << " " << c - a << "ms\n";
		return 0;
	}

	claujson::log.no_print();
	claujson::log.console();
	claujson::log.info();
	claujson::log.warn();

	claujson::Document d;
	claujson::parser p(22);


	if (p.parse("citylots.json", d, 0).first == false) {
		std::cout << "err";
	}


	return 0;
}

