/**
 *
 *  Copyright (C) 2018-2019 khjxiaogu
 *  Copyright (c) 2007 Go Watanabe
 *  Author: khjxiaogu
 *  Web: http://www.khjxiaogu.com
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
*/
#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <algorithm>
#include <map>
#include <vector>
enum vt {
	object,
	array,
	string,
	real,
	integer,
	boul,
	null
};
class variant {
public:
	void* val;
	vt type;
	std::map<std::wstring, variant>& getObject() {
		if (type == vt::object)
			return *(std::map<std::wstring, variant>*)val;
	}
	std::vector<variant>& getArray() {
		if (type == vt::array)
			return *(std::vector<variant>*)val;
	}
	std::wstring getString() {
		if (type == vt::string)
			return *(std::wstring*)val;
		else if (type == real)
			return std::to_wstring(*(double*)val);
		else if (type == integer)
			return std::to_wstring(*(int*)val);
		else if (type == vt::boul)
			return std::to_wstring(*(bool*)val);
		else if (type == vt::null)
			return std::wstring();
		
	}
	double getReal() {
		if (type == real)
			return *(double*)val;
		else if (type == integer)
			return *(int*)val;
		else if (type == vt::boul)
			return *(bool*)val;
		else if (type == vt::string)
			return std::stod(*(std::wstring*)val);
		else if (type == vt::null)
			return 0.0;
	}
	int getInt() {
		if (type == integer)
			return *(int*)val;
		else if (type == real)
			return (int)*(double*)val;
		else if (type == vt::string)
			return std::stoi(*(std::wstring*)val);
		else if (type == vt::boul)
			return (int)*(bool*)val;
		else if (type == vt::null)
			return 0;
	}
	bool getBool() {
		if (type == vt::boul)
			return *(bool*)val;
		else if (type == integer)
			return (bool)*(int*)val;
		else if (type == real)
			return (bool)*(double*)val;
		else if (type == vt::string)
			return std::stoi(*(std::wstring*)val);
		else if (type == vt::null)
			return false;
	}
	bool isNull() {
		return type == vt::null;
	}
};
#define UNICODE_BOM (0xfeff)
class IReader {
public:

	virtual int getc() = 0;
	virtual void ungetc() = 0;
	virtual void close() = 0;

	bool isError;
	IReader();

	virtual ~IReader();;
	void toEOL();
	int next();
	void next(std::wstring& str, int n);

	void parseObject(variant& var);

	void parseArray(variant& var);

	void parseQuoteString(int quote, variant& var);

	bool isNumberFirst(int ch);

	bool isNumber(int ch);

	bool isString(int ch);

	void parse(variant& var);
};


class IStringReader : public IReader {

	std::wstring dat;
	const wchar_t* p;
	unsigned long length;
	unsigned long pos;

public:
	IStringReader(const wchar_t* str);

	void close();

	int getc();

	void ungetc();
};
void eval(IReader& file, variant* result);
variant evalJson(std::wstring str);