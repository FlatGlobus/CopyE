#pragma once
///////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <vector>
#include <time.h>
#include <filesystem>
#include <boost/format.hpp>

namespace fs = std::filesystem;

///////////////////////////////////////////////////////////////////////////////////////////////
class FileData
{
public:
	bool  is_archive = 0; 
	fs::path file;
	FileData(const fs::path& p, bool a) : file(p), is_archive(a)
	{

	}
};

typedef std::vector<FileData> fileDataVector;
typedef std::vector<fs::path> fileVector;
typedef std::vector<std::wstring> stringVector;


std::wstring& replace(std::wstring& s, const std::wstring& from, const std::wstring& to);


std::wstring GetLastErrorStr();

bool GetAllFilesFromFolder(const fs::path& strSourceFolder, bool bRecursively, fileDataVector& strList, std::wstring & mask);
bool GetAllFilesFromFolder(const fs::path& strSourceFolder, bool bRecursively, fileDataVector &strList, stringVector & maskVector);
void AddBackSlash(fs::path& p);
void PrintError(std::error_code &);

void SetArchived(const fs::path& p, bool flag);

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