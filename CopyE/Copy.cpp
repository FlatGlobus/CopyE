#include "stdafx.h"
#include "Copy.h"
//////////////////////////////////////////////////////////////////////////
CCopy::CCopy(fileVector& source) :src(source), s(false), d(false), modified_only(false)
{
}
//////////////////////////////////////////////////////////////////////////
CCopy::~CCopy()
{
}
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
void CCopy::SetModifiedOnly(bool m)
{
    modified_only = m;
}
//////////////////////////////////////////////////////////////////////////
bool CCopy::CheckDir()
{
    std::error_code e;
    bool ret = fs::exists(dst, e);
    PrintError(e);

    if (ret == false)
    {
        std::wcout << _T("Error: Path \"") << dst.c_str() << _T("\" doesn't exist.") << std::endl;
        return false;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////
bool CCopy::CollectFiles()
{
    if (CheckDir())
    {
        std::wcout << _T("Collect files from ") << dst.c_str() << std::endl;
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
            fileDataVector src_list;
            fs::path src_file((*i));
            if (!fs::is_regular_file(src_file, e))
            {
                e.clear();
                src_file = src_file.parent_path();
                if (src_file.empty() == true)
                    src_file = fs::current_path(e);
                PrintError(e);
                GetAllFilesFromFolder(src_file, s, src_list, i->filename().generic_wstring());
            }
            else
            {
                src_list.push_back(FileData(*i, false));
            }

            for (auto k = src_list.begin(); k != src_list.end(); k++)
            {
                fileDataVector targetFiles;
                Find((*k), targetFiles);
                if (!targetFiles.empty())
                {
                    for (auto j = targetFiles.begin(); j != targetFiles.end(); j++)
                    {
                        std::wcout << _T("Copy ") << k->file.c_str() << " to " << j->file.c_str() << std::endl;

                        SetFileAttributes(j->file.c_str(), GetFileAttributes(j->file.c_str()) & ~FILE_ATTRIBUTE_READONLY);
                        if (CopyFile(k->file.c_str(), j->file.c_str(), FALSE) == FALSE)
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
void CCopy::Find(const FileData & s, fileDataVector & found)
{
    for (auto i = dst_list.begin(); i != dst_list.end(); i++)
    {
        if (i->file.has_filename())
        {
            if (CheckFilesForCopy(s, *(i)))
            {
                found.push_back(*i);
            }
        }
    }
}

bool CCopy::CheckFilesForCopy(const FileData & s, const FileData & d)
{
    if (s.file.filename() != d.file.filename())
        return false;

    std::error_code e;
    bool ret = fs::equivalent(s.file, d.file, e);
    PrintError(e);
    if (ret == true)
    {
        return false;
    }

    if (modified_only && !s.is_archive)
    {
        return false;
    }
    else
    {
        SetArchived(s.file, false);
    }

    return true;
}

void CCopy::MakeMaskVector()
{
     mask.empty();

    for (auto i = src.begin(); i != src.end(); i++)
    {
        std::wstring msk = i->filename();
        if(msk.size())
            mask.push_back(msk);
    }

    if(mask.size() == 0)
        mask.push_back(_T("*"));
}

