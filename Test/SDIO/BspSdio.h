#ifndef TEST_SDIO_H_
#define TEST_SDIO_H_

#include "ff.h"

void ReadDumpTest(void);

void TestSdio(void);

int SD_disk_status(void);

int SD_disk_initialize(void);

int SD_disk_read(BYTE *buff, DWORD sector, UINT count);

int SD_disk_write(const BYTE *buff, DWORD sector, UINT count);

#endif
