#pragma once
//////////////////////////////////////////////////////////////////////////

#include <filesystem>
namespace fs = std::filesystem;

class CCopy
{
protected:
	fileVector &src;
	fs::path dst;
	fileDataVector dst_list;
	stringVector mask;
	bool s;
	bool d;
	bool modified_only;

	bool CollectFiles();
	void Find(const FileData &, fileDataVector &);
	bool CheckDir();
	bool CheckFilesForCopy(const FileData &, const FileData &);
	void MakeMaskVector();
public:
	CCopy(fileVector& source);
	~CCopy();
	//void SetSource(fileVector& source);
	void SetDestination(const fs::path& destination);
	void SetRecursive(bool s, bool d);
	void SetModifiedOnly(bool m);
	bool Process();

};
//////////////////////////////////////////////////////////////////////////
