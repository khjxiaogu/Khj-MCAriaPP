/**
 * copyright (c) 2007 Go Watanabe
 */


#include "json.h"
using namespace std;


// -----------------------------------------------------------------



void eval(IReader& file, variant* result){
	file.parse(*result);
}
variant evalJson(std::wstring str) {
	variant ret;
	IStringReader isr(str.c_str());
	eval(isr, &ret);
	return ret;
}
//---------------------------------------------------------------------------

IStringReader::IStringReader(const wchar_t* str) {
	dat = str;
	p = dat.c_str();
	length = dat.length();
	pos = 0;
}

void IStringReader::close() {
}
int IStringReader::getc() {
	return pos < length ? p[pos++] : EOF;
}
void IStringReader::ungetc() {
	if (pos > 0) {
		pos--;
	}
}
IReader::IReader() {
	isError = false;
}
IReader::~IReader() {}
void IReader::toEOL() {
	int c;
	do {
		c = getc();
	} while (c != EOF && c != '\n' && c != '\r');
}
int IReader::next() {
	for (;;) {
		int c = getc();
		if (c == '#') {
			toEOL();
		}
		else if (c == '/') {
			switch (getc()) {
			case '/':
				toEOL();
				break;
			case '*':
				for (;;) {
					c = getc();
					if (c == EOF) {
						return EOF;
					}
					if (c == '*') {
						if (getc() == '/') {
							break;
						}
						ungetc();
					}
				}
				break;
			default:
				ungetc();
				return '/';
			}
		}
		else if (c != UNICODE_BOM && (c == EOF || c > ' ')) {
			return c;
		}
	}
}
void IReader::next(std::wstring& str, int n) {
	str = L"";
	while (n > 0) {
		int c = getc();
		if (c == EOF) {
			break;
		}
		str += c;
		n--;
	}
}

void IReader::parseObject(variant& var) {

	auto keymap = new std::map<std::wstring, variant>();
	var.type = object;
	var.val = keymap;
	int c;
	for (;;) {
		variant key;
		c = next();
		if (c == EOF) {
			return;
		}
		else if (c == '}') {
			return;
		}
		else if (c == ',' || c == ';') {
			ungetc();
		}
		else {
			ungetc();
			parse(key);
			c = next();
			if (c == '=') {
				if (getc() != '>') {
					ungetc();
				}
			}
			else if (c != ':') {
				return;
			}
			variant value;
			parse(value);
			keymap->emplace(*(std::wstring*)key.val, value);
		}

		switch (next()) {
		case ';':
		case ',':
			break;
		case '}':
			return;
		default:
			return;
		}
	}
}

void IReader::parseArray(variant& var) {

	std::vector<variant>* array = new std::vector<variant>();
	var.type = vt::array;
	var.val = array;
	int cnt = 0;

	for (;;) {
		int ch = next();
		switch (ch) {
		case EOF:
			return;
		case ']':
			return;
		case ',':
		case ';':
		{
			ungetc();
			variant value;
			array->push_back(value);
		}
		break;
		default:
			ungetc();
			variant value;
			parse(value);
			array->push_back(value);
		}

		switch (next()) {
		case ';':
		case ',':
			break;
		case ']':
			return;
		default:
			return;
		}
	}
}
void IReader::parseQuoteString(int quote, variant& var) {
	int c;
	std::wstring str;
	for (;;) {
		c = getc();
		switch (c) {
		case 0:
		case '\n':
		case '\r':
			return;
		case '\\':
			c = getc();
			switch (c) {
			case 'b':
				str += '\b';
				break;
			case 'f':
				str += '\f';
				break;
			case 't':
				str += '\t';
				break;
			case 'r':
				str += '\r';
				break;
			case 'n':
				str += '\n';
				break;
			case 'u':
			{
				std::wstring work;
				next(work, 4);
				str += (wchar_t)wcstol(work.c_str(), NULL, 16);
			}
			break;
			case 'x':
			{
				std::wstring work;
				next(work, 2);
				str += (wchar_t)wcstol(work.c_str(), NULL, 16);
			};
			break;
			default:
				str += c;
			}
			break;
		default:
			if (c == quote) {
				var.val = new std::wstring(str);
				var.type = vt::string;
				return;
			}
			str += c;
		}
	}
}

bool IReader::isNumberFirst(int ch) {
	return (ch >= '0' && ch <= '9') || ch == '.' || ch == '-' || ch == '+';
}

bool IReader::isNumber(int ch) {
	return (ch >= '0' && ch <= '9') || ch == '.' || ch == '-' || ch == '+' || ch == 'e' || ch == 'E';
}

bool IReader::isString(int ch) {
	return ch > 0x80 || ch > ' ' && wcschr(L",:]}/\\\"[{;=#", ch) == NULL;
}

void IReader::parse(variant& var) {

	int ch = next();

	switch (ch) {
	case '"':
	case '\'':
		parseQuoteString(ch, var);
		break;
	case '{':
		parseObject(var);
		break;
	case '[':
		parseArray(var);
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':
	case '-':
	case '+':
	{
		bool doubleValue = false;

		std::wstring s;
		while (isNumber(ch)) {
			if (ch == '.') {
				doubleValue = true; 
			}
			s += ch;
			ch = getc();
		}
		ungetc();

		if (doubleValue) {
			double value = wcstod(s.c_str(), NULL);
			var.val = new double{ value };
			var.type = real;
		}
		else {
			__int64 value = _wcstoi64(s.c_str(), NULL, 0);
			var.val = new __int64{ value };
			var.type = integer;
		}
	}
	break;
	default:
		if (ch >= 'a' && ch <= 'z') {
			std::wstring s;
			while (ch >= 'a' && ch <= 'z') {
				s += ch;
				ch = getc();
			}
			ungetc();

			if (s == L"true") {
				var.val = new bool{ true };
				var.type = vt::boul;
			}
			else if (s == L"false") {
				var.val = new bool{ false};
				var.type = vt::boul;
			}
			else if (s == L"null") {
				var.type = null;
			}
			else if (s == L"void") {
				var.type = null;
			}
			else {
			}
		}
		else {
		}
	}
}
