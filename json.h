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

	/**
	 * コンストラクタ
	 */
	IReader();

	virtual ~IReader();;

	/**
	 * 行末まで読み飛ばす
	 */
	void toEOL();

	/**
	 * 空白とコメントを除去して次の文字を返す
	 */
	int next();

	/**
	 * 指定された文字数分の文字列を取得
	 * @param str 文字列の格納先
	 * @param n 文字数
	 */
	void next(std::wstring& str, int n);

	void parseObject(variant& var);

	void parseArray(variant& var);

	/**
	 * クオート文字列のパース
	 * @param quote クオート文字
	 * @param var 格納先
	 */
	void parseQuoteString(int quote, variant& var);

	/**
	 * 指定した文字が数値の１文字目の構成要素かどうか
	 */
	bool isNumberFirst(int ch);

	/**
	 * 指定した文字が数値の構成要素かどうか
	 */
	bool isNumber(int ch);

	/**
	 * 指定した文字が文字列の構成要素かどうか
	 */
	bool isString(int ch);

	/**
	 * パースの実行
	 * @param var 結果格納先
	 */
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