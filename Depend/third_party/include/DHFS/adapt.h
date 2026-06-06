//
//  "$Id: adapt.h 214 2008-02-22 11:18:56Z wang_haifeng $"
//
//  Copyright (c)1992-2007, ZheJiang Dahua Technology Stock CO.LTD.
//  All Rights Reserved.
//
//	Description:	
//	Revisions:		Year-Month-Day  SVN-Author  Modification
//

#ifndef __DHFS_ADAPT_H_
#define __DHFS_ADAPT_H_

#include <stdio.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/// 文件系统查找结果
struct FSFindData
{
	unsigned    attrib;
	time_t      time_create;    /* -1 for FAT file systems */
	time_t      time_access;    /* -1 for FAT file systems */
	time_t      time_write;
	size_t      size;
	char        name[260];
};

// 文件系统标准入口
FILE* dh_fopen(const char *, const char *);
int dh_fclose(FILE *);
size_t dh_fread(void *, size_t, size_t, FILE *);
size_t dh_fwrite(const void *, size_t, size_t, FILE *);
int dh_fflush(FILE *);
int dh_fseek(FILE *, long, int);
long dh_ftell(FILE *);
char * dh_fgets(char *, int, FILE *);
int dh_fputs( const char *, FILE * );
int dh_rename(const char *oldname, const char *newname);
int dh_remove(const char *path);
long dh_findfirst(const char *, void *);
int dh_findnext(long, void *);
int dh_findclose(long);
int dh_mkdir( const char *dirname);
int dh_rmdir( const char *dirname);
int dh_statfs( const char *path, uint64* userFreeBytes, uint64* totalBytes, uint64* totalFreeBytes);

// 控制台接口
int dh_console(int argc, char * argv[]);

void dh_use_adapt(int enable);

#ifdef __cplusplus
}
#endif

#endif //__DHFS_H_

