#pragma once
///////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <time.h>
#include <filesystem>
#include <boost/format.hpp>

namespace fs = std::experimental::filesystem;

///////////////////////////////////////////////////////////////////////////////////////////////

typedef std::vector<fs::path> fileVector;
typedef std::vector<fs::path>::iterator fileIterator;
typedef std::vector<fs::path>::reverse_iterator RfileIterator;

typedef std::vector<std::wstring> stringVector;
typedef std::vector<std::wstring>::iterator stringIterator;

std::wstring& replace(std::wstring& s, const std::wstring& from, const std::wstring& to);

template<typename T> void Remove(std::basic_string<T> & Str, const T * CharsToRemove)
{
	std::basic_string<T>::size_type pos = 0;
	while ((pos = Str.find_first_of(CharsToRemove, pos)) != std::std::basic_string<T>::npos)
	{
		Str.erase(pos, 1);
	}
}

inline bool icasecmp(const std::string& l, const std::string& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](std::string::value_type l1, std::string::value_type r1)
	{ return toupper(l1) == toupper(r1); });
}

inline bool icasecmp(const std::wstring& l, const std::wstring& r)
{
	return l.size() == r.size()
		&& equal(l.cbegin(), l.cend(), r.cbegin(),
			[](std::wstring::value_type l1, std::wstring::value_type r1)
	{ return towupper(l1) == towupper(r1); });
}

std::wstring GetLastErrorStr();

bool GetAllFilesFromFolder(const fs::path& strSourceFolder, bool bRecursively, fileVector& strList, std::wstring & mask);
bool GetAllFilesFromFolder(const fs::path& strSourceFolder, bool bRecursively, fileVector &strList, stringVector & maskVector);
void AddBackSlash(fs::path& p);
void PrintError(std::error_code &);


//MakeString2("Operation with id = %1% failed, because data1 (%2%) is incompatible with data2 (%3%)") << id << data1 << data2);
class MakeString
{
public:
	MakeString(const char* fmt) : m_fmt(fmt) {}

	template<class T>
	MakeString& operator<< (const T& arg) {
		m_fmt% arg;
		return *this;
	}
	operator std::string() const {
		return m_fmt.str();
	}
protected:
	boost::format m_fmt;
};