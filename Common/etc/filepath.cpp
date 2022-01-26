
#include "stdafx.h"
#include "FilePath.h"
#include <shlwapi.h> // filesystem library
#include <io.h>
#include <Shellapi.h>
#pragma comment(lib, "shlwapi")
#include <sys/stat.h>
#include <shlobj.h>


namespace 
{
	//------------------------------------------------------------------------
	// srcFileName�� Ȯ���ڿ� compareExtendName �̸��� ���ٸ� true�� �����Ѵ�.
	// compareExtendName : .bmp, .jpg, .fbx
	//------------------------------------------------------------------------
	bool CompareExtendName(const char* srcFileName, const int srcStringMaxLength
		, const char* compareExtendName)
	{
		const int len = (int)strnlen_s(srcFileName, srcStringMaxLength);
		if (len <= 0)
			return FALSE;

		const int TEMPSIZE = 16;
		int count = 0;
		char temp[TEMPSIZE];
		for (int i = 0; i < len && i < (ARRAYSIZE(temp) - 1); ++i)
		{
			const char c = srcFileName[len - i - 1];
			temp[count++] = c;
			if ('.' == c)
				break;
		}
		temp[count] = NULL;

		char extendName[TEMPSIZE];
		for (int i = 0; i < count; ++i)
			extendName[i] = temp[count - i - 1];
		extendName[count] = NULL;

		return !strcmp(extendName, compareExtendName);
	}


	// compare extend filename
	// compareExtendName : .bmp, .jpg, .fbx
	bool CompareExtendName(const wchar_t* srcFileName, const int srcStringMaxLength
		, const wchar_t* compareExtendName)
	{
		const int len = (int)wcslen(srcFileName);
		if (len <= 0)
			return FALSE;

		const int TEMPSIZE = 16;
		int count = 0;
		wchar_t temp[TEMPSIZE];
		for (int i = 0; i < len && i < (ARRAYSIZE(temp) - 1); ++i)
		{
			const wchar_t c = srcFileName[len - i - 1];
			temp[count++] = c;
			if ('.' == c)
				break;
		}

		temp[count] = NULL;

		wchar_t extendName[TEMPSIZE];
		for (int i = 0; i < count; ++i)
			extendName[i] = temp[count - i - 1];
		extendName[count] = NULL;

		return !wcscmp(extendName, compareExtendName);
	}

}


common::StrPath common::RelativePathTo(const StrPath &pathFrom, const StrPath &pathTo)
{
	StrPath out;
	const BOOL result = PathRelativePathToA(out.m_str,
		pathFrom.c_str(), FILE_ATTRIBUTE_DIRECTORY,
		pathTo.c_str(), FILE_ATTRIBUTE_NORMAL);
	return out;
}

// ��� ����̸� true�� �����Ѵ�.
bool common::IsRelativePath(const StrPath &path)
{
	return PathIsRelativeA(path.c_str()) ? true : false;
}


//-----------------------------------------------------------------------------//
// searchPath������ findExt Ȯ���� ����Ʈ�� ���Ե� ������ out�� �����Ѵ�.
//
// searchPath: Ž���ϰ��� �ϴ� ���丮 ���
//		- �������� / �־���Ѵ�.
// findExt: ã���� �ϴ� Ȯ����, 2���̻� �����Ҽ��ְ� �ϱ����ؼ� ����Ʈ �ڷ����°� �Ǿ���.
//			ex) .bmp, .jpg, .png
// out: ��ġ�ϴ� Ȯ���ڸ� ���� �����̸��� �����Ѵ�.
//-----------------------------------------------------------------------------//
bool common::CollectFiles(const vector<WStr32> &findExt, const wchar_t *searchPath
	, OUT vector<WStrPath> &out)
{
	WStrPath modifySearchPath;
	//if (!searchPath.empty() &&
	//	(searchPath[searchPath.size() - 1] == '/') || (searchPath[searchPath.size() - 1] == '\\'))
	const int searchLen = wcslen(searchPath);
	if ((searchLen != 0) &&
		(searchPath[searchLen - 1] == '/') || (searchPath[searchLen - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath;
		modifySearchPath += L"\\";
	}

	WIN32_FIND_DATAW fd;
	WStrPath searchDir = modifySearchPath + L"*.*";
	HANDLE hFind = FindFirstFileW(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(L".", fd.cFileName) && wcscmp(L"..",fd.cFileName))
			{
				const WStrPath newPath = modifySearchPath + fd.cFileName + L"/";
				CollectFiles(findExt, newPath.c_str(), out);
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fd.cFileName);
			}
			else
			{
				for (auto &ext : findExt)
				{
					if (CompareExtendName(fd.cFileName, wcslen(fd.cFileName), ext.c_str()))
					{
						out.push_back(modifySearchPath + fd.cFileName);
						break;
					}
				}
			}
		}

		if (!FindNextFileW(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


//-----------------------------------------------------------------------------//
// searchPath������ findExt Ȯ���� ����Ʈ�� ���Ե� ������ out�� �����Ѵ�.
//
// searchPath: Ž���ϰ��� �ϴ� ���丮 ���
//		- �������� / �־���Ѵ�.
// findExt: ã���� �ϴ� Ȯ����, 2���̻� �����Ҽ��ְ� �ϱ����ؼ� ����Ʈ �ڷ����°� �Ǿ���.
//			ex) .bmp, .jpg, .png
// out: ��ġ�ϴ� Ȯ���ڸ� ���� �����̸��� �����Ѵ�.
//-----------------------------------------------------------------------------//
bool common::CollectFiles(const list<WStr32> &findExt, const wchar_t *searchPath
	, OUT list<WStrPath> &out)
{
	WStrPath modifySearchPath;
	//if (!searchPath.empty() &&
	//	(searchPath[searchPath.size() - 1] == '/') || (searchPath[searchPath.size() - 1] == '\\'))
	const int searchLen = wcslen(searchPath);
	if ((searchLen != 0) &&
		(searchPath[searchLen - 1] == '/') || (searchPath[searchLen - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath;
		modifySearchPath += L"\\";
	}

	WIN32_FIND_DATAW fd;
	WStrPath searchDir = modifySearchPath + L"*.*";
	HANDLE hFind = FindFirstFileW(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(L".", fd.cFileName) && wcscmp(L"..", fd.cFileName))
			{
				const WStrPath newPath = modifySearchPath + fd.cFileName + L"/";
				CollectFiles(findExt, newPath.c_str(), out);
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fd.cFileName);
			}
			else
			{
				for (auto &ext : findExt)
				{
					if (CompareExtendName(fd.cFileName, wcslen(fd.cFileName), ext.c_str()))
					{
						out.push_back(modifySearchPath + fd.cFileName);
						break;
					}
				}
			}
		}

		if (!FindNextFileW(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


//-----------------------------------------------------------------------------//
// searchPath������ findExt Ȯ���� ����Ʈ�� ���Ե� ������ out�� �����Ѵ�.
//	ignoreDirs �� ���Ե� ������ �˻����� ���ܵȴ�.
//
// searchPath: Ž���ϰ��� �ϴ� ���丮 ���
//		- �������� / �־���Ѵ�.
// findExt: ã���� �ϴ� Ȯ����, 2���̻� �����Ҽ��ְ� �ϱ����ؼ� ����Ʈ �ڷ����°� �Ǿ���.
//			ex) .bmp, .jpg, .png
// out: ��ġ�ϴ� Ȯ���ڸ� ���� �����̸��� �����Ѵ�.
//-----------------------------------------------------------------------------//
bool common::CollectFiles3(const vector<WStr32> &findExt, const wchar_t *searchPath
	, const vector<WStr64> &ignoreDirs, OUT vector<WStrPath> &out)
{
	WStrPath modifySearchPath;
	const int searchLen = wcslen(searchPath);
	if ((searchLen != 0) &&
		(searchPath[searchLen - 1] == '/') || (searchPath[searchLen - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath;
		modifySearchPath += L"\\";
	}

	WIN32_FIND_DATAW fd;
	WStrPath searchDir = modifySearchPath + L"*.*";
	HANDLE hFind = FindFirstFileW(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(L".", fd.cFileName) && wcscmp(L"..", fd.cFileName))
			{
				if (ignoreDirs.end() == std::find(ignoreDirs.begin(), ignoreDirs.end(), fd.cFileName))
				{
					const WStrPath newPath = modifySearchPath + fd.cFileName + L"/";
					CollectFiles(findExt, newPath.c_str(), out);
				}
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fd.cFileName);
			}
			else
			{
				for (auto &ext : findExt)
				{
					if (CompareExtendName(fd.cFileName, wcslen(fd.cFileName), ext.c_str()))
					{
						out.push_back(modifySearchPath + fd.cFileName);
						break;
					}
				}
			}
		}

		if (!FindNextFileW(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


//-----------------------------------------------------------------------------//
// searchPath������ findExt Ȯ���� ����Ʈ�� ���Ե� ������ out�� �����Ѵ�.
//	ignoreDirs �� ���Ե� ������ �˻����� ���ܵȴ�.
//
// searchPath: Ž���ϰ��� �ϴ� ���丮 ���
//		- �������� / �־���Ѵ�.
// findExt: ã���� �ϴ� Ȯ����, 2���̻� �����Ҽ��ְ� �ϱ����ؼ� ����Ʈ �ڷ����°� �Ǿ���.
//			ex) .bmp, .jpg, .png
// out: ��ġ�ϴ� Ȯ���ڸ� ���� �����̸��� �����Ѵ�.
//-----------------------------------------------------------------------------//
bool common::CollectFiles3(const list<WStr32> &findExt, const wchar_t *searchPath
	, const list<WStr64> &ignoreDirs, OUT list<WStrPath> &out)
{
	WStrPath modifySearchPath;
	const int searchLen = wcslen(searchPath);
	if ((searchLen != 0) &&
		(searchPath[searchLen - 1] == '/') || (searchPath[searchLen - 1] == '\\'))
	{
		modifySearchPath = searchPath;
	}
	else
	{
		modifySearchPath = searchPath;
		modifySearchPath += L"\\";
	}

	WIN32_FIND_DATAW fd;
	WStrPath searchDir = modifySearchPath + L"*.*";
	HANDLE hFind = FindFirstFileW(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (wcscmp(L".", fd.cFileName) && wcscmp(L"..", fd.cFileName))
			{
				if (ignoreDirs.end() == std::find(ignoreDirs.begin(), ignoreDirs.end(), fd.cFileName))
				{
					const WStrPath newPath = modifySearchPath + fd.cFileName + L"/";
					CollectFiles(findExt, newPath.c_str(), out);
				}
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			if (findExt.empty())
			{
				out.push_back(modifySearchPath + fd.cFileName);
			}
			else
			{
				for (auto &ext : findExt)
				{
					if (CompareExtendName(fd.cFileName, wcslen(fd.cFileName), ext.c_str()))
					{
						out.push_back(modifySearchPath + fd.cFileName);
						break;
					}
				}
			}
		}

		if (!FindNextFileW(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return true;
}


// searchPath ���丮 �ȿ��� findName �� �����̸��� ���� ������ �ִٸ� �ش� ��θ�
// out �� �����ϰ� true �� �����Ѵ�.
// depth ũ�⸸ŭ ���� ���丮�� �˻��Ѵ�.  -1�̸� ������ �˻��Ѵ�.
bool common::FindFile( const StrPath &findName, const StrPath &searchPath, StrPath &out
	, const int depth //= -1
)
{
	if (depth == 0)
		return false;

	StrPath lowerCaseFindFileName = findName;
	lowerCaseFindFileName.lowerCase();

	WIN32_FIND_DATAA fd;
	const StrPath searchDir = searchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if (strcmp(".", fd.cFileName) && strcmp("..", fd.cFileName))
			{
				if (FindFile( findName, searchPath + fd.cFileName + "/", out, depth-1 ))
					break;
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			StrPath fileName = fd.cFileName;
			// �ӵ��� �������� �ּ�ó����, GetFileName()ȣ���� �ܺο��� �� ��
			//if (lowerCase(fileName) == lowerCase(GetFileName(findName)))
			if (fileName.lowerCase() == lowerCaseFindFileName)
			{
				out = searchPath + findName.GetFileName();
				break;
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return !out.empty();
}


// searchPath ���丮 �ȿ��� findName �� �����̸��� ���� ������ �ִٸ� �ش� ��θ�
// out �� �����ϰ� true �� �����Ѵ�.
// depth ũ�⸸ŭ ���� ���丮�� �˻��Ѵ�.  -1�̸� ������ �˻��Ѵ�.
// ignoreDirs �� ���Ե� �������� �˻����� ���ܵȴ�.
bool common::FindFile2(const StrPath &findName, const StrPath &searchPath
	, const list<string> &ignoreDirs, StrPath &out
	, const int depth //= -1
)
{
	if (depth == 0)
		return false;

	StrPath lowerCaseFindFileName = findName;
	lowerCaseFindFileName.lowerCase();

	WIN32_FIND_DATAA fd;
	const StrPath searchDir = searchPath + "*.*";
	HANDLE hFind = FindFirstFileA(searchDir.c_str(), &fd);

	while (1)
	{
		if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (strcmp(".", fd.cFileName) && strcmp("..", fd.cFileName))
			{
				if (ignoreDirs.end() == std::find(ignoreDirs.begin(), ignoreDirs.end(), fd.cFileName))
				{
					if (FindFile(findName, searchPath + fd.cFileName + "/", out, depth - 1))
						break;
				}
			}
		}
		else if (fd.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		{
			StrPath fileName = fd.cFileName;
			// �ӵ��� �������� �ּ�ó����, GetFileName()ȣ���� �ܺο��� �� ��
			//if (lowerCase(fileName) == lowerCase(GetFileName(findName)))
			if (fileName.lowerCase() == lowerCaseFindFileName)
			{
				out = searchPath + findName.GetFileName();
				break;
			}
		}

		if (!FindNextFileA(hFind, &fd))
			break;
	}

	FindClose(hFind);

	return !out.empty();
}


bool common::IsFileExist(const StrPath &fileName) {
	return _access_s(fileName.c_str(), 0) == 0;
}

bool common::IsDirectoryExist(const StrPath &fileName) {
	return IsDirectoryExist(fileName.c_str());
}

