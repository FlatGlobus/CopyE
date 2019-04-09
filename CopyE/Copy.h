#pragma once
//////////////////////////////////////////////////////////////////////////

#include <filesystem>
namespace fs = std::experimental::filesystem;

class CCopy
{
protected:
	fileVector &src;
	fs::path dst;
	fileVector dst_list;
	stringVector mask;
	bool s;
	bool d;

	bool CollectFiles();
	void Find(const fs::path &, fileVector &);
	bool CheckDir();
	bool CheckFilesForCopy(const fs::path &, const fs::path &);
	void MakeMaskVector();
public:
	CCopy(fileVector& source);
	~CCopy();
	//void SetSource(fileVector& source);
	void SetDestination(const fs::path& destination);
	void SetRecursive(bool s, bool d);
	bool Process();

};
//////////////////////////////////////////////////////////////////////////
