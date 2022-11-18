/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2014        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: Header file of existing USB MSD control module */
//#include "atadrive.h"	/* Example: Header file of existing ATA harddisk control module */
//#include "sdcard.h"		/* Example: Header file of existing MMC/SDC contorl module */
#include "stm32_eval_sdio_sd.h"
#include "BspSdio.h"

/* Definitions of physical drive number for each drive */
#define ATA		0	/* Example: Map ATA harddisk to physical drive 0 */
#define MMC		1	/* Example: Map MMC/SD card to physical drive 1 */
#define USB		2	/* Example: Map USB MSD to physical drive 2 */


#define SD_BLOCKSIZE 512
extern SD_CardInfo SDCardInfo;;
extern void MemDump(void* ptr, unsigned int length, unsigned int width);



/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT;


	switch (pdrv) {
	case ATA :
		stat &= ~STA_NOINIT;

		// translate the reslut code here

		return stat;

	case MMC :
//		result = MMC_disk_status();

		// translate the reslut code here

		return stat;

	case USB :
//		result = USB_disk_status();

		// translate the reslut code here

		return stat;
	}
	return stat;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	DSTATUS stat = STA_NOINIT ;
//	int result;
	printf("disk_initialize() Start ------------------------->\n");
	switch (pdrv) {
		case ATA :
	//		result = ATA_disk_initialize();

			// translate the reslut code here
			
			if(SD_Init() == SD_OK)
			{
				printf("disk_initialize(): Success\n");
				stat &= ~STA_NOINIT;
			}
			else 
			{
				printf("disk_initialize(): Failed\n");
				stat = STA_NOINIT;
			}
		
			break;

		case MMC :
	//		result = MMC_disk_initialize();

			// translate the reslut code here

			return stat;

		case USB :
	//		result = USB_disk_initialize();

			// translate the reslut code here

			return stat;
			
		default:
			stat = STA_NOINIT;
	}
	
	printf("disk_initialize() end, stat = 0x%x-------------------->\n", stat);
	
	return stat;
}


/*-----------------------------------------------------------------------*/
/* 读扇区：读取扇区内容到指定存储区                                              */
/*-----------------------------------------------------------------------*/
DRESULT disk_read (
	BYTE pdrv,		/* 设备物理编号(0..) */
	BYTE *buff,		/* 数据缓存�?*/
	DWORD sector,	/* 扇区首地址 */
	UINT count		/* 扇区个数(1..128) */
)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;
//	printf("disk_read() start: SD_state = 0x%x---------------------->>\n", SD_state);
	switch (pdrv) {
		case ATA:	/* SD CARD */						
			if((DWORD)buff&3)
			{
				DRESULT res = RES_OK;
				DWORD scratch[SD_BLOCKSIZE / 4];

				while (count--) 
				{
					res = disk_read(ATA,(void *)scratch, sector++, 1);

					if (res != RES_OK) 
					{
						break;
					}
					memcpy(buff, scratch, SD_BLOCKSIZE);
					buff += SD_BLOCKSIZE;
				}
				return res;
			}

			SD_state = SD_ReadMultiBlocks(buff, (uint64_t)sector * SD_BLOCKSIZE, SD_BLOCKSIZE, count);
//			printf("disk_read()1: SD_state = 0x%x\n", SD_state);
			if(SD_state == SD_OK)
			{
				/* Check if the Transfer is finished */
				SD_state = SD_WaitReadOperation();
//				printf("disk_read()2: SD_state = 0x%x\n", SD_state);
//				MemDump(buff, (SD_BLOCKSIZE*count), 8);
				
				while(SD_GetStatus() != SD_TRANSFER_OK);
			}
//			printf("disk_read()3: SD_state = 0x%x\n", SD_state);
			if(SD_state != SD_OK)
				status = RES_PARERR;
			else
				status = RES_OK;
		
			break;   

	//	case SPI_FLASH:
	//		break;

		default:
			status = RES_PARERR;
	}
//	printf("disk_read() end: SD_state = 0x%x----------------------->\n", SD_state);
	return status;
}



/*-----------------------------------------------------------------------*/
/* 写扇区：见数据写入指定扇区空间上                                      */
/*-----------------------------------------------------------------------*/
#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			  /* 设备物理编号(0..) */
	const BYTE *buff,	/* 欲写入数据的缓存�?*/
	DWORD sector,		  /* 扇区首地址 */
	UINT count			  /* 扇区个数(1..128) */
)
{
	DRESULT status = RES_PARERR;
	SD_Error SD_state = SD_OK;
//	printf("disk_write() start: SD_state = 0x%x----------------------->\n", SD_state);
	if (!count) {
		return RES_PARERR;		/* Check parameter */
	}

	switch (pdrv) {
		case ATA:	/* SD CARD */  
			if((DWORD)buff&3)
			{
				DRESULT res = RES_OK;
				DWORD scratch[SD_BLOCKSIZE / 4];

				while (count--) 
				{
					memcpy( scratch,buff,SD_BLOCKSIZE);
					res = disk_write(ATA,(void *)scratch, sector++, 1);
					if (res != RES_OK) 
					{
						break;
					}					
					buff += SD_BLOCKSIZE;
		    }
		    return res;
			}		
		
			SD_state=SD_WriteMultiBlocks((uint8_t *)buff,(uint64_t)sector*SD_BLOCKSIZE,SD_BLOCKSIZE,count);
			if(SD_state==SD_OK)
			{
				/* Check if the Transfer is finished */
				SD_state=SD_WaitWriteOperation();

				/* Wait until end of DMA transfer */
				while(SD_GetStatus() != SD_TRANSFER_OK);			
			}
			if(SD_state!=SD_OK)
				status = RES_PARERR;
		  else
			  status = RES_OK;	
		break;

//		case SPI_FLASH:
//		break;
    
		default:
			status = RES_PARERR;
	}
//	printf("disk_write() end: SD_state = 0x%x----------------------->\n", SD_state);
	return status;
}
#endif


/*-----------------------------------------------------------------------*/
/* 其他控制                                                              */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* 物理编号 */
	BYTE cmd,		  /* 控制指令 */
	void *buff		/* 写入或者读取数据地址指针 */
)
{
	DRESULT status = RES_PARERR;
	switch (pdrv) {
		case ATA:	/* SD CARD */
			switch (cmd) 
			{
				// Get R/W sector size (WORD) 
				case GET_SECTOR_SIZE :    
					*(WORD * )buff = SD_BLOCKSIZE;
				break;
				// Get erase block size in unit of sector (DWORD)
				case GET_BLOCK_SIZE :      
					*(DWORD * )buff = 1;
				break;

				case GET_SECTOR_COUNT:
					*(DWORD * )buff = SDCardInfo.CardCapacity/SDCardInfo.CardBlockSize;
					break;
				case CTRL_SYNC :
				break;
			}
			status = RES_OK;
			break;
    
//		case SPI_FLASH:		      
//		break;
    
		default:
			status = RES_PARERR;
	}
	return status;
}
#endif

							 
__weak DWORD get_fattime(void) {
	/* 返回当前时间�?*/
	return	  ((DWORD)(2015 - 1980) << 25)	/* Year 2015 */
			| ((DWORD)1 << 21)				/* Month 1 */
			| ((DWORD)1 << 16)				/* Mday 1 */
			| ((DWORD)0 << 11)				/* Hour 0 */
			| ((DWORD)0 << 5)				  /* Min 0 */
			| ((DWORD)0 >> 1);				/* Sec 0 */
}
