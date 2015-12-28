#ifndef __FILENAME_H
#define __FILENAME_H

/*****************************************************************************
 *
 *	  FILE:    filename.h
 *
 *    PURPOSE: routines that handle different aspects of filenames
 *
 *    DATE       DESCRIPTION
 *	             Initial coding by JS
 *
 *****************************************************************************/

/// given a full path, returns just the filename and extension
CString justFileName(LPCSTR);

/// given a full path, returns just the filename
CString justName(LPCSTR);

/// given a full path, returns just the extension
CString justExtention(LPCSTR);

///  given a full path, returns just the drive and directory
CString justPath(LPCSTR);

/// given a full path, returns just the drive
CString justDrive(LPCSTR);

/// given a filename (with or without a full path), changes the extension
CString forceExtention(CString fn, CString newext);

/// given a filename (with or without a full path), changes the filename leaving the extension the same
CString forceFileName(CString fn, CString newname);

CString forcePath(CString fn, CString newpath);

BOOL FileExists(CString fn);
BOOL ValidFileName(CString fn, BOOL b8dot3=FALSE);

int DeleteFile(CString fname);
int RenameFile(CString oldname, CString newname);
void AddBackSlash(CString& path);
void AddBackSlash(char* path);
CString RemoveBackSlash(CString path);
CString GetProgramPath(BOOL bAndExeName=FALSE);

#endif

