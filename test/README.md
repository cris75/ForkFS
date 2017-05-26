# Test Software
Software to test the ForkFS.

* 2017-05-26 - Version 014 - Added f_forward() test at the File System.
  * Added f_forward() test.

* 2017-05-23 - Version 013 - Changed for FatFS R0.13 from ELM-ChaN.
  * Fixed date check problems at f_stat().
  * Changed Makefile.

* 2017-05-19 - Version 012 - Changed File System tests and added more tests.
  * Changed File System struct to use buffer.
  * Added f_chdrive() test.

* 2017-05-16 - Version 011 - Added more tests to check the File System.
  * Changed _USE_EXPAND to use f_expand().
  * Added f_expand() test.

* 2017-05-12 - Version 010 - Fixed warnings and added more test to check the File System.
  * Fixed Warning about cast using strcmp().
  * Fixed Warning about data type using printf().
  * Added f_lseek() test.

* 2017-05-09 - Version 009 - Changed to support Linux GCC.
  * Changed diskio.c to use fseek() instead of fsetpos().

* 2017-05-05 - Version 008 - Added more tests to check the File System.
  * Added FILINFO test, from f_stat().
  * Added f_utime() test.

* 2017-05-02 - Version 007 - Added more tests to check the File System.
  * Added f_chmod() test.

* 2017-04-28 - Version 006 - Added more tests to check the File System.
  * Added f_getcwd() test.
  * Added f_setlabel() test.
  * Added f_getlabel() test.
  * Added f_getfree() test.

* 2017-04-21 - Version 005 - Added more tests to check the File System.
  * Added f_stat() test.
  * Added f_sync() test.

* 2017-04-18 - Version 004 - Added more tests to check the File System.
  * Added f_chdir() test.
  * Added f_opendir() test.
  * Added f_readdir() test.
  * Added f_closedir() test.
  * Changed the software name to "Sanity Test Software".

* 2017-04-11 - Version 003 - Added more tests to check the File System.
  * Added f_unlink() test.
  * Added f_disk() test.

* 2017-04-07 - Version 002 - Added more tests to check the File System.
  * Added f_truncate() test.

* 2017-04-04 - Version 001 - Added more tests to check the File System.
  * Added fread() and f_rename() test.
  * Printed parameters.
  * Check return of written or read bytes.
  * Check file size.

* 2017-03-31 - Version 000 - First version of the test project.
  * Criate a 16MBytes disk.
  * The test formats the disk, opens a file in the root directory, writes 1KByte, and closes the file.
