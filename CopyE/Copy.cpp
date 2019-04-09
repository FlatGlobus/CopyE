#include "stdafx.h"
#include "Copy.h"
//////////////////////////////////////////////////////////////////////////
CCopy::CCopy(fileVector& source):src(source),s(false), d(false)
{
}
//////////////////////////////////////////////////////////////////////////
CCopy::~CCopy()
{
}
//////////////////////////////////////////////////////////////////////////
//void CCopy::SetSource(fileVector& source)
// {
// 	src = source;
// }
//////////////////////////////////////////////////////////////////////////
void CCopy::SetDestination(const fs::path& destination)
{
	dst = destination;
}

void CCopy::SetRecursive(bool _s, bool _d)
{
	s = _s;
	d = _d;
}
//////////////////////////////////////////////////////////////////////////
bool CCopy::CheckDir()
{
	std::error_code e;
	bool ret = fs::exists(dst, e);
	PrintError(e);

	if(ret == false)
	{
		std::wcout << _T("Error: Path \"") << dst << _T("\" doesn't exist.") << std::endl;
		return false;
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////
bool CCopy::CollectFiles()
{
	if (CheckDir())
	{
		std::wcout << _T("Collect files from ") << dst << std::endl;
		GetAllFilesFromFolder(dst, d, dst_list, mask);
		std::wcout << dst_list.size() << _T(" files collected") << std::endl;
		return !dst_list.empty();
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
bool CCopy::Process()
{
	bool retCode = false;
	int count = 0;
	std::error_code e;

	MakeMaskVector();

	if (CollectFiles() == true)
	{
 		for (auto i = src.begin(); i != src.end(); i++)
 		{
 			fileVector src_list;
			fs::path src_file(*i);
 			if (!fs::is_regular_file(src_file, e))
 			{
				PrintError(e);
				src_file = src_file.parent_path();
				if (src_file.empty() == true)
					src_file = fs::current_path(e);
				PrintError(e);
				GetAllFilesFromFolder(src_file, s, src_list,(*i).filename().generic_wstring());
 			}
 			else
 			{
 				src_list.push_back(*i);
 			}
			PrintError(e);
 			for(fileIterator k = src_list.begin(); k != src_list.end(); k++)
 			{
 				fileVector targetFiles;
 				Find(*k, targetFiles);
 				if (!targetFiles.empty())
 				{
 					for (fileIterator j = targetFiles.begin(); j != targetFiles.end(); j++)
 					{
 						std::wcout << _T("Copy ") << *k << " to " << *j << std::endl;
 
 						SetFileAttributes(j->c_str(),GetFileAttributes(j->c_str()) & ~FILE_ATTRIBUTE_READONLY);
 						if (CopyFile(k->c_str(), j->c_str(), false) == false)
 						{
 							std::wcout << _T("Error: ") << GetLastErrorStr();
 							continue;
 						}
 						count++;
 					}
 					retCode = true;
 				}
 			}
 		}
	}

    std::wcout << count << _T(" files copied") << std::endl;
	return retCode;
}

//////////////////////////////////////////////////////////////////////////
void CCopy::Find(const fs::path & s, fileVector & found)
{
 	for (auto i = dst_list.begin(); i != dst_list.end(); i++)
 	{
 		if ((*i).has_filename())
 		{
 			if(CheckFilesForCopy(s, *i))
 			{
 				found.push_back(*i);
 			}
 		}
 	}
}

bool CCopy::CheckFilesForCopy(const fs::path & s, const fs::path & d)
{
	if (s.filename() != d.filename())
		return false;

	std::error_code e;
	bool ret = fs::equivalent(s, d, e);
	PrintError(e);
	if(ret == true)
	{
		return false;
	}

// 	auto stime = fs::last_write_time(s,e);
// 	PrintError(e);
// 
// 	auto dtime = fs::last_write_time(d,e);
// 	PrintError(e);
// 
// 	if (stime < dtime)
// 	{
// 		return false;
// 	}
	return true;
}

void CCopy::MakeMaskVector()
{
	mask.empty();

	for (auto i = src.begin(); i != src.end(); i++)
	{
		mask.push_back((*i).filename());
	}
}

