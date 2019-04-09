//////////////////////////////////////////////////////////////////////

#if !defined(__PATH_H__)
#define __PATH_H__

#include <string>
#include <vector>
#include <time.h>
#include <atltime.h>


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

const char	sCEmptyString = 0x0;
enum		{FILE_CREATION, FILE_ACCESS, FILE_WRITE};

using namespace::std;

typedef vector<string> strings;
typedef vector<string>::iterator stringsIterator;
typedef vector<string>::reverse_iterator RstringsIterator;

class CPath  
{
public:
	CPath();
	CPath(LPCTSTR szPath, BOOL bIsFolderPath = FALSE);
	virtual ~CPath();

	// Parses a path or PATH_CMDLINE, PATH_MODULE
	void	SetPath(LPCTSTR, BOOL bIsFolderPath = FALSE);
	string	GetPath(BOOL bOriginal = FALSE);
	string	GetShortPath();
	string	GetLongPath();

	BOOL IsLocalPath();
	BOOL IsRelativePath();
	BOOL IsFilePath();

	BOOL ExistFile();
	BOOL ExistLocation();


	// If the path set in the object is not a relative one returns empty
	string	GetAbsolutePath(LPCTSTR szBaseFolder);

	// If the path set in the object is a relative one returns empty
	string	GetRelativePath(LPCTSTR szBaseFolder);

	// Get drive string (empty for a network path) [e.g.: "c:"]
	string	GetDrive();

	// Get drive label (pc name for a network path) [e.g.: "MAIN_HD"]
	string	GetDriveLabel(BOOL bPCNameIfNetwork = FALSE);

	// Get folder count in path [e.g.: 2 for "c:\folder\subfolder\file.ext"]
	int		GetDirCount();

	// Get 0 based nIndex folder string [e.g.: "folder" for nIndex = 0]
	// If nIndex = -1 the return string is the full dir string 
	// [e.g.: "\folder\subfolder\" or "\\pcname\folder\" for non-local]
	// If it's a relative path no "\" is added at the beginning [e.g.: "..\sub\"]
	string	GetDir(int nIndex = -1);

    //Dirname like "asd" not "\asd"
    void AddDir(const string&);

	// File location or directory path [e.g.: "c:\folder\subfolder\"]
	string	GetLocation();

	// File title string (without extension) [e.g.: "file" for "..\file.ext"]
	string	GetTitle();

    void SetTitle(const string&);

	// Filename = File title + extension [e.g.: "file.ext"]
	string	GetFileName();

	// Extension name (dot not included) [e.g.: "ext"]
	string	GetExt();

    void	SetExt(const string&);

	// Get the size in bytes of the current file
	BOOL	GetFileSize(__int64 &nSize);

	// Get the size in bytes of the current file
	// values: FILE_CREATION, FILE_ACCESS, FILE_WRITE
	BOOL	GetFileTime(CTime &time, DWORD dwType = FILE_WRITE);

	// Return a temporary character pointer to the path data.
	operator LPCTSTR ();

	// Same as SetPath(szPath, FALSE, FALSE)
	const CPath& operator = (LPCTSTR szPath);

	// Makes a copy of the object
	const CPath& operator = (CPath &ref);

    void SetCurDir();
	// Add a back slash ('\' or '/' if bInverted is TRUE) if necessary
	static string AddBackSlash(LPCTSTR szFolderPath, BOOL bInverted = FALSE);

	// Removes a trailing back slash if found
	static string  RemoveBackSlash(LPCTSTR szFolderPath);
    static  BOOL    GetAllFilesFromFolder(string folder, BOOL bRecursively, strings &strList,LPCTSTR ext=NULL);
    static string  GetAppPath();
    static string  GetAppName();
    void Empty();    
private:
    void FillDirArray();
    BOOL FillFileInfoStruct();

	string			_sOriginalPath;
	string			_sDrive;
	string			_sDriveLabel;
	strings			_aDir;
	string			_sDir;
	string			_sFileTitle;
	string			_sExtName;
	BOOL			_bIsRelative;
	BY_HANDLE_FILE_INFORMATION _fis;
	string			_sLPCTSTRPath;
};

#endif // !defined(__PATH_H__)
