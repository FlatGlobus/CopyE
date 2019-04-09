#include "stdafx.h"
#include "Path.h"
#include "Shlwapi.h"
#include <direct.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPath::CPath()
{
    _bIsRelative = FALSE;
}

CPath::CPath(LPCTSTR szPath, BOOL bIsFolderPath)
{
    SetPath(szPath, bIsFolderPath);
}

CPath::~CPath()
{

}

void CPath::SetPath(LPCTSTR szPath, BOOL bIsFolderPath)
{
    char *szParamPath = NULL;
    char szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
    char szName[_MAX_FNAME], szExt[_MAX_EXT];

    // Reset
    _sOriginalPath.clear();
    _sDriveLabel.clear();
    _bIsRelative = FALSE;
    _aDir.clear();
    _sExtName.clear();

    // Original path
    _sOriginalPath = szPath;

    // Get args and remove them from path
    szParamPath = _strdup( szPath);

    PathUnquoteSpaces(szParamPath);
    if (szParamPath[0] == 0x0)
    {
        free(szParamPath);
        return;
    }

    _splitpath(szParamPath, szDrive, szDir, szName, szExt);

    // Drive
    _sDrive = szDrive;
    // Directory
    _sDir = szDir;
    _sDir=replace(_sDir, "/", "\\");
    if (!_sDir.empty())
        _bIsRelative = (_sDir[0] != '\\');
    
    // FileTitle
    if (bIsFolderPath)
    {
        _sDir = CPath::AddBackSlash(_sDir.c_str());
        _sDir += szName;
        _sDir = CPath::AddBackSlash(_sDir.c_str());
    }
    else
    {
        _sFileTitle = szName;
    }

    // Get extension name (e.g.: "txt")
    if (IsFilePath())
    {
        _sExtName = szExt;
        Remove(_sExtName, ".");
    }
    free(szParamPath);
}

BOOL CPath::IsLocalPath()
{
    return !_sDrive.empty() && !_bIsRelative;
}

BOOL CPath::IsRelativePath()
{
    return _bIsRelative;
}

BOOL CPath::IsFilePath()
{
    return !_sFileTitle.empty();
}

BOOL CPath::ExistFile()
{
    if (!IsFilePath()) return FALSE;

    return PathFileExists(GetPath().c_str());
}

BOOL CPath::ExistLocation()
{
    return PathFileExists(GetLocation().c_str());
}

string CPath::GetAbsolutePath(LPCTSTR szBaseFolder)
{
    if (!IsRelativePath()) return sCEmptyString;

    char	szAbsolutePath[_MAX_PATH];
    string sFullPath(szBaseFolder);

    if (sFullPath.empty()) return sCEmptyString;

    sFullPath = CPath::AddBackSlash(sFullPath.c_str());
    sFullPath += GetPath();

    if (!PathCanonicalize(szAbsolutePath, sFullPath.c_str())) return sCEmptyString;

    return szAbsolutePath;
}

string CPath::GetRelativePath(LPCTSTR szBaseFolder)
{
    if (IsRelativePath()) return sCEmptyString;

    char	szRelativePath[_MAX_PATH];
    string	sRelPath;

    PathRelativePathTo(szRelativePath, szBaseFolder, FILE_ATTRIBUTE_DIRECTORY, 
                    GetPath().c_str(), IsFilePath() ? 0 : FILE_ATTRIBUTE_DIRECTORY);

    sRelPath = szRelativePath;
    if (sRelPath.size() > 1)
    {
        // Remove ".\" from the beginning
        if ((sRelPath[0] == '.') && (sRelPath[1] == '\\'))
            sRelPath.substr(sRelPath.size() - 2);//   Right(sRelPath.GetLength() - 2);
    }

    return sRelPath;
}

string CPath::GetPath(BOOL bOriginal)
{
    string sPath;

    if (bOriginal)
        sPath = _sOriginalPath;
    else
        sPath = GetLocation() + GetFileName();

    return sPath;
}

string	CPath::GetShortPath()
{
    char szShortPath[_MAX_PATH];
    szShortPath[0] = 0x0;

    GetShortPathName(GetPath().c_str(), szShortPath, _MAX_PATH);

    return szShortPath;
}

string	CPath::GetLongPath()
{
    char szLongPath[_MAX_PATH];
    szLongPath[0] = 0x0;

    GetLongPathName(GetPath().c_str(), szLongPath, _MAX_PATH);

    return szLongPath;
}

string CPath::GetDrive()
{
    return _sDrive;
}

string	CPath::GetDriveLabel(BOOL bPCNameIfNetwork)
{
    if (_sDriveLabel.empty() && !IsRelativePath())
    {
        if ((bPCNameIfNetwork) && (!IsLocalPath()))
            _sDriveLabel = GetDir(0);
        else
        {
            string sRoot;
            char	szVolumeName[256];

            szVolumeName[0] = '\0';
            if (IsLocalPath())
            {
                sRoot = _sDrive + string("\\");
            }
            else if (GetDirCount() > 1)
            {
                //sRoot. format("\\\\%s\\%s\\", GetDir(0).c_str(), GetDir(1).c_str());
            }

            GetVolumeInformation(sRoot.c_str(), szVolumeName, 255, NULL, NULL, NULL, NULL, 0);

            _sDriveLabel = szVolumeName;
        }
    }

    return _sDriveLabel;
}

int	CPath::GetDirCount()
{
    FillDirArray();
    return _aDir.size();
}

string CPath::GetDir(int nIndex)
{
    if (nIndex < 0)
        return _sDir;
    else if (nIndex < GetDirCount())
    {
        FillDirArray();
        return _aDir[nIndex];
    }

    return sCEmptyString;
}

string	CPath::GetLocation()
{
    return _sDrive + GetDir();
}

string CPath::GetTitle()
{
    return _sFileTitle;
}

void CPath::SetTitle(const string &val)
{
    _sFileTitle = val;
}

string CPath::GetFileName()
{
    return _sFileTitle.empty() == TRUE ? _T("") :_sFileTitle+ _T(".")+GetExt();
}

string CPath::GetExt()
{
    return _sExtName;
}

void	CPath::SetExt(const string &val)
{
    _sExtName = val;
}

BOOL CPath::GetFileSize(__int64 &nSize)
{
    BOOL bResult;

    bResult = FillFileInfoStruct();
    nSize = ((__int64)_fis.nFileSizeHigh * (__int64)MAXDWORD) + (__int64)_fis.nFileSizeLow;
    return bResult;
}

BOOL CPath::GetFileTime(CTime &time, DWORD dwType)
{
    BOOL bResult;
    FILETIME *pTime = NULL;

    bResult = FillFileInfoStruct();
    switch (dwType)
    {
    case FILE_CREATION:	pTime = &_fis.ftCreationTime;	break;
    case FILE_WRITE:	pTime = &_fis.ftLastWriteTime;	break;
    case FILE_ACCESS:	
    default:			pTime = &_fis.ftLastAccessTime;	break;
    }

    if (pTime != NULL) time = CTime(*pTime);
    return bResult;
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Private methods

// This function must be called whenever _aDir array is needed, since this
// method is the one which parses _sDir and fill _aDir
void CPath::FillDirArray()
{
    if (_sDir.empty() || (_aDir.size() > 0)) return;

    int nFrom, nTo;

    // nFrom: 0 - relative / 1 - local / 2 - network
    nFrom = IsLocalPath() ? 1 : (IsRelativePath() ? 0 : 2);

    while ((nTo = _sDir.find('\\', nFrom)) != -1)
    {
        _aDir.push_back(_sDir.substr(nFrom, nTo - nFrom));
        nFrom = nTo + 1;
    }
}

BOOL CPath::FillFileInfoStruct()
{
    HANDLE	hFile;
    BOOL	bResult;

    ::memset(&_fis, 0, sizeof(_fis));

    hFile = CreateFile(GetPath().c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, 
                        OPEN_EXISTING, FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN |
                        FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM, NULL);

    if (hFile == INVALID_HANDLE_VALUE) return FALSE;

    bResult = GetFileInformationByHandle(hFile, &_fis);

    CloseHandle(hFile);

    return bResult;
}

string CPath::AddBackSlash(LPCTSTR szFolderPath, BOOL bInverted)
{
    string	sResult(szFolderPath);
    int		nLastChar = sResult.length() - 1;

    if (nLastChar >= 0)
    {
        if ((sResult[nLastChar] != '\\') && (sResult[nLastChar] != '/'))
            sResult += bInverted ? '/' : '\\';
    }

    return sResult;
}

string CPath::RemoveBackSlash(LPCTSTR szFolderPath)
{
    string	sResult(szFolderPath);
    int		nLastChar = sResult.size() - 1;

    if (nLastChar >= 0)
    {
        if ((sResult[nLastChar] == '\\') || (sResult[nLastChar] == '/'))
            sResult = sResult.substr(0, nLastChar);   // Left(nLastChar);
    }

    return sResult;
}

CPath::operator LPCTSTR ()
{
    _sLPCTSTRPath = GetPath();
    return _sLPCTSTRPath.c_str();
}

const CPath& CPath::operator = (LPCTSTR szPath)
{
    SetPath(szPath);
    return *this;
}

const CPath& CPath::operator = (CPath &ref)
{
    _aDir.clear();
    _aDir=ref._aDir;
    
    _bIsRelative = ref._bIsRelative;

    _fis = ref._fis;

    _sDir = ref._sDir;
    _sDrive = ref._sDrive;
    _sDriveLabel = ref._sDriveLabel;
    _sExtName = ref._sExtName;
    _sFileTitle = ref._sFileTitle;
    _sOriginalPath = ref._sOriginalPath;
    return *this;
}

BOOL IsDots(WIN32_FIND_DATA * pFindData)
{
    BOOL bResult = FALSE;
    if (pFindData != NULL && pFindData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    {
        if (pFindData->cFileName[0] == '.')
        {
            if (pFindData->cFileName[1] == '\0' ||
                (pFindData->cFileName[1] == '.' &&
                pFindData->cFileName[2] == '\0'))
            {
                bResult = TRUE;
            }
        }
    }
    return bResult;
}

BOOL CPath::GetAllFilesFromFolder(string strSourceFolder, BOOL bRecursively, strings &strList,LPCTSTR ext)
{
    strSourceFolder=AddBackSlash(strSourceFolder.c_str());

    string strExt(ext == NULL ? _T("*.*") : ext);
    WIN32_FIND_DATA FindFileData = {0};
    HANDLE hFind = FindFirstFile((strSourceFolder + strExt).c_str(), &FindFileData);
    if (hFind == INVALID_HANDLE_VALUE)
    {
		cout << "Error: " << strSourceFolder << " " << GetLastErrorAsString();// << endl;
        return FALSE;
    }
    do
    {
        if(IsDots(&FindFileData))
            continue;

        if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            if(bRecursively)
                GetAllFilesFromFolder(string(strSourceFolder + FindFileData.cFileName + _T("\\")), bRecursively, strList);
        }
        else
            strList.push_back(strSourceFolder + FindFileData.cFileName);
    }
    while (FindNextFile(hFind, &FindFileData));
    FindClose(hFind);
    return strList.size()!=0;
}

string CPath::GetAppPath()
{
    return CPath(GetAppName().c_str()).GetLocation();
}

string CPath::GetAppName()
{
    TCHAR file[_MAX_PATH]; 
    ::GetModuleFileName(NULL, file, _MAX_PATH);
    return file;
}

void CPath::AddDir(const string &val)
{
    _sDir = AddBackSlash(_sDir.c_str());
    _sDir +=val;
    _sDir = AddBackSlash(_sDir.c_str());
}

void CPath::SetCurDir()
{
    _tchdir(GetLocation().c_str());
}

void CPath::Empty()
{
    _sOriginalPath.clear();
    _sDrive.clear();
    _sDriveLabel.clear();
    _aDir.clear();
    _sDir.clear();
    _sFileTitle.clear();
    _sExtName.clear();
    _sLPCTSTRPath.clear();
}