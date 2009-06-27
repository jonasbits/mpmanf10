/****************************************************************************

	mpm.c	MPMan utility - Ashpool Systems (c) 1998
                                (C)Copyright 1999 by Hiroshi Takekawa

Version History

	v0.01 07/10/98	jab@cix.co.uk	

		Initial release

	v0.02 26/10/98	jab@cix.co.uk	

		Bug fix for last block of file uploaded to mpman.

	v0.03 29/04/99	sian@fennel.org

	    Deleted turboc support.
		Replaced interactive operation with command line interface.

	v0.04 15/08/99	sian@fennel.org

	    Finally, I succeeded to use whole 64M memory...

	v0.05 06/08/01	tork@get2net.dk

		Move file up/down one position in mpman implemented.

	v0.06 21/10/01	tork@get2net.dk

		Changed delay to 2 mS in GetBlock and SendBlock and MAX_RETRY to 10
		to get the program to work with Windows XP.

	v0.07 07/05/02	tork@get2net.dk

		Changed _sleep to Sleep, delay from 2 mS to 1 mS in GetBlock,
		put delay 1 mS before return in GetBlockRetry and SendBlockRetry.

****************************************************************************/

#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<time.h>
#include	<sys/stat.h>
#include	<windows.h>

#define		OUTPORT(p,v)	outportb(p,v)
#define		INPORT(p)		inportb(p)

/****************************************************************************/
#define	ID_VERSION			7 /* version 0.07 */

#define	PORT_BASE_DEFAULT	0x378
#define	OFFSET_PORT_DATA	0
#define	OFFSET_PORT_STATUS	1
#define	OFFSET_PORT_CONTROL	2

#define	MAX_DIRENTRY		63
#define	SIZE_BLOCK			32768

#define	MAX_BLOCK			2048
#define	TOTAL_BLOCK_DEFAULT	2048 /* 64M = 2048, 32M = 1024 */

#define ID_BLOCK_INUSE		0x00
#define	ID_BLOCK_FREE		0xff

#define	MAX_RETRY			10

//#define	TRUE				(1 == 1)
//#define	FALSE				(1 == 0)

#define	B_00000000			0x00
#define	B_00000001			0x01
#define	B_00000100			0x04
#define	B_00000101			0x05
#define	B_00001000			0x08
#define	B_00100000			0x20
#define	B_01000000			0x40

typedef int BOOL;
typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned int UINT;
typedef unsigned long ULONG;

/****************************************************************************/
/* enable byte align structs */
#pragma pack(1)

/* directory header: 128bytes */
typedef struct
{
	UCHAR m_ucCountEntry;
	char m_acNotUsed[3];
	USHORT m_usTotalBlock;
	USHORT m_usInUseBlock;
	USHORT m_usFreeBlock;
	char m_acNotUsed2[2];
	UCHAR m_ucCountEntryMP3;
	ULONG m_ulChecksum;
	char m_acNotUsed3[128 - 17];
}	DIRHEADER;

/* directory entry: 128bytes/entry */
typedef struct
{
	USHORT m_usOffsetBlock;
	USHORT m_usInUseBlock;
	USHORT m_usModBlock;
	long m_lSize;
	char m_acNotUsed[6];
	long m_lDateTime;
	char m_szName[128 - 20];
}	DIRENTRY;

/* end block info */
typedef struct
{
	UCHAR m_ucFlagNext;
	USHORT m_usKNext;
	UCHAR m_uc32MBlockNext; /* char cNotUsed2; */
	UCHAR m_uc16MBlockNext;
	UCHAR m_uc16MBlockPlus1Next;
	UCHAR m_ucFlagPrev;
	USHORT m_usKPrev;
	UCHAR m_uc32MBlockPrev; /* char cNotUsed4; */
	UCHAR m_uc16MBlockPrev;
	UCHAR m_uc16MBlockPlus1Prev;
}	ENDBLOCKINFO;

/* directory block */
typedef struct
{
	DIRHEADER m_sDirHeader;              /* 0x0000 - 0x007F (128)      */
	DIRENTRY m_asDirEntry[MAX_DIRENTRY]; /* 0x0080 - 0x1FFF (128 * 63) */
	UCHAR m_aucBlockInUse[MAX_BLOCK];    /* 0x2000 - 0x3FFF (2048)     */
	USHORT m_ausFAT[MAX_BLOCK];          /* 0x4000 - 0x7FFF (2 * 2048) */
}	DIRBLOCK;


/* disable byte align */
#pragma pack()

/****************************************************************************/
static UINT uiPort;
static UINT uiBlockAvailable;
static UINT uiRx;

//---------------------------------------------------------------------------

#define DLLimport __declspec ( dllimport ) unsigned long Port();

#define DLLexport __declspec ( dllexport ) void func();

//---------------------------------------------------------------------------

DLLimport __stdcall DlPortReadPortUchar(unsigned long Port);

unsigned char __fastcall inportb(unsigned long iport)
{
	return DlPortReadPortUchar(iport);
}

//---------------------------------------------------------------------------

DLLexport __stdcall DlPortWritePortUchar(unsigned long Port, unsigned char Value);

void __fastcall outportb(int iport, unsigned char ucbyte)
{
	DlPortWritePortUchar(iport, ucbyte);
}

/****************************************************************************/

/* return pointer to formatted date time str */
char * TimeStr(long lValue)
{
	static char szBuf[64];
	struct tm *sDateTime;
	sDateTime = localtime(&lValue);

    sprintf(szBuf, "%02u/%02u/%02u",
		(UINT)sDateTime->tm_year % 100,
		(UINT)sDateTime->tm_mon+1 % 100,
		(UINT)sDateTime->tm_mday % 100);

	
	/* sprintf(szBuf, "%02u/%02u/%02u %02u:%02u:%02u", 
		(UINT)sDateTime->tm_mday % 100,
		(UINT)sDateTime->tm_mon+1 % 100,
		(UINT)sDateTime->tm_year % 100);
		(UINT)sDateTime->tm_hour % 100,
		(UINT)sDateTime->tm_min % 100,
		(UINT)sDateTime->tm_sec % 100); */

	return szBuf;
}

/* reverse bits in byte */
UINT BitRev(UINT uiValue)
{
	UINT uiRet = 0;
	int iA;

	for (iA = 0; iA < 8; iA++)
	{
		uiRet <<= 1;
		uiRet |= (uiValue & 1);
		uiValue >>= 1;
	}

	return uiRet;
}

/* return file info */
BOOL GetFileInfo(char *pszFile, long *plSize, long *plDateTime)
{
	struct stat sStat;
	FILE *fpFile;

	fpFile = fopen(pszFile, "rb");
	if (!fpFile)
		return FALSE;

	if (stat(pszFile, &sStat))
		return FALSE;

	fclose(fpFile);

	*plSize = sStat.st_size;
	*plDateTime = sStat.st_mtime;

	return TRUE;
}

/* check for MP3 file extention */
BOOL CheckMP3FileExtention(char *pszFile)
{
	int iLength = strlen(pszFile);

	if (iLength < 4)
		return FALSE;

	if (!strncmp(&pszFile[iLength - 4], ".mp3", 4))
		return TRUE;

	if (!strncmp(&pszFile[iLength - 4], ".MP3", 4))
		return TRUE;

	return FALSE;
}

/* return file name */
char * GetFileName(char *pszPath)
{
	int iLength;
	char *pc;

	iLength = strlen(pszPath);
	if (!iLength)
		return pszPath;

	pc = pszPath + iLength - 1;
	while(*pc != '\\' && *pc != '/' && *pc != ':')
	{
		if (pc == pszPath)
			return pc;
		pc--;
	}
	pc++;

	return pc;
}

/* initialize directory block */
void InitDirBlock(DIRBLOCK *psDirBlock)
{
	DIRHEADER *psDirHeader;

	/* dir header */
	psDirHeader = &psDirBlock->m_sDirHeader;
	psDirHeader->m_ucCountEntry = 0;
	memset(psDirHeader->m_acNotUsed, 0, sizeof(psDirHeader->m_acNotUsed));
	psDirHeader->m_usTotalBlock = uiBlockAvailable;
	psDirHeader->m_usInUseBlock = 1;
	psDirHeader->m_usFreeBlock = uiBlockAvailable - psDirHeader->m_usInUseBlock;
	memset(psDirHeader->m_acNotUsed2, 0, sizeof(psDirHeader->m_acNotUsed2));
	psDirHeader->m_ucCountEntryMP3 = 0;
	memset(psDirHeader->m_acNotUsed3, 0, sizeof(psDirHeader->m_acNotUsed3));

	/* dir entries */
	memset(psDirBlock->m_asDirEntry, 0, sizeof(DIRENTRY) * MAX_DIRENTRY);

	/* blocks in use (first block is directory block) */
	memset(psDirBlock->m_aucBlockInUse, ID_BLOCK_FREE, MAX_BLOCK);
	psDirBlock->m_aucBlockInUse[0] = ID_BLOCK_INUSE;

	/* FAT */
	memset(psDirBlock->m_ausFAT, 0, sizeof(short) * MAX_BLOCK);
	psDirBlock->m_ausFAT[0] = 0;
}

/* find first free block */
UINT FindFreeBlock(DIRBLOCK *psDirBlock)
{
	USHORT uiA;
	UCHAR *puc = psDirBlock->m_aucBlockInUse;

	for (uiA = 0; uiA < MAX_BLOCK; uiA++, puc++)
	{
		if (*puc == ID_BLOCK_FREE)
			return uiA;
	}

	return 0xffff;
}

/****************************************************************************/
/* wait ack */
BOOL WaitAck(BOOL bFlag)
{
	long lTime = 0;

	while (1)
	{
		if (lTime++ > 200000L) 
			return FALSE; 

		uiRx = INPORT(uiPort + OFFSET_PORT_STATUS);
		
		if (bFlag)
		{
			if (uiRx & B_00001000)
				break;
		}
		else
		{
			if (!(uiRx & B_00001000))
				break;
		}
	}
  
	return TRUE;
}

/****************************************************************************/
BOOL Sync( void )
{
	/* clock out lo, wait for ack lo */
	OUTPORT(uiPort+OFFSET_PORT_CONTROL, B_00000001);
	if (!WaitAck(FALSE))
		return FALSE;
	/* clock out hi, wait for ack hi */
	OUTPORT(uiPort+OFFSET_PORT_CONTROL, B_00000000);
	if (!WaitAck(TRUE))
		return FALSE;
	/* clock out lo, wait for ack lo */
	OUTPORT(uiPort+OFFSET_PORT_CONTROL, B_00000001);
	if (!WaitAck(FALSE))
		return FALSE;

	return TRUE;
}

/****************************************************************************/
/* get block */
BOOL GetBlockRetry(UCHAR *pucBlock, UINT uiOffsetBlock)
{
	long lA;
	UINT uiTemp, uiTemp2 = 0;
	BOOL bClkHi;
	BOOL bHi;

	/* sync */
	if (!Sync())
		return FALSE;

	/* 'get block' command, clock and wait for ack */
	OUTPORT(uiPort + OFFSET_PORT_DATA, B_00100000);
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000000);
	if (!WaitAck(TRUE))
		return FALSE;

	/* source block lo, clock and wait for ack */
	OUTPORT(uiPort + OFFSET_PORT_DATA, (uiOffsetBlock & 0x00ff));
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000001);
	if (!WaitAck(FALSE))
		return FALSE;

	/* source block hi, clock and wait for ack */
	OUTPORT(uiPort + OFFSET_PORT_DATA, (uiOffsetBlock & 0xff00) >> 8);
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000000);
	if (!WaitAck(TRUE))
		return FALSE;

	/* ready for transfer */
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000100);

	/* rx 2 nibbles (unknown) */
	if (!WaitAck(FALSE))
		return FALSE;
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000101);
	if (!WaitAck(TRUE))
		return FALSE;
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000100);
  
	/* get data */
	bClkHi = FALSE;
	bHi = FALSE;
	for (lA = 0; lA < 65536; lA++)
	{
		/* wait for data */
		if (bClkHi)
		{
			if (!WaitAck(TRUE))
				return FALSE;
		}
		else
		{
			if (!WaitAck(FALSE))
				return FALSE;
		}

		/* get data */
		uiTemp = BitRev(uiRx ^ 0x80);
		uiTemp = uiTemp & 0x0f;

		if (!bHi)
			uiTemp2 = uiTemp;
		else
		{
			uiTemp2 |= uiTemp << 4;
			*pucBlock++ = (UCHAR)uiTemp2;
		}
		bHi = !bHi;

		/* ack */
		if (bClkHi)
			OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000100);
		else
			OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000101);
		bClkHi = !bClkHi;
	}

	/* rx 2 nibbles (unknown) */
	if (!WaitAck(FALSE))
		return FALSE;
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000101);
	if (!WaitAck(TRUE))
		return FALSE;
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000100);

	Sleep(1);
	return TRUE;
}

/****************************************************************************/
/* get block */
BOOL GetBlock(UCHAR *pucBlock, UINT uiOffsetBlock)
{
	int iRetry = 0;

	while (iRetry++ < MAX_RETRY)
	{
		if (GetBlockRetry(pucBlock, uiOffsetBlock))
			return TRUE; 
		Sleep(1);
	}

	fprintf(stderr, "too many retries for block\n");

	return FALSE;
}

/****************************************************************************/
/* send block */
BOOL SendBlockRetry(UCHAR *pucBlock, UINT uiOffsetBlock, UINT uiBlockPrev, UINT uiBlockNext)
{
	ENDBLOCKINFO sEndBlockInfo;
	long lA;
	BOOL bClkHi;
	UCHAR *puc;
	UCHAR ucTemp;

	/* sync */
	if (!Sync())
		return FALSE;

	/* 'send block' command, clock and wait for ack */
	OUTPORT(uiPort + OFFSET_PORT_DATA, B_01000000);
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000000);
	if (!WaitAck(TRUE))
		return FALSE;

	/* dest block lo, clock and wait for ack */
	OUTPORT(uiPort + OFFSET_PORT_DATA, (uiOffsetBlock & 0x00ff));
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000001);
	if (!WaitAck(FALSE))
		return FALSE;

	/* dest block hi, clock and wait for ack */
	OUTPORT(uiPort + OFFSET_PORT_DATA, (uiOffsetBlock & 0xff00) >> 8);
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000000);
	if (!WaitAck(TRUE))
		return FALSE;

	/* ready for transfer */
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000100);

	/* rx 2 nibbles (unknown) */
	if (!WaitAck(FALSE))
		return FALSE;
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000101);
	if (!WaitAck(TRUE))
		return FALSE;
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000100);

	/* send data */
	bClkHi = FALSE;
	for(lA = 0; lA < 32768; lA++)
	{
		/* output data */
		ucTemp = *pucBlock++;
		OUTPORT(uiPort + OFFSET_PORT_DATA, ucTemp);

		/* flag as data ready */
		if (bClkHi)
			OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000100);
		else
			OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000101);

		/* wait for ack */
		if ( bClkHi )
		{
			if (!WaitAck(TRUE))
			return FALSE;
		}
		else
		{
			if (!WaitAck(FALSE))
				return FALSE;
		}

		bClkHi = !bClkHi;
	}

	/* build end block info */
	memset(&sEndBlockInfo, 0, sizeof(ENDBLOCKINFO));
	if (uiBlockNext == 0xffff) {
		sEndBlockInfo.m_ucFlagNext = 0xff;
		sEndBlockInfo.m_usKNext = 0xffff;
		sEndBlockInfo.m_uc16MBlockPlus1Next = 1;
	}
	else
	{
		sEndBlockInfo.m_usKNext = (uiBlockNext << 5) & 0x3fff;
		/*
		* block number      <512 <1024 <1536 <2048
		* m_uc32MBlockNext     0     0     1     1
		* m_uc16MBlockNext     0     2     0     2
		*/
		if (uiBlockNext < 512)
		{
			sEndBlockInfo.m_uc32MBlockNext = 0;
			sEndBlockInfo.m_uc16MBlockNext = 0;
		 }
		else if (uiBlockNext < 1024)
		{
			sEndBlockInfo.m_uc32MBlockNext = 0;
			sEndBlockInfo.m_uc16MBlockNext = 2;
		}
		else if (uiBlockNext < 1536)
		{
			sEndBlockInfo.m_uc32MBlockNext = 1;
			sEndBlockInfo.m_uc16MBlockNext = 0;
		}
		else if (uiBlockNext < 2048)
		{
			sEndBlockInfo.m_uc32MBlockNext = 1;
			sEndBlockInfo.m_uc16MBlockNext = 2;
		}
		else
		{
			fprintf(stderr, "Block number too big %d >= 2048\n", uiBlockNext);
			exit(1);
		}
		sEndBlockInfo.m_uc16MBlockPlus1Next = sEndBlockInfo.m_uc16MBlockNext + 1;
	}

	if (uiBlockPrev == 0xffff)
	{
		sEndBlockInfo.m_ucFlagPrev = 0xff;
		sEndBlockInfo.m_usKPrev = 0xffff;
		sEndBlockInfo.m_uc16MBlockPlus1Prev = 1;
	}
	else
	{
		sEndBlockInfo.m_usKPrev = (uiBlockPrev << 5) & 0x3fff;
		if (uiBlockPrev < 512)
		{
			sEndBlockInfo.m_uc32MBlockPrev = 0;
			sEndBlockInfo.m_uc16MBlockPrev = 0;
		}
		else if (uiBlockPrev < 1024)
		{
			sEndBlockInfo.m_uc32MBlockPrev = 0;
			sEndBlockInfo.m_uc16MBlockPrev = 2;
		}
		else if (uiBlockPrev < 1536)
		{
			sEndBlockInfo.m_uc32MBlockPrev = 1;
			sEndBlockInfo.m_uc16MBlockPrev = 0;
		}
		else if (uiBlockPrev < 2048)
		{
			sEndBlockInfo.m_uc32MBlockPrev = 1;
			sEndBlockInfo.m_uc16MBlockPrev = 2;
		}
		else
		{
			fprintf(stderr, "Block number too big %d >= 2048\n", uiBlockPrev);
			exit(1);
		}
		sEndBlockInfo.m_uc16MBlockPlus1Prev = sEndBlockInfo.m_uc16MBlockPrev + 1;
	}

	/* send end block info */
	puc = (UCHAR *)&sEndBlockInfo;
	bClkHi = FALSE;
	for (lA = 0; lA < sizeof(ENDBLOCKINFO); lA++)
	{
		/* output data */
		OUTPORT(uiPort + OFFSET_PORT_DATA, *puc++);

		/* flag as data ready */
		if (bClkHi)
			OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000100);
		else
			OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000101);

		/* wait for ack */
		if (bClkHi)
		{
			if (!WaitAck(TRUE))
				return FALSE;
		}
		else
		{
			if (!WaitAck(FALSE))
				return FALSE;
		}

		bClkHi = !bClkHi;
	}

	/* rx 2 nibbles (unknown) */
	if (!WaitAck(FALSE))
		return FALSE;
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000101);
	if (!WaitAck(TRUE))
		return FALSE;
	OUTPORT(uiPort + OFFSET_PORT_CONTROL, B_00000100);
	
	Sleep(1);
	return TRUE;
}

/****************************************************************************/
/* send block */
BOOL SendBlock(UCHAR *pucBlock, UINT uiOffsetBlock, UINT uiBlockPrev, UINT uiBlockNext)
{
	int iRetry = 0;

	while (iRetry++ < MAX_RETRY)
	{
		if (SendBlockRetry(pucBlock, uiOffsetBlock, uiBlockPrev, uiBlockNext))
			return TRUE;
		Sleep(2);
	}

	fprintf(stderr, "too many retries for block\n");

	return FALSE;
}

/****************************************************************************/
/* get directory */
BOOL GetDirectory(DIRBLOCK *psDirBlock, UCHAR *paucBlock)
{
	/* get directory block */
	if (!GetBlock(paucBlock, 0))
		return FALSE;

	/* copy dir block */
	memcpy(psDirBlock, paucBlock, sizeof(DIRBLOCK));

	/* if uninitialized, initialize with valid data */
	if (psDirBlock->m_sDirHeader.m_ucCountEntry == 0xff)
		InitDirBlock(psDirBlock);

	return TRUE;
}

/****************************************************************************/
/* send directory */
BOOL SendDirectory(DIRBLOCK *psDirBlock, UCHAR *paucBlock)
{
	ULONG ulChecksum;
	long lA;
	UCHAR *puc;
	int iA;

	/* clear */
	memset(paucBlock, 0, SIZE_BLOCK);
	/* copy dir header */
	memcpy(paucBlock, psDirBlock, sizeof(DIRBLOCK));

	/* create and store checksum */
	ulChecksum = 0;
	puc = paucBlock;
	for (lA = 0; lA < SIZE_BLOCK; lA++)
		ulChecksum += *puc++;
	puc = (UCHAR*)&psDirBlock->m_sDirHeader.m_ulChecksum;
	for (iA = 0; iA < sizeof(long); iA++)
		ulChecksum -= *puc++;
	((DIRHEADER *)paucBlock)->m_ulChecksum = ulChecksum;
	/* ((DIRHEADER *)paucBlock)->m_ulChecksum = 0x54594; */

	/* Send directory block */
	if (!SendBlock(paucBlock, 0, 0, 0))
		return FALSE;

	return TRUE;
}

/****************************************************************************/
/* hex dump of directorry block */
void DumpDirectory(DIRBLOCK *psDirBlock, long lOffset)
{
	int iA, iB, iC;
	UCHAR *puc;

	puc = (UCHAR*)psDirBlock;
	puc += lOffset;
	for (iA = 0; iA < 2048; iA++)
	{
		printf("%08lx: ", lOffset + iA * 16);
		for (iB = 0; iB < 16; iB++)
		{
			if (iB == 8)
				printf(" ");
			printf("%02x ", *(puc + iA * 16 + iB));
		}
		printf("  ");
		for (iB = 0; iB < 16; iB++)
		{
			if (iB == 8)
				printf( " " );
			iC = *(puc + iA * 16 + iB);
			printf("%c", (iC < 32 || iC > 127) ? '.' : iC);
		}
		printf("\n");
	} 
}

/****************************************************************************/
/* display directory */
void DisplayDirectory( DIRBLOCK* psDirBlock )
{
	DIRHEADER* psDirHeader;
	DIRENTRY* pDE;
	int iA, iCount;

	/* directory header */
	psDirHeader = &psDirBlock->m_sDirHeader;
	printf( "total: %luKB\n", ((long)psDirHeader->m_usTotalBlock * SIZE_BLOCK) / 1024 );
	printf( "used: %luKB\n", ((long)psDirHeader->m_usInUseBlock * SIZE_BLOCK) / 1024 );
	printf( "unused: %luKB\n", ((long)psDirHeader->m_usFreeBlock * SIZE_BLOCK) / 1024 );

	/* directory entries */
	iCount = psDirHeader->m_ucCountEntry;

	if ( iCount )
	{
		pDE = psDirBlock->m_asDirEntry;
		for( iA=0; iA<iCount; ++iA, ++pDE )
		{
			long lTime = pDE->m_lDateTime;
			printf( "\"%02d %08d %s %s\"\n",
				iA+1,
				pDE->m_lSize,
				TimeStr(lTime),
				pDE->m_szName);
		}
	}
}

/****************************************************************************/
/* upload file */
BOOL UploadFile(DIRBLOCK *psDirBlock, char *pszFile, DIRENTRY *pDE, UCHAR *paucBlock)
{
	FILE *fpFile;
	UINT uiSize;
	USHORT usBlock, usBlockEnd;
	USHORT usBlockCurrent, usBlockPrev, usBlockNext;
	BOOL bRet;

	/* open file for read */
	fpFile = fopen(pszFile, "rb");
	if (!fpFile)
	{
		fprintf(stderr, "unable to open '%s' for read\n", pszFile);
		return FALSE;
	}

	/* default return */
	bRet = FALSE;

	/* send all blocks */
	usBlockCurrent = pDE->m_usOffsetBlock;
	usBlockPrev = 0xffff;
	usBlockEnd = pDE->m_usInUseBlock;
	for (usBlock = 0; usBlock < usBlockEnd; usBlock++)
	{
		/* printf("blocks to go %-5hd\r", usBlockEnd - usBlock); */
		printf("upload complete %-5hd\r", 1 + 100 * usBlock / usBlockEnd);
		fflush(stdout);

		/* mark as block in use */
		psDirBlock->m_aucBlockInUse[usBlockCurrent] = ID_BLOCK_INUSE;

		/* get next block and update FAT */
		if (usBlock == (usBlockEnd - 1))
		{
			usBlockNext = 0xffff;
			psDirBlock->m_ausFAT[usBlockCurrent] = 0;
		}
		else
		{
			usBlockNext = FindFreeBlock(psDirBlock);
			psDirBlock->m_ausFAT[usBlockCurrent] = usBlockNext;
		}

		/* read block */
		if (usBlock == (usBlockEnd - 1) && pDE->m_usModBlock)
		{
			uiSize = pDE->m_usModBlock;
			memset(paucBlock + uiSize, 0, SIZE_BLOCK - uiSize);
		}
		else
			uiSize = SIZE_BLOCK;
		if (fread(paucBlock, uiSize, 1, fpFile) < 1)
		{
			fprintf(stderr, "error reading from file '%s'\n", pszFile);
			break;
		}

		/* send */
		if (!SendBlock(paucBlock, usBlockCurrent, usBlockPrev, usBlockNext))
			break;

		/* update current and prev block markers */
		usBlockPrev = usBlockCurrent;
		usBlockCurrent = usBlockNext;
	}

	/* if transfer ok */
	if (usBlock == usBlockEnd)
		bRet = TRUE;

	/* close file */
	fclose(fpFile);

	return bRet;
}

/****************************************************************************/
/* download file */
BOOL DownloadFile(DIRBLOCK *psDirBlock, char *pszFile, DIRENTRY *pDE, UCHAR *paucBlock)
{
	FILE *fpFile;
	UINT uiSize;
	USHORT usBlock, usBlockEnd;
	USHORT usBlockCurrent;
	BOOL bRet;

	/* open file for write */
	if ((fpFile = fopen(pszFile, "wb")) == NULL)
	{
		fprintf(stderr, "unable to open '%s' for write\n", pszFile);
		return FALSE;
	}

	/* default return */
	bRet = FALSE;

	/* get all blocks */
	usBlockCurrent = pDE->m_usOffsetBlock;
	usBlockEnd = pDE->m_usInUseBlock;
	for (usBlock = 0; usBlock < usBlockEnd; usBlock++)
	{
		/* printf("blocks to go %-5hd\r", usBlockEnd-usBlock); */
		printf("download complete %-5hd\r", 1 + 100 * usBlock / usBlockEnd);
		fflush(stdout);

		/* get block */
		if (!GetBlock(paucBlock, usBlockCurrent))
			break;

		/* next block */
		usBlockCurrent = psDirBlock->m_ausFAT[usBlockCurrent];

		/* save block */
		if (usBlock == (usBlockEnd - 1) && pDE->m_usModBlock)
			uiSize = pDE->m_usModBlock;
		else
			uiSize = SIZE_BLOCK;
		if (fwrite(paucBlock, uiSize, 1, fpFile) < 1)
		{
			fprintf(stderr, "error writing to file '%s'\n", pszFile);
			break;
		}
	}
	/* if transfer ok */
	if (usBlock == usBlockEnd)
		bRet = TRUE;

	/* close file */
	fclose(fpFile);

	return bRet;
}


void usage(void)
{
	fprintf(stderr, " Mpman utility v%d.%02d - (c) 2001 by Torben Kristensen\n",
			ID_VERSION / 100, ID_VERSION % 100);
	fprintf(stderr, " Based on mpman utility v0.04 - (c) 1999 by Hiroshi Takekawa\n");
	fprintf(stderr, " and mpman utility v0.02 - Ashpool Systems (c) 1998\n");
	fprintf(stderr, "----------------------------------------------\n");
	fprintf(stderr, "command line switches available:\n");
	fprintf(stderr, " -p  Parallel port IO address, default=0x378\n");
	fprintf(stderr, " -m  MBytes available on MPMan, default=64\n");
	fprintf(stderr, " -l  list\n");
	fprintf(stderr, " -u  upload\n");
	fprintf(stderr, " -d  download\n");
	fprintf(stderr, " -r  remove\n");
	fprintf(stderr, " -fu move file up one position in mpman\n");
	fprintf(stderr, " -fd move file down one position in mpman\n");
	fprintf(stderr, " -i  initialize (Don't confirm. Use with care)\n");
	fprintf(stderr, " -D  dump\n");
}

typedef enum
{
  _DONOTHING,
  _LIST,
  _UPLOAD,
  _DOWNLOAD,
  _REMOVE,
  _INITIALIZE,
  _DUMP,
  _FIX,
  _MOVEUP,
  _MOVEDOWN
} Operation;

/****************************************************************************/
/* entry point */
int main(int argc, char *argv[])
{
	DIRBLOCK *psDirBlock, *psDirBlock1;
	DIRHEADER *psDirHeader;
	char *srcfile, *destfile;
	long lDump, lSizeFile, lDateTime;
	DIRENTRY *pDE;
	USHORT usTemp, usBlock, usInUseBlock;
	UCHAR *paucBlock;
	int iA, iB, iC, iCount, any;
	Operation op = _DONOTHING;

	/* process command line */
	uiPort = PORT_BASE_DEFAULT;
	uiBlockAvailable = TOTAL_BLOCK_DEFAULT;
	for( iC=1; iC<argc; ++iC )
	{
		if  ( !strcmp(argv[iC], "-p") )
		{
			if ( (iC+1) < argc )
			{
				sscanf( argv[iC+1], "%x", &uiPort );
				++iC;
			}
		}
		else if ( !strcmp(argv[iC], "-m") )
		{
			if ( (iC+1) < argc )
			{
				sscanf( argv[iC+1], "%u", &uiBlockAvailable );
				uiBlockAvailable *= 32;
				++iC;
			}
		}
		else if ( !strcmp(argv[iC], "-u") )
		{
			++iC;
			op = _UPLOAD;
			break;
		}
		else if ( !strcmp(argv[iC], "-d") )
		{
			++iC;
			op = _DOWNLOAD;
			break;
		}
		else if ( !strcmp(argv[iC], "-r") )
		{
		    ++iC;
			op = _REMOVE;
			break;
		}
		else if ( !strcmp(argv[iC], "-fu") )
		{
		    ++iC;
			op = _MOVEUP;
			break;
		}
		else if ( !strcmp(argv[iC], "-fd") )
		{
		    ++iC;
			op = _MOVEDOWN;
			break;
		}
		else if ( !strcmp(argv[iC], "-l") )
		{
			op = _LIST;
		}
		else if ( !strcmp(argv[iC], "-i") )
		{
        	op = _INITIALIZE;
		}
		else if ( !strcmp(argv[iC], "-f") )
		{
        	op = _FIX;
		}
		else if ( !strcmp(argv[iC], "-D") )
		{
        	if ( (iC+1) < argc )
			{
				sscanf( argv[iC+1], "%x", &lDump );
				++iC;
			}
			op = _DUMP;
		}
		else
		{
			usage();	
			return TRUE;
		}
	}


	/* alloc dir block and temp block */
	psDirBlock  = (DIRBLOCK *)malloc(sizeof(DIRBLOCK));
	psDirBlock1 = (DIRBLOCK *)malloc(sizeof(DIRBLOCK));
	paucBlock = (UCHAR *)malloc(SIZE_BLOCK);
	if (!psDirBlock || !paucBlock)
	{
		fprintf(stderr, "malloc failed\n");
		return FALSE;
	}

	/* pointer to dir header */
	psDirHeader = &psDirBlock->m_sDirHeader;

	/* operations: _LIST, _UPLOAD, _DOWNLOAD, _REMOVE, _INITIALIZE, _MOVEUP, _MOVEDOWN */
	switch (op)
	{
	case _LIST:
		if (!GetDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "get directory failed\n");
			return FALSE;
		}
		DisplayDirectory(psDirBlock);
		break;

	case _UPLOAD:
		if (!GetDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "get directory failed\n");
			return FALSE;
		}
		for (; iC < argc; iC++)
		{
			srcfile = argv[iC];
			destfile = GetFileName(srcfile);

			/* check current entry count */
			if (psDirHeader->m_ucCountEntry >= MAX_DIRENTRY)
			{
				fprintf(stderr, "maximum number of directory entries (%d) already in use\n", MAX_DIRENTRY);
				return FALSE;
			}

			/* get info on file to upload */
			if (!GetFileInfo(srcfile, &lSizeFile, &lDateTime))
			{
				fprintf(stderr, "unable to open '%s' for read\n", srcfile);
				return FALSE;
			}

			/* determine if enough room to store file */
			usInUseBlock = (lSizeFile / SIZE_BLOCK) + (lSizeFile % SIZE_BLOCK ? 1 : 0);
			if (usInUseBlock > psDirHeader->m_usFreeBlock)
			{
				fprintf(stderr, "not enough memory on mpman to store file\n");
				return FALSE;
			}

			/* update dir entry */
			pDE = psDirBlock->m_asDirEntry + psDirHeader->m_ucCountEntry;
			pDE->m_usOffsetBlock = FindFreeBlock(psDirBlock);
			pDE->m_usInUseBlock = usInUseBlock;
			pDE->m_usModBlock = lSizeFile % SIZE_BLOCK;
			pDE->m_lSize = lSizeFile;
			pDE->m_acNotUsed[0] = 0x00; /* 32M -> 0x80?? */
			pDE->m_acNotUsed[1] = 0x00;
			pDE->m_acNotUsed[2] = 0x00;
			pDE->m_acNotUsed[3] = 0x00;
			pDE->m_acNotUsed[4] = 0x00;
			pDE->m_acNotUsed[5] = 0x00;
			pDE->m_lDateTime = lDateTime;
			strncpy(pDE->m_szName, destfile, sizeof(pDE->m_szName));

			/* update dir header */
			psDirHeader->m_ucCountEntry++;
			psDirHeader->m_usInUseBlock += usInUseBlock;
			psDirHeader->m_usFreeBlock = uiBlockAvailable - psDirHeader->m_usInUseBlock;
			if (CheckMP3FileExtention(pDE->m_szName))
				psDirHeader->m_ucCountEntryMP3++;

			/* upload file */
			if (!UploadFile(psDirBlock, srcfile, pDE, paucBlock))
			{
				fprintf(stderr, "upload failed\n");
				return FALSE;
			}
		}

		/* upload directory */
		if (!SendDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "send directory failed\n");
			return FALSE;
		}

		printf("upload complete 100\n");
		break;

	case _DOWNLOAD:
		if (iC == argc)
		{
			fprintf(stderr, "No file to download.\n");
			return FALSE;
		}

		if (!strcmp(argv[iC], "*"))
			any = 1;
		else
			any = 0;

		if (!GetDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "get directory failed\n");
			return FALSE;
		}

		pDE = psDirBlock->m_asDirEntry;
		iCount = psDirHeader->m_ucCountEntry;
		iA = 0;

		/* get file to download */
		for (; iC < argc || any; iC++)
		{
			/* search directory for filename */
			if (!any)
			{
				srcfile = destfile = argv[iC];
				pDE = psDirBlock->m_asDirEntry;
				for (iA = 0; iA < iCount; iA++, pDE++)
				{
					if (!strcmp(pDE->m_szName, srcfile))
						break;
				}
				if (iA == iCount)
				{
					fprintf(stderr, "file '%s' not found for download\n", srcfile);
					return FALSE;
				}
			}
			else
			{
				if (iA++ == iCount)
					break;
				srcfile = destfile = pDE->m_szName;
		}
      
		/* download */
		if (!DownloadFile(psDirBlock, destfile, pDE, paucBlock))
		{
			fprintf(stderr, "download failed\n");
			return FALSE;
		}
		if (any)
		pDE++;
		}

		printf("download complete 100\n");
		break;

	case _REMOVE:
		if (!GetDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "get directory failed\n");
			return FALSE;
		}

		/* get file to remove */
		for (; iC < argc; iC++)
		{
			srcfile = argv[iC];
      
			/* search directory for filename */
			pDE = psDirBlock->m_asDirEntry;
			iCount = psDirHeader->m_ucCountEntry;
			for (iA = 0; iA < iCount; iA++, pDE++)
			{
				if (!strcmp(pDE->m_szName, srcfile))
				break;
			}
			if (iA == iCount)
			{
				fprintf(stderr, "file '%s' not found for remove\n", srcfile);
				return FALSE;
			}

			/* free blocks in use */
			usBlock = pDE->m_usOffsetBlock;
			while (usBlock)
			{
				psDirBlock->m_aucBlockInUse[usBlock] = ID_BLOCK_FREE;
				usTemp = psDirBlock->m_ausFAT[usBlock];
				psDirBlock->m_ausFAT[usBlock] = 0;
				usBlock = usTemp;
			}

			/* adjust directory header */
			psDirHeader->m_ucCountEntry--;
			psDirHeader->m_usInUseBlock -= pDE->m_usInUseBlock;
			psDirHeader->m_usFreeBlock = uiBlockAvailable - psDirHeader->m_usInUseBlock;
			if (CheckMP3FileExtention(pDE->m_szName))
				psDirHeader->m_ucCountEntryMP3--;

			/* shuffle directory */
			iCount = iCount - iA;
			for(iB = 0; iB < (iCount - 1); iB++)
			{
				memcpy(&psDirBlock->m_asDirEntry[iA + iB],
				&psDirBlock->m_asDirEntry[iA + 1 + iB], sizeof(DIRENTRY));
			}
		}

		/* upload directory */
		if (!SendDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "send directory failed\n");
			return FALSE;
		}

		printf("remove complete\n");
		break;

	case _INITIALIZE:
		/* init dir block */
		InitDirBlock(psDirBlock);

		/* upload directory */
		if (!SendDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "send directory failed\n");
			return FALSE;
		}

		printf("initialization complete\n");
		break;

	case _FIX:
		if (!GetDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "get directory failed\n");
			return FALSE;
		}
		psDirHeader->m_acNotUsed[0] = 0x09;
		psDirHeader->m_acNotUsed[1] = 0x04;
		psDirHeader->m_acNotUsed[2] = 0x1d;
		if (!SendDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "send directory failed\n");
			return FALSE;
		}
		printf("fix complete\n");
		break;

	case _DUMP:
		if (!GetDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "get directory failed\n");
			return FALSE;
		}
		if ((lDump + 128) > sizeof(DIRBLOCK))
			return FALSE;
		DumpDirectory(psDirBlock, lDump);
		break;

	/* move up*/
	case _MOVEUP:
		if (!GetDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "get directory failed\n");
			return FALSE;
		}

		/* get file to move up */
		for (; iC < argc; iC++)
		{
			srcfile = argv[iC];
		}
		/* search directory for filename */
		pDE = psDirBlock->m_asDirEntry;
		iCount = psDirHeader->m_ucCountEntry;
		for (iA = 0; iA < iCount; iA++, pDE++)
		{
			if (!strcmp(pDE->m_szName, srcfile))
				break;
		}
		if (iA == iCount)
		{
			fprintf(stderr, "file '%s' not found for move\n", srcfile);
			return FALSE;
		}
		if ( iA == 0 )
		{
			printf( "Error: file already the first file\n", srcfile);
			return FALSE;
		}

		/* shuffle directory */

		memcpy(&psDirBlock1->m_asDirEntry[iA],
		&psDirBlock->m_asDirEntry[iA-1], sizeof(DIRENTRY));
		memcpy(&psDirBlock1->m_asDirEntry[iA-1],
		&psDirBlock->m_asDirEntry[iA], sizeof(DIRENTRY));
		memcpy(&psDirBlock->m_asDirEntry[iA],
		&psDirBlock1->m_asDirEntry[iA], sizeof(DIRENTRY));
		memcpy(&psDirBlock->m_asDirEntry[iA-1],
		&psDirBlock1->m_asDirEntry[iA-1], sizeof(DIRENTRY));

		/* upload directory */
		if (!SendDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "send directory failed\n");
			return FALSE;
		}

		printf("move complete\n");
		break;

	/* move down*/
	case _MOVEDOWN:
		if (!GetDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "get directory failed\n");
			return FALSE;
		}

		/* get file to move down */
		for (; iC < argc; iC++)
		{
			srcfile = argv[iC];
		}
		/* search directory for filename */
		pDE = psDirBlock->m_asDirEntry;
		iCount = psDirHeader->m_ucCountEntry;
		for (iA = 0; iA < iCount; iA++, pDE++)
		{
			if (!strcmp(pDE->m_szName, srcfile))
				break;
		}
		if (iA == iCount)
		{
			fprintf(stderr, "file '%s' not found for move\n", srcfile);
			return FALSE;
		}
		if ( iA == iCount-1 )
		{
			printf( "Error: file already the last file\n", srcfile);
			return FALSE;
		}


		/* shuffle directory */
		memcpy(&psDirBlock1->m_asDirEntry[iA+1],
		&psDirBlock->m_asDirEntry[iA], sizeof(DIRENTRY));
		memcpy(&psDirBlock1->m_asDirEntry[iA],
		&psDirBlock->m_asDirEntry[iA+1], sizeof(DIRENTRY));			
		memcpy(&psDirBlock->m_asDirEntry[iA],
		&psDirBlock1->m_asDirEntry[iA], sizeof(DIRENTRY));			
		memcpy(&psDirBlock->m_asDirEntry[iA+1],
		&psDirBlock1->m_asDirEntry[iA+1], sizeof(DIRENTRY) );			

		/* upload directory */
		if (!SendDirectory(psDirBlock, paucBlock))
		{
			fprintf(stderr, "send directory failed\n");
			return FALSE;
		}

		printf("move complete\n");
		break;
		
	default:
		usage();
		return FALSE;
	}

	free(psDirBlock);
	free(psDirBlock1);
	free(paucBlock);

	return TRUE;
}