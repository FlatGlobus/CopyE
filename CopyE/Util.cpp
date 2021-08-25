#include "stdafx.h"
#include "Util.h"

//////////////////////////////////////////////////////////////////////////
__time64_t FileTimeToUnixTime(FILETIME& ft)
{
    ULARGE_INTEGER ull;

    ull.LowPart = ft.dwLowDateTime;
    ull.HighPart = ft.dwHighDateTime;

    return ull.QuadPart / 10000000ULL - 11644473600ULL;
}
//////////////////////////////////////////////////////////////////////////
std::wstring GetLastErrorStr()
{
    DWORD errorMessageID = ::GetLastError();
    if (errorMessageID == 0)
        return std::wstring();

    LPTSTR messageBuffer = nullptr;
    size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&messageBuffer, 0, NULL);

    std::wstring message(messageBuffer, size);

    LocalFree(messageBuffer);

    return message;
}

bool IsDots(WIN32_FIND_DATA* pFindData)
{
    bool bResult = false;
    if (pFindData != NULL && pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (pFindData->cFileName[0] == _T('.'))
        {
            if (pFindData->cFileName[1] == _T('\0') ||
                (pFindData->cFileName[1] == _T('.') &&
                    pFindData->cFileName[2] == _T('\0')))
            {
                bResult = true;
            }
        }
    }
    return bResult;
}

bool GetAllFilesFromFolder(const fs::path& sourceFolder, bool recursively, fileDataVector& files, std::wstring& mask)
{
    stringVector maskVector;
    maskVector.push_back(mask);
    return GetAllFilesFromFolder(sourceFolder, recursively, files, maskVector);
}

bool GetAllFilesFromFolder(const fs::path& source, bool recursively, fileDataVector& files, stringVector& maskVector)
{
    fs::path sourceFolder(source);
    AddBackSlash(sourceFolder);

    fs::path current(sourceFolder);
    current /= _T("*");

    WIN32_FIND_DATA FindFileData;
    HANDLE hFind;
    if (recursively)
    {
        hFind = FindFirstFile(current.c_str(), &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE)
            return false;
        do
        {
            if (IsDots(&FindFileData))
                continue;

            if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (recursively)
                    GetAllFilesFromFolder(fs::path(sourceFolder) /= fs::path(FindFileData.cFileName), recursively, files, maskVector);
            }

        } while (FindNextFile(hFind, &FindFileData));
        FindClose(hFind);
    }

    if (maskVector.empty())
    {
        maskVector.push_back(std::wstring(_T("*")));
    }

    for (auto mask = maskVector.begin(); mask != maskVector.end(); ++mask)
    {
        current = sourceFolder;
        AddBackSlash(current);
        current /= (*mask);
        hFind = FindFirstFile(current.c_str(), &FindFileData);
        if (hFind == INVALID_HANDLE_VALUE)
            continue;
        do
        {
            if (IsDots(&FindFileData))
                continue;

            if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
            {
                files.push_back(FileData(fs::path(sourceFolder) /= fs::path(FindFileData.cFileName), FindFileData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE));
            }

        } while (FindNextFile(hFind, &FindFileData));

        FindClose(hFind);
    }
    return !files.empty();
}
void AddBackSlash(fs::path& p)
{
    if (!p.empty() && p.wstring().back() != fs::path::preferred_separator)
        p += fs::path::preferred_separator;
}

void PrintError(std::error_code& e)
{
    if (e)
    {
        std::cout << e.message() << std::endl;
        e.clear();
    }

}

void SetArchived(const fs::path& p, bool flag)
{
    DWORD attr_flags = GetFileAttributes(p.c_str());

    attr_flags = flag ? attr_flags | FILE_ATTRIBUTE_ARCHIVE : attr_flags & !FILE_ATTRIBUTE_ARCHIVE;

    SetFileAttributes(p.c_str(), attr_flags);
}