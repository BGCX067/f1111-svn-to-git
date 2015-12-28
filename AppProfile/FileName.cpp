/*******************************************************************************
 *
 *	  FILE:    filename.cpp
 *
 *    PURPOSE: routines that handle different aspects of filenames
 *
 *    DATE       DESCRIPTION
 *	             Initial coding by JS
 *
 ******************************************************************************/

#include "stdafx.h"
#include "FileName.h"
#include <io.h>

//------------------------------------------------------------------------------
CString justFileName(LPCSTR f)                                               
{
  char drive[_MAX_DRIVE],
        dir[_MAX_DIR],
        file[_MAX_FNAME],
        ext[_MAX_EXT];
  CString st;
  _splitpath_s((char *)f, drive, _MAX_DRIVE, dir, _MAX_DIR, file, _MAX_FNAME, ext, _MAX_EXT);
  st = file;
  st += ext;
  return st;
}

//------------------------------------------------------------------------------
CString justName(LPCSTR f)
{
  char drive[_MAX_DRIVE],
        dir[_MAX_DIR],
        file[_MAX_FNAME],
        ext[_MAX_EXT];
  CString st;
  _splitpath_s((char *)f, drive, _MAX_DRIVE, dir, _MAX_DIR, file, _MAX_FNAME, ext, _MAX_EXT);
  st = file;
  return st;
}


//------------------------------------------------------------------------------
CString justPath(LPCSTR f)
{
  char drive[_MAX_DRIVE],
        dir[_MAX_DIR],
        file[_MAX_FNAME],
        ext[_MAX_EXT];
  CString st;
  _splitpath_s((char *)f, drive, _MAX_DRIVE, dir, _MAX_DIR, file, _MAX_FNAME, ext, _MAX_EXT);
  st = drive;
  st += dir;
  return st;
}   

//------------------------------------------------------------------------------
CString justExtention(LPCSTR f)
{
  char drive[_MAX_DRIVE],
       dir[_MAX_DIR],
       file[_MAX_FNAME],
       ext[_MAX_EXT];
  CString st;
  _splitpath_s((char *)f, drive, _MAX_DRIVE, dir, _MAX_DIR, file, _MAX_FNAME, ext, _MAX_EXT);
  st = ext;
  return st;
}

//------------------------------------------------------------------------------
CString forceExtention(CString fn, CString newext)
{
  CString st = justPath(fn);
  st += justName(fn);
  st += newext;
  return st;
}

//------------------------------------------------------------------------------
CString forceFileName(CString fn, CString newname)
{
  CString st = justPath(fn);
  st += newname;
  st += justExtention(fn);
  return st;
}

//------------------------------------------------------------------------------
CString forcePath(CString fn, CString newpath)
{
  CString st = newpath;
  AddBackSlash(st);
  st += justFileName(fn);
  return st;
}

//------------------------------------------------------------------------------
BOOL ValidFileName(CString fn, BOOL b8dot3/*=FALSE*/)
{
	CString name = justName(fn);
	CString ext  = justExtention(fn);

	if (b8dot3)
	{
		if (name.GetLength() > 8 || ext.GetLength() > 4)
		{
			return FALSE;
		}
		if (name.FindOneOf(" *#%;/\\") >= 0 || ext.FindOneOf(" *#%;/\\") >= 0)
		{
			return FALSE;
		}
	}
	if (name.FindOneOf(" *#%;/") >= 0 || ext.FindOneOf(" *#%;/") >= 0)
	{
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------------------------------------------
BOOL FileExists(CString fn)
{
	BOOL exists = FALSE;                   // assume failure
	if (!fn.IsEmpty())// && ValidFileName(fn)) // if we have a filename and it appears to be valid
	{
		int status = _access(fn,0);
		if (status == 0)
		{
			exists = TRUE;
		}
	}
	return exists;
}

//------------------------------------------------------------------------------
int RenameFile(CString oldname, CString newname)
{
	int  status    = 0;
	BOOL oldexists = FileExists(oldname);
	BOOL newexists = FileExists(newname);

	if (oldexists && !newexists)
	{
		CFile::Rename(oldname, newname);
	}
	else
	{
		status = -1;
	}
	return status;
}

//------------------------------------------------------------------------------
int DeleteFile(CString fname)
{
	int  status    = 0;
	if (FileExists(fname))
	{
		CFile::Remove(fname);
	}
	else
	{
		status = -1;
	}
	return status;
}

//------------------------------------------------------------------------------
// Add a back slash to the end of the path if it doesn't exist.	
//------------------------------------------------------------------------------
void AddBackSlash(CString& path)
{
	int length = path.GetLength();

	if (length > 1)
	{
		if (path.GetAt(length - 1) != '\\')
		{
			path += "\\";
		}
	}
	else if (1 == length)
	{
		CString temp = path;
		temp.MakeUpper();
		TCHAR ch = temp.GetAt(0); 

		if (ch >= 'A' && ch <= 'Z')
		{
			path += ":\\";
		}
		else if (ch != '\\')  // add a back slash anyway
		{
			path += "\\";
		}
	}
	else
	{
		path += "\\";
	}
}

//------------------------------------------------------------------------------
//	Add a back slash to the end of the path if it doesn't exist.	
//------------------------------------------------------------------------------
void AddBackSlash(char* path)
{
	CString cs(path);
	AddBackSlash(cs);
	strcpy_s(path, cs.GetLength(), (LPCTSTR)cs);
}

//------------------------------------------------------------------------------
CString RemoveBackSlash(CString path)
{
	if (path.Find('\\') == 0)
	{
		return path;
	}
	int len = path.GetLength();
	path = path.Left(len - 1);
	return path;
}

//-------------------------------------------------------------------------------
CString GetProgramPath(BOOL bAndExeName/*=FALSE*/)
{
	TCHAR szFullPath[MAX_PATH];
	::GetModuleFileName(NULL, szFullPath, MAX_PATH);
	CString path = szFullPath;
	if (!bAndExeName)
	{
		path = justPath(path);
		AddBackSlash(path);
	}
	return path;
}

