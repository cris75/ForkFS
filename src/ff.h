/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem module  R0.13                               /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2017, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:

/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/


#ifndef FF_DEFINED
#define FF_DEFINED

#ifdef __cplusplus
extern "C" {
#endif

#include "ffconf.h"        /* FatFs configuration options */

#include <stdint.h>

/* Definitions of volume management */

typedef uint16_t WCHAR ;

#if FF_MULTI_PARTITION        /* Multiple partition configuration */
typedef struct {
    uint8_t pd;    /* Physical drive number */
    uint8_t pt;    /* Partition: 0:Auto detect, 1-4:Forced partition) */
} PARTITION;
extern PARTITION VolToPart[];    /* Volume - Partition resolution table */
#endif

/* Type of path name strings on FatFs API */

#if FF_LFN_UNICODE && FF_USE_LFN    /* Unicode (UTF-16) string */
  #ifndef _INC_TCHAR
    typedef WCHAR TCHAR;
    
    #ifndef _T
      #define _T(x) L ## x
    #endif
    
    #ifndef _TEXT
      #define _TEXT(x) L ## x
    #endif

    #define _INC_TCHAR
  #endif
#else                        /* ANSI/OEM string */
  #ifndef _INC_TCHAR
    typedef char TCHAR;

    #ifndef _T
      #define _T(x) x
    #endif

    #ifndef _TEXT
      #define _TEXT(x) x
    #endif

    #define _INC_TCHAR
  #endif
#endif



/* Type of file size variables */

#if FF_FS_EXFAT
#if !FF_USE_LFN
#error LFN must be enabled when enable exFAT
#endif
typedef uint64_t FSIZE_t;
#else
typedef uint32_t FSIZE_t;
#endif

/* Filesystem type (FATFS.fs_type) */
typedef enum _FS_TYPE_T_
{
    fsType_NO_FILESYSTEM ,
    fsType_FAT12 ,
    fsType_FAT16 ,
    fsType_FAT32 ,
    fsType_EXFAT
} fs_type_t ;


/* Filesystem object structure (FATFS) */

typedef struct {
    fs_type_t    fs_type;        /* Filesystem type (0:N/A) */
    uint8_t    pdrv;            /* Physical drive number */
    uint8_t    n_fats;            /* Number of FATs (1 or 2) */
    uint8_t    wflag;            /* win[] flag (b0:dirty) */
    uint8_t    fsi_flag;        /* FSINFO flags (b7:disabled, b0:dirty) */
    uint16_t    id;                /* Volume mount ID */
    uint16_t    n_rootdir;        /* Number of root directory entries (FAT12/16) */
    uint16_t    csize;            /* Cluster size [sectors] */
#if FF_MAX_SS != FF_MIN_SS
    uint16_t    ssize;            /* Sector size (512, 1024, 2048 or 4096) */
#endif
#if FF_USE_LFN
    WCHAR*    lfnbuf;            /* LFN working buffer */
#endif
#if FF_FS_EXFAT
    uint8_t *    dirbuf;            /* Directory entry block scratchpad buffer for exFAT */
#endif
#if FF_FS_REENTRANT
    FF_SYNC_t    sobj;        /* Identifier of sync object */
#endif
    uint32_t    last_clst;        /* Last allocated cluster */
    uint32_t    free_clst;        /* Number of free clusters */
    uint32_t    cdir;            /* Current directory start cluster (0:root) */
#if FF_FS_EXFAT
    uint32_t    cdc_scl;        /* Containing directory start cluster (invalid when cdir is 0) */
    uint32_t    cdc_size;        /* b31-b8:Size of containing directory, b7-b0: Chain status */
    uint32_t    cdc_ofs;        /* Offset in the containing directory (invalid when cdir is 0) */
#endif
    uint32_t    n_fatent;        /* Number of FAT entries (number of clusters + 2) */
    uint32_t    fsize;            /* Size of an FAT [sectors] */
    uint32_t    volbase;        /* Volume base sector */
    uint32_t    fatbase;        /* FAT base sector */
    uint32_t    dirbase;        /* Root directory base sector/cluster */
    uint32_t    database;        /* Data base sector */
    uint32_t    winsect;        /* Current sector appearing in the win[] */
    uint8_t    win[FF_MAX_SS];    /* Disk access window for Directory, FAT (and file data at tiny cfg) */
} FATFS;



/* Object ID and allocation information (FFOBJID) */

typedef struct {
    FATFS*    fs;                /* Pointer to the hosting volume of this object */
    uint16_t    id;                /* Hosting volume mount ID */
    uint8_t    attr;            /* Object attribute */
    uint8_t    stat;            /* Object chain status (b1-0: =0:not contiguous, =2:contiguous, =3:flagmented in this session, b2:sub-directory stretched) */
    uint32_t    sclust;            /* Object data start cluster (0:no cluster or root directory) */
    FSIZE_t    objsize;        /* Object size (valid when sclust != 0) */
#if FF_FS_EXFAT
    uint32_t    n_cont;            /* Size of first fragment - 1 (valid when stat == 3) */
    uint32_t    n_frag;            /* Size of last fragment needs to be written to FAT (valid when not zero) */
    uint32_t    c_scl;            /* Containing directory start cluster (valid when sclust != 0) */
    uint32_t    c_size;            /* b31-b8:Size of containing directory, b7-b0: Chain status (valid when c_scl != 0) */
    uint32_t    c_ofs;            /* Offset in the containing directory (valid when file object and sclust != 0) */
#endif
#if FF_FS_LOCK
    unsigned int    lockid;            /* File lock ID origin from 1 (index of file semaphore table Files[]) */
#endif
} FFOBJID;



/* File object structure (FIL) */

typedef struct {
    FFOBJID    obj;            /* Object identifier (must be the 1st member to detect invalid object pointer) */
    uint8_t    flag;            /* File status flags */
    uint8_t    err;            /* Abort flag (error code) */
    FSIZE_t    fptr;            /* File read/write pointer (Zeroed on file open) */
    uint32_t    clust;            /* Current cluster of fpter (invalid when fptr is 0) */
    uint32_t    sect;            /* Sector number appearing in buf[] (0:invalid) */
    uint32_t    dir_sect;        /* Sector number containing the directory entry (not used at exFAT) */
    uint8_t *    dir_ptr;        /* Pointer to the directory entry in the win[] (not used at exFAT) */
    uint32_t*    cltbl;            /* Pointer to the cluster link map table (nulled on open, set by application) */
    uint8_t    buf[FF_MAX_SS];    /* File private data read/write window */
} FIL;



/* Directory object structure (DIR) */

typedef struct {
    FFOBJID    obj;            /* Object identifier */
    uint32_t    dptr;            /* Current read/write offset */
    uint32_t    clust;            /* Current cluster */
    uint32_t    sect;            /* Current sector (0:Read operation has terminated) */
    uint8_t *    dir;            /* Pointer to the directory item in the win[] */
    uint8_t     fn[12];            /* SFN (in/out) {body[8],ext[3],status[1]} */
#if FF_USE_LFN
    uint32_t    blk_ofs;        /* Offset of current entry block being processed (0xFFFFFFFF:Invalid) */
#endif
#if FF_USE_FIND
    const TCHAR* pat;        /* Pointer to the name matching pattern */
#endif
} DIR;



/* File information structure (FILINFO) */

typedef struct {
    FSIZE_t    fsize;            /* File size */
    uint16_t    fdate;            /* Modified date */
    uint16_t    ftime;            /* Modified time */
    uint8_t    fattrib;        /* File attribute */
#if FF_USE_LFN
    TCHAR    altname[13];            /* Altenative file name */
    TCHAR    fname[FF_MAX_LFN + 1];    /* Primary file name */
#else
    TCHAR    fname[13];        /* File name */
#endif
} FILINFO;



/* File function return code (FRESULT) */

typedef enum {
    FR_OK = 0,                /* (0) Succeeded */
    FR_DISK_ERR,            /* (1) A hard error occurred in the low level disk I/O layer */
    FR_INT_ERR,                /* (2) Assertion failed */
    FR_NOT_READY,            /* (3) The physical drive cannot work */
    FR_NO_FILE,                /* (4) Could not find the file */
    FR_NO_PATH,                /* (5) Could not find the path */
    FR_INVALID_NAME,        /* (6) The path name format is invalid */
    FR_DENIED,                /* (7) Access denied due to prohibited access or directory full */
    FR_EXIST,                /* (8) Access denied due to prohibited access */
    FR_INVALID_OBJECT,        /* (9) The file/directory object is invalid */
    FR_WRITE_PROTECTED,        /* (10) The physical drive is write protected */
    FR_INVALID_DRIVE,        /* (11) The logical drive number is invalid */
    FR_NOT_ENABLED,            /* (12) The volume has no work area */
    FR_NO_FILESYSTEM,        /* (13) There is no valid FAT volume */
    FR_MKFS_ABORTED,        /* (14) The f_mkfs() aborted due to any problem */
    FR_TIMEOUT,                /* (15) Could not get a grant to access the volume within defined period */
    FR_LOCKED,                /* (16) The operation is rejected according to the file sharing policy */
    FR_NOT_ENOUGH_CORE,        /* (17) LFN working buffer could not be allocated */
    FR_TOO_MANY_OPEN_FILES,    /* (18) Number of open files > FF_FS_LOCK */
    FR_INVALID_PARAMETER    /* (19) Given parameter is invalid */
} FRESULT;



/*--------------------------------------------------------------*/
/* FatFs module application interface                           */

FRESULT f_open (FIL* fp, const TCHAR* path, uint8_t mode);                /* Open or create a file */
FRESULT f_close (FIL* fp);                                            /* Close an open file object */
FRESULT f_read (FIL* fp, void* buff, unsigned int btr, unsigned int* br);            /* Read data from the file */
FRESULT f_write (FIL* fp, const void* buff, unsigned int btw, unsigned int* bw);    /* Write data to the file */
FRESULT f_lseek (FIL* fp, FSIZE_t ofs);                                /* Move file pointer of the file object */
FRESULT f_truncate (FIL* fp);                                        /* Truncate the file */
FRESULT f_sync (FIL* fp);                                            /* Flush cached data of the writing file */
FRESULT f_opendir (DIR* dp, const TCHAR* path);                        /* Open a directory */
FRESULT f_closedir (DIR* dp);                                        /* Close an open directory */
FRESULT f_readdir (DIR* dp, FILINFO* fno);                            /* Read a directory item */
FRESULT f_findfirst (DIR* dp, FILINFO* fno, const TCHAR* path, const TCHAR* pattern);    /* Find first file */
FRESULT f_findnext (DIR* dp, FILINFO* fno);                            /* Find next file */
FRESULT f_mkdir (const TCHAR* path);                                /* Create a sub directory */
FRESULT f_unlink (const TCHAR* path);                                /* Delete an existing file or directory */
FRESULT f_rename (const TCHAR* path_old, const TCHAR* path_new);    /* Rename/Move a file or directory */
FRESULT f_stat (const TCHAR* path, FILINFO* fno);                    /* Get file status */
FRESULT f_chmod (const TCHAR* path, uint8_t attr, uint8_t mask);            /* Change attribute of a file/dir */
FRESULT f_utime (const TCHAR* path, const FILINFO* fno);            /* Change timestamp of a file/dir */
FRESULT f_chdir (const TCHAR* path);                                /* Change current directory */
FRESULT f_chdrive (const TCHAR* path);                                /* Change current drive */
FRESULT f_getcwd (TCHAR* buff, unsigned int len);                            /* Get current directory */
FRESULT f_getfree (const TCHAR* path, uint32_t* nclst, FATFS** fatfs);    /* Get number of free clusters on the drive */
FRESULT f_getlabel (const TCHAR* path, TCHAR* label, uint32_t* vsn);    /* Get volume label */
FRESULT f_setlabel (const TCHAR* label);                            /* Set volume label */
FRESULT f_forward (FIL* fp, unsigned int(*func)(const uint8_t*,unsigned int), unsigned int btf, unsigned int* bf);    /* Forward data to the stream */
FRESULT f_expand (FIL* fp, FSIZE_t szf, uint8_t opt);                    /* Allocate a contiguous block to the file */
FRESULT f_mount (FATFS* fs, const TCHAR* path, uint8_t opt);            /* Mount/Unmount a logical drive */
FRESULT f_mkfs (const TCHAR* path, uint8_t opt, uint32_t au, void* work, unsigned int len);    /* Create a FAT volume */
FRESULT f_fdisk (uint8_t pdrv, const uint32_t* szt, void* work);            /* Divide a physical drive into some partitions */
FRESULT f_setcp (uint16_t cp);                                            /* Set current code page */
int f_putc (TCHAR c, FIL* fp);                                        /* Put a character to the file */
int f_puts (const TCHAR* str, FIL* cp);                                /* Put a string to the file */
int f_printf (FIL* fp, const TCHAR* str, ...);                        /* Put a formatted string to the file */
TCHAR* f_gets (TCHAR* buff, int len, FIL* fp);                        /* Get a string from the file */

#define f_eof(fp) ((int)((fp)->fptr == (fp)->obj.objsize))
#define f_error(fp) ((fp)->err)
#define f_tell(fp) ((fp)->fptr)
#define f_size(fp) ((fp)->obj.objsize)
#define f_rewind(fp) f_lseek((fp), 0)
#define f_rewinddir(dp) f_readdir((dp), 0)
#define f_rmdir(path) f_unlink(path)
#define f_unmount(path) f_mount(0, path, 0)

#ifndef EOF
#define EOF (-1)
#endif




/*--------------------------------------------------------------*/
/* Additional user defined functions                            */

/* RTC function */
#if !FF_FS_NORTC
uint32_t get_fattime (void);
#endif

/* LFN support functions */
#if FF_USE_LFN                        /* Code conversion (defined in unicode.c) */
WCHAR ff_oem2uni (WCHAR oem, uint16_t cp);    /* OEM code to Unicode conversion */
WCHAR ff_uni2oem (WCHAR uni, uint16_t cp);    /* Unicode to OEM code conversion */
WCHAR ff_wtoupper (WCHAR uni);            /* Unicode upper-case conversion */
#endif
#if FF_USE_LFN == 3                        /* Dynamic memory allocation */
void* ff_memalloc (unsigned int msize);            /* Allocate memory block */
void ff_memfree (void* mblock);            /* Free memory block */
#endif

/* Sync functions */
#if FF_FS_REENTRANT
int ff_cre_syncobj (uint8_t vol, FF_SYNC_t* sobj);    /* Create a sync object */
int ff_req_grant (FF_SYNC_t sobj);        /* Lock sync object */
void ff_rel_grant (FF_SYNC_t sobj);        /* Unlock sync object */
int ff_del_syncobj (FF_SYNC_t sobj);    /* Delete a sync object */
#endif




/*--------------------------------------------------------------*/
/* Flags and offset address                                     */


/* File access mode and open method flags (3rd argument of f_open) */
#define    FA_READ                0x01
#define    FA_WRITE            0x02
#define    FA_OPEN_EXISTING    0x00
#define    FA_CREATE_NEW        0x04
#define    FA_CREATE_ALWAYS    0x08
#define    FA_OPEN_ALWAYS        0x10
#define    FA_OPEN_APPEND        0x30

/* Fast seek controls (2nd argument of f_lseek) */
#define CREATE_LINKMAP    ((FSIZE_t)0 - 1)

/* Format options (2nd argument of f_mkfs) */
#define FM_FAT        0x01
#define FM_FAT32    0x02
#define FM_EXFAT    0x04
#define FM_ANY        0x07
#define FM_SFD        0x08

/* File attribute bits for directory entry (FILINFO.fattrib) */
#define    AM_RDO    0x01    /* Read only */
#define    AM_HID    0x02    /* Hidden */
#define    AM_SYS    0x04    /* System */
#define AM_DIR    0x10    /* Directory */
#define AM_ARC    0x20    /* Archive */


#ifdef __cplusplus
}
#endif

#endif /* FF_DEFINED */
