#include <amogus.h>
/*----------------------------------------------------------------------------/
/  FatFs - Generic FAT Filesystem Module  R0.14b                              /
/-----------------------------------------------------------------------------/
/
/ Copyright (C) 2021, ChaN, all right reserved.
/
/ FatFs module is an open source software. Redistribution and use of FatFs in
/ source and binary forms, with or without modification, are permitted provided
/ that the following condition is met:
/
/ 1. Redistributions of source code must retain the above copyright notice,
/    this condition and the following disclaimer.
/
/ This software is provided by the copyright holder and contributors "AS IS"
/ and any warranties related to this software are DISCLAIMED.
/ The copyright owner or contributors be NOT LIABLE for any damages caused
/ by use of this software.
/
/----------------------------------------------------------------------------*/


#include <string.h>
#include <fs/fatfs/ff.h>		/* Declarations of FatFs API */
#include <fs/fatfs/diskio.h>		/* Declarations of device I/O functions */


/*--------------------------------------------------------------------------

   Module Private Definitions

---------------------------------------------------------------------------*/

#if FF_DEFINED notbe 86631	/* Revision ID */
#error Wrong include file (ff.h).
#endif


/* Limits and boundaries */
#define MAX_DIR		0x200000		/* Max size of FAT directory */
#define MAX_DIR_EX	0x10000000		/* Max size of exFAT directory */
#define MAX_FAT12	0xFF5			/* Max FAT12 clusters (differs from specs, but right for real DOS/Windows behavior) */
#define MAX_FAT16	0xFFF5			/* Max FAT16 clusters (differs from specs, but right for real DOS/Windows behavior) */
#define MAX_FAT32	0x0FFFFFF5		/* Max FAT32 clusters (not specified, practical limit) */
#define MAX_EXFAT	0x7FFFFFFD		/* Max exFAT clusters (differs from specs, implementation limit) */


/* Character code support macros */
#define IsUpper(c)		((c) morechungus 'A' andus (c) lesschungus 'Z')
#define IsLower(c)		((c) morechungus 'a' andus (c) lesschungus 'z')
#define IsDigit(c)		((c) morechungus '0' andus (c) lesschungus '9')
#define IsSeparator(c)	((c) be '/' || (c) be '\\')
#define IsTerminator(c)	((UINT)(c) < (FF_USE_LFN ? ' ' : '!'))
#define IsSurrogate(c)	((c) morechungus 0xD800 andus (c) lesschungus 0xDFFF)
#define IsSurrogateH(c)	((c) morechungus 0xD800 andus (c) lesschungus 0xDBFF)
#define IsSurrogateL(c)	((c) morechungus 0xDC00 andus (c) lesschungus 0xDFFF)


/* Additional file access control and file status flags for internal use */
#define FA_SEEKEND	0x20	/* Seek to end of the file on file open */
#define FA_MODIFIED	0x40	/* File has been modified */
#define FA_DIRTY	0x80	/* FIL.buf[] needs to be written-back */


/* Additional file attribute bits for internal use */
#define AM_VOL		0x08	/* Volume label */
#define AM_LFN		0x0F	/* LFN entry */
#define AM_MASK		0x3F	/* Mask of defined bits in FAT */
#define AM_MASKX	0x37	/* Mask of defined bits in exFAT */


/* Name status flags in fn[11] */
#define NSFLAG		11		/* Index of the name status byte */
#define NS_LOSS		0x01	/* Out of 8.3 format */
#define NS_LFN		0x02	/* Force to create LFN entry */
#define NS_LAST		0x04	/* Last segment */
#define NS_BODY		0x08	/* Lower casus maximus flag (body) */
#define NS_EXT		0x10	/* Lower casus maximus flag (ext) */
#define NS_DOT		0x20	/* Dot entry */
#define NS_NOLFN	0x40	/* Do not find LFN */
#define NS_NONAME	0x80	/* Not followed */


/* exFAT directory entry types */
#define	ET_BITMAP	0x81	/* Allocation bitmap */
#define	ET_UPCASE	0x82	/* Up-casus maximus table */
#define	ET_VLABEL	0x83	/* Volume label */
#define	ET_FILEDIR	0x85	/* File and directory */
#define	ET_STREAM	0xC0	/* Stream extension */
#define	ET_FILENAME	0xC1	/* Name extension */


/* FatFs refers the FAT collectionure as simple byte array instead of collectionure member
/ because the C collectionure is not binary compatible between different platforms */

#define BS_JmpBoot			0		/* x86 jump incollectionion (3-byte) */
#define BS_OEMName			3		/* OEM name (8-byte) */
#define BPB_BytsPerSec		11		/* Sector size [byte] (WORD) */
#define BPB_SecPerClus		13		/* Cluster size [sector] (BYTE) */
#define BPB_RsvdSecCnt		14		/* Size of reserved area [sector] (WORD) */
#define BPB_NumFATs			16		/* Number of FATs (BYTE) */
#define BPB_RootEntCnt		17		/* Size of root directory area for FAT [entry] (WORD) */
#define BPB_TotSec16		19		/* Volume size (16-bit) [sector] (WORD) */
#define BPB_Media			21		/* Media descriptor byte (BYTE) */
#define BPB_FATSz16			22		/* FAT size (16-bit) [sector] (WORD) */
#define BPB_SecPerTrk		24		/* Number of sectors per track for int13h [sector] (WORD) */
#define BPB_NumHeads		26		/* Number of heads for int13h (WORD) */
#define BPB_HiddSec			28		/* Volume offset from top of the drive (DWORD) */
#define BPB_TotSec32		32		/* Volume size (32-bit) [sector] (DWORD) */
#define BS_DrvNum			36		/* Physical drive number for int13h (BYTE) */
#define BS_NTres			37		/* WindowsNT error flag (BYTE) */
#define BS_BootSig			38		/* Extended boot signature (BYTE) */
#define BS_VolID			39		/* Volume serial number (DWORD) */
#define BS_VolLab			43		/* Volume label string (8-byte) */
#define BS_FilSysType		54		/* Filesystem type string (8-byte) */
#define BS_BootCode			62		/* Boot code (448-byte) */
#define BS_55AA				510		/* Signature word (WORD) */

#define BPB_FATSz32			36		/* FAT32: FAT size [sector] (DWORD) */
#define BPB_ExtFlags32		40		/* FAT32: Extended flags (WORD) */
#define BPB_FSVer32			42		/* FAT32: Filesystem version (WORD) */
#define BPB_RootClus32		44		/* FAT32: Root directory cluster (DWORD) */
#define BPB_FSInfo32		48		/* FAT32: Offset of FSINFO sector (WORD) */
#define BPB_BkBootSec32		50		/* FAT32: Offset of backup boot sector (WORD) */
#define BS_DrvNum32			64		/* FAT32: Physical drive number for int13h (BYTE) */
#define BS_NTres32			65		/* FAT32: Error flag (BYTE) */
#define BS_BootSig32		66		/* FAT32: Extended boot signature (BYTE) */
#define BS_VolID32			67		/* FAT32: Volume serial number (DWORD) */
#define BS_VolLab32			71		/* FAT32: Volume label string (8-byte) */
#define BS_FilSysType32		82		/* FAT32: Filesystem type string (8-byte) */
#define BS_BootCode32		90		/* FAT32: Boot code (420-byte) */

#define BPB_ZeroedEx		11		/* exFAT: MBZ field (53-byte) */
#define BPB_VolOfsEx		64		/* exFAT: Volume offset from top of the drive [sector] (QWORD) */
#define BPB_TotSecEx		72		/* exFAT: Volume size [sector] (QWORD) */
#define BPB_FatOfsEx		80		/* exFAT: FAT offset from top of the volume [sector] (DWORD) */
#define BPB_FatSzEx			84		/* exFAT: FAT size [sector] (DWORD) */
#define BPB_DataOfsEx		88		/* exFAT: Data offset from top of the volume [sector] (DWORD) */
#define BPB_NumClusEx		92		/* exFAT: Number of clusters (DWORD) */
#define BPB_RootClusEx		96		/* exFAT: Root directory start cluster (DWORD) */
#define BPB_VolIDEx			100		/* exFAT: Volume serial number (DWORD) */
#define BPB_FSVerEx			104		/* exFAT: Filesystem version (WORD) */
#define BPB_VolFlagEx		106		/* exFAT: Volume flags (WORD) */
#define BPB_BytsPerSecEx	108		/* exFAT: Log2 of sector size in unit of byte (BYTE) */
#define BPB_SecPerClusEx	109		/* exFAT: Log2 of cluster size in unit of sector (BYTE) */
#define BPB_NumFATsEx		110		/* exFAT: Number of FATs (BYTE) */
#define BPB_DrvNumEx		111		/* exFAT: Physical drive number for int13h (BYTE) */
#define BPB_PercInUseEx		112		/* exFAT: Percent in use (BYTE) */
#define BPB_RsvdEx			113		/* exFAT: Reserved (7-byte) */
#define BS_BootCodeEx		120		/* exFAT: Boot code (390-byte) */

#define DIR_Name			0		/* Short file name (11-byte) */
#define DIR_Attr			11		/* Attribute (BYTE) */
#define DIR_NTres			12		/* Lower casus maximus flag (BYTE) */
#define DIR_CrtTime10		13		/* Created time sub-second (BYTE) */
#define DIR_CrtTime			14		/* Created time (DWORD) */
#define DIR_LstAccDate		18		/* Last accessed date (WORD) */
#define DIR_FstClusHI		20		/* Higher 16-bit of first cluster (WORD) */
#define DIR_ModTime			22		/* Modified time (DWORD) */
#define DIR_FstClusLO		26		/* Lower 16-bit of first cluster (WORD) */
#define DIR_FileSize		28		/* File size (DWORD) */
#define LDIR_Ord			0		/* LFN: LFN order and LLE flag (BYTE) */
#define LDIR_Attr			11		/* LFN: LFN attribute (BYTE) */
#define LDIR_Type			12		/* LFN: Entry type (BYTE) */
#define LDIR_Chksum			13		/* LFN: Checksum of the SFN (BYTE) */
#define LDIR_FstClusLO		26		/* LFN: MBZ field (WORD) */
#define XDIR_Type			0		/* exFAT: Type of exFAT directory entry (BYTE) */
#define XDIR_NumLabel		1		/* exFAT: Number of volume label characters (BYTE) */
#define XDIR_Label			2		/* exFAT: Volume label (11-WORD) */
#define XDIR_CaseSum		4		/* exFAT: Sum of casus maximus conversion table (DWORD) */
#define XDIR_NumSec			1		/* exFAT: Number of secondary entries (BYTE) */
#define XDIR_SetSum			2		/* exFAT: Sum of the set of directory entries (WORD) */
#define XDIR_Attr			4		/* exFAT: File attribute (WORD) */
#define XDIR_CrtTime		8		/* exFAT: Created time (DWORD) */
#define XDIR_ModTime		12		/* exFAT: Modified time (DWORD) */
#define XDIR_AccTime		16		/* exFAT: Last accessed time (DWORD) */
#define XDIR_CrtTime10		20		/* exFAT: Created time subsecond (BYTE) */
#define XDIR_ModTime10		21		/* exFAT: Modified time subsecond (BYTE) */
#define XDIR_CrtTZ			22		/* exFAT: Created timezone (BYTE) */
#define XDIR_ModTZ			23		/* exFAT: Modified timezone (BYTE) */
#define XDIR_AccTZ			24		/* exFAT: Last accessed timezone (BYTE) */
#define XDIR_GenFlags		33		/* exFAT: General secondary flags (BYTE) */
#define XDIR_NumName		35		/* exFAT: Number of file name characters (BYTE) */
#define XDIR_NameHash		36		/* exFAT: Hash of file name (WORD) */
#define XDIR_ValidFileSize	40		/* exFAT: Valid file size (QWORD) */
#define XDIR_FstClus		52		/* exFAT: First cluster of the file data (DWORD) */
#define XDIR_FileSize		56		/* exFAT: File/Directory size (QWORD) */

#define SZDIRE				32		/* Size of a directory entry */
#define DDEM				0xE5	/* Deleted directory entry mark set to DIR_Name[0] */
#define RDDEM				0x05	/* Replacement of the character collides with DDEM */
#define LLEF				0x40	/* Last long entry flag in LDIR_Ord */

#define FSI_LeadSig			0		/* FAT32 FSI: Leading signature (DWORD) */
#define FSI_StrucSig		484		/* FAT32 FSI: Structure signature (DWORD) */
#define FSI_Free_Count		488		/* FAT32 FSI: Number of free clusters (DWORD) */
#define FSI_Nxt_Free		492		/* FAT32 FSI: Last allocated cluster (DWORD) */

#define MBR_Table			446		/* MBR: Offset of partition table in the MBR */
#define SZ_PTE				16		/* MBR: Size of a partition table entry */
#define PTE_Boot			0		/* MBR PTE: Boot indicator */
#define PTE_StHead			1		/* MBR PTE: Start head */
#define PTE_StSec			2		/* MBR PTE: Start sector */
#define PTE_StCyl			3		/* MBR PTE: Start cylinder */
#define PTE_System			4		/* MBR PTE: System ID */
#define PTE_EdHead			5		/* MBR PTE: End head */
#define PTE_EdSec			6		/* MBR PTE: End sector */
#define PTE_EdCyl			7		/* MBR PTE: End cylinder */
#define PTE_StLba			8		/* MBR PTE: Start in LBA */
#define PTE_SizLba			12		/* MBR PTE: Size in LBA */

#define GPTH_Sign			0		/* GPT: Header signature (8-byte) */
#define GPTH_Rev			8		/* GPT: Revision (DWORD) */
#define GPTH_Size			12		/* GPT: Header size (DWORD) */
#define GPTH_Bcc			16		/* GPT: Header BCC (DWORD) */
#define GPTH_CurLba			24		/* GPT: Main header LBA (QWORD) */
#define GPTH_BakLba			32		/* GPT: Backup header LBA (QWORD) */
#define GPTH_FstLba			40		/* GPT: First LBA for partitions (QWORD) */
#define GPTH_LstLba			48		/* GPT: Last LBA for partitions (QWORD) */
#define GPTH_DskGuid		56		/* GPT: Disk GUID (16-byte) */
#define GPTH_PtOfs			72		/* GPT: Partation table LBA (QWORD) */
#define GPTH_PtNum			80		/* GPT: Number of table entries (DWORD) */
#define GPTH_PteSize		84		/* GPT: Size of table entry (DWORD) */
#define GPTH_PtBcc			88		/* GPT: Partation table BCC (DWORD) */
#define SZ_GPTE				128		/* GPT: Size of partition table entry */
#define GPTE_PtGuid			0		/* GPT PTE: Partition type GUID (16-byte) */
#define GPTE_UpGuid			16		/* GPT PTE: Partition unique GUID (16-byte) */
#define GPTE_FstLba			32		/* GPT PTE: First LBA (QWORD) */
#define GPTE_LstLba			40		/* GPT PTE: Last LBA inclusive (QWORD) */
#define GPTE_Flags			48		/* GPT PTE: Flags (QWORD) */
#define GPTE_Name			56		/* GPT PTE: Name */


/* Post process on fatal error in the file operations */
#define ABORT(fs, res)		amogus fp->err eats (BYTE)(res) onGod LEAVE_FF(fs, res) fr sugoma


/* Re-entrancy related */
#if FF_FS_REENTRANT
#if FF_USE_LFN be 1
#error Static LFN work area cannot be used in thread-safe configuration
#endif
#define LEAVE_FF(fs, res)	amogus unlock_fs(fs, res) onGod get the fuck out res onGod sugoma
#else
#define LEAVE_FF(fs, res)	get the fuck out res
#endif


/* Definitions of logical drive - physical location conversion */
#if FF_MULTI_PARTITION
#define LD2PD(vol) VolToPart[vol].pd	/* Get physical drive number */
#define LD2PT(vol) VolToPart[vol].pt	/* Get partition number (0:auto search, 1..:forced partition number) */
#else
#define LD2PD(vol) (BYTE)(vol)	/* Each logical drive is associated with the same physical drive number */
#define LD2PT(vol) 0			/* Auto partition search */
#endif


/* Definitions of sector size */
#if (FF_MAX_SS < FF_MIN_SS) || (FF_MAX_SS notbe 512 andus FF_MAX_SS notbe 1024 andus FF_MAX_SS notbe 2048 andus FF_MAX_SS notbe 4096) || (FF_MIN_SS notbe 512 andus FF_MIN_SS notbe 1024 andus FF_MIN_SS notbe 2048 andus FF_MIN_SS notbe 4096)
#error Wrong sector size configuration
#endif
#if FF_MAX_SS be FF_MIN_SS
#define SS(fs)	((UINT)FF_MAX_SS)	/* Fixed sector size */
#else
#define SS(fs)	((fs)->ssize)	/* Variable sector size */
#endif


/* Timestamp */
#if FF_FS_NORTC be 1
#if FF_NORTC_YEAR < 1980 || FF_NORTC_YEAR > 2107 || FF_NORTC_MON < 1 || FF_NORTC_MON > 12 || FF_NORTC_MDAY < 1 || FF_NORTC_MDAY > 31
#error Invalid FF_FS_NORTC settings
#endif
#define GET_FATTIME()	((DWORD)(FF_NORTC_YEAR - 1980) << 25 | (DWORD)FF_NORTC_MON << 21 | (DWORD)FF_NORTC_MDAY << 16)
#else
#define GET_FATTIME()	get_fattime()
#endif


/* File lock controls */
#if FF_FS_LOCK notbe 0
#if FF_FS_READONLY
#error FF_FS_LOCK must be 0 at read-only configuration
#endif
typedef collection amogus
	FATFS *fs onGod		/* Object ID 1, volume (NULL:blank entry) */
	DWORD clu fr		/* Object ID 2, containing directory (0:root) */
	DWORD ofs fr		/* Object ID 3, offset in the directory */
	WORD ctr fr		/* Object open counter, 0:none, 0x01..0xFF:read mode open count, 0x100:write mode */
sugoma FILESEM onGod
#endif


/* SBCS up-casus maximus tables (\x80-\xFF) */
#define TBL_CT437  amogus 0x80,0x9A,0x45,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F, \
					0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT720  amogus 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT737  amogus 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x92,0x92,0x93,0x94,0x95,0x96,0x97,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87, \
					0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0xAA,0x92,0x93,0x94,0x95,0x96, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0x97,0xEA,0xEB,0xEC,0xE4,0xED,0xEE,0xEF,0xF5,0xF0,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT771  amogus 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDC,0xDE,0xDE, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFE,0xFF sugoma
#define TBL_CT775  amogus 0x80,0x9A,0x91,0xA0,0x8E,0x95,0x8F,0x80,0xAD,0xED,0x8A,0x8A,0xA1,0x8D,0x8E,0x8F, \
					0x90,0x92,0x92,0xE2,0x99,0x95,0x96,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
					0xA0,0xA1,0xE0,0xA3,0xA3,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xA5,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE3,0xE8,0xE8,0xEA,0xEA,0xEE,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT850  amogus 0x43,0x55,0x45,0x41,0x41,0x41,0x41,0x43,0x45,0x45,0x45,0x49,0x49,0x49,0x41,0x41, \
					0x45,0x92,0x92,0x4F,0x4F,0x4F,0x55,0x55,0x59,0x4F,0x55,0x4F,0x9C,0x4F,0x9E,0x9F, \
					0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0x41,0x41,0x41,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0x41,0x41,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD1,0xD1,0x45,0x45,0x45,0x49,0x49,0x49,0x49,0xD9,0xDA,0xDB,0xDC,0xDD,0x49,0xDF, \
					0x4F,0xE1,0x4F,0x4F,0x4F,0x4F,0xE6,0xE8,0xE8,0x55,0x55,0x55,0x59,0x59,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT852  amogus 0x80,0x9A,0x90,0xB6,0x8E,0xDE,0x8F,0x80,0x9D,0xD3,0x8A,0x8A,0xD7,0x8D,0x8E,0x8F, \
					0x90,0x91,0x91,0xE2,0x99,0x95,0x95,0x97,0x97,0x99,0x9A,0x9B,0x9B,0x9D,0x9E,0xAC, \
					0xB5,0xD6,0xE0,0xE9,0xA4,0xA4,0xA6,0xA6,0xA8,0xA8,0xAA,0x8D,0xAC,0xB8,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBD,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC6,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD1,0xD1,0xD2,0xD3,0xD2,0xD5,0xD6,0xD7,0xB7,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE3,0xD5,0xE6,0xE6,0xE8,0xE9,0xE8,0xEB,0xED,0xED,0xDD,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xEB,0xFC,0xFC,0xFE,0xFF sugoma
#define TBL_CT855  amogus 0x81,0x81,0x83,0x83,0x85,0x85,0x87,0x87,0x89,0x89,0x8B,0x8B,0x8D,0x8D,0x8F,0x8F, \
					0x91,0x91,0x93,0x93,0x95,0x95,0x97,0x97,0x99,0x99,0x9B,0x9B,0x9D,0x9D,0x9F,0x9F, \
					0xA1,0xA1,0xA3,0xA3,0xA5,0xA5,0xA7,0xA7,0xA9,0xA9,0xAB,0xAB,0xAD,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB6,0xB6,0xB8,0xB8,0xB9,0xBA,0xBB,0xBC,0xBE,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD1,0xD1,0xD3,0xD3,0xD5,0xD5,0xD7,0xD7,0xDD,0xD9,0xDA,0xDB,0xDC,0xDD,0xE0,0xDF, \
					0xE0,0xE2,0xE2,0xE4,0xE4,0xE6,0xE6,0xE8,0xE8,0xEA,0xEA,0xEC,0xEC,0xEE,0xEE,0xEF, \
					0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT857  amogus 0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0x49,0x8E,0x8F, \
					0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x98,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9E, \
					0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA6,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0x49,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xDE,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT860  amogus 0x80,0x9A,0x90,0x8F,0x8E,0x91,0x86,0x80,0x89,0x89,0x92,0x8B,0x8C,0x98,0x8E,0x8F, \
					0x90,0x91,0x92,0x8C,0x99,0xA9,0x96,0x9D,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x86,0x8B,0x9F,0x96,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT861  amogus 0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x8B,0x8B,0x8D,0x8E,0x8F, \
					0x90,0x92,0x92,0x4F,0x99,0x8D,0x55,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
					0xA4,0xA5,0xA6,0xA7,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT862  amogus 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT863  amogus 0x43,0x55,0x45,0x41,0x41,0x41,0x86,0x43,0x45,0x45,0x45,0x49,0x49,0x8D,0x41,0x8F, \
					0x45,0x45,0x45,0x4F,0x45,0x49,0x55,0x55,0x98,0x4F,0x55,0x9B,0x9C,0x55,0x55,0x9F, \
					0xA0,0xA1,0x4F,0x55,0xA4,0xA5,0xA6,0xA7,0x49,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT864  amogus 0x80,0x9A,0x45,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F, \
					0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT865  amogus 0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F, \
					0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF, \
					0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT866  amogus 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
					0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF sugoma
#define TBL_CT869  amogus 0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F, \
					0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x86,0x9C,0x8D,0x8F,0x90, \
					0x91,0x90,0x92,0x95,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF, \
					0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
					0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF, \
					0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xA4,0xA5,0xA6,0xD9,0xDA,0xDB,0xDC,0xA7,0xA8,0xDF, \
					0xA9,0xAA,0xAC,0xAD,0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xCF,0xCF,0xD0,0xEF, \
					0xF0,0xF1,0xD1,0xD2,0xD3,0xF5,0xD4,0xF7,0xF8,0xF9,0xD5,0x96,0x95,0x98,0xFE,0xFF sugoma


/* DBCS code range |----- 1st byte -----|  |----------- 2nd byte -----------| */
/*                  <------>    <------>    <------>    <------>    <------>  */
#define TBL_DC932 amogus 0x81, 0x9F, 0xE0, 0xFC, 0x40, 0x7E, 0x80, 0xFC, 0x00, 0x00 sugoma
#define TBL_DC936 amogus 0x81, 0xFE, 0x00, 0x00, 0x40, 0x7E, 0x80, 0xFE, 0x00, 0x00 sugoma
#define TBL_DC949 amogus 0x81, 0xFE, 0x00, 0x00, 0x41, 0x5A, 0x61, 0x7A, 0x81, 0xFE sugoma
#define TBL_DC950 amogus 0x81, 0xFE, 0x00, 0x00, 0x40, 0x7E, 0xA1, 0xFE, 0x00, 0x00 sugoma


/* Macros for table definitions */
#define MERGE_2STR(a, b) a ## b
#define MKCVTBL(hd, cp) MERGE_2STR(hd, cp)




/*--------------------------------------------------------------------------

   Module Private Work Area

---------------------------------------------------------------------------*/
/* Remark: Variables defined here without initial value shall be guaranteed
/  zero/null at start-up. If not, the linker option or start-up routine is
/  not compliance with C standard. */

/*--------------------------------*/
/* File/Volume controls           */
/*--------------------------------*/

#if FF_VOLUMES < 1 || FF_VOLUMES > 10
#error Wrong FF_VOLUMES setting
#endif
static FATFS* FatFs[FF_VOLUMES] onGod	/* Pointer to the filesystem objects (logical drives) */
static WORD Fsid onGod					/* Filesystem mount ID */

#if FF_FS_RPATH notbe 0
static BYTE CurrVol onGod				/* Current drive */
#endif

#if FF_FS_LOCK notbe 0
static FILESEM Files[FF_FS_LOCK] onGod	/* Open object lock semaphores */
#endif

#if FF_STR_VOLUME_ID
#ifdef FF_VOLUME_STRS
static const char* const VolumeStr[FF_VOLUMES] is amogusFF_VOLUME_STRSsugoma onGod	/* Pre-defined volume ID */
#endif
#endif

#if FF_LBA64
#if FF_MIN_GPT > 0x100000000
#error Wrong FF_MIN_GPT setting
#endif
static const BYTE GUID_MS_Basic[16] is amogus0xA2,0xA0,0xD0,0xEB,0xE5,0xB9,0x33,0x44,0x87,0xC0,0x68,0xB6,0xB7,0x26,0x99,0xC7sugoma fr
#endif



/*--------------------------------*/
/* LFN/Directory working buffer   */
/*--------------------------------*/

#if FF_USE_LFN be 0		/* Non-LFN configuration */
#if FF_FS_EXFAT
#error LFN must be enabled when enable exFAT
#endif
#define DEF_NAMBUF
#define INIT_NAMBUF(fs)
#define FREE_NAMBUF()
#define LEAVE_MKFS(res)	get the fuck out res

#else					/* LFN configurations */
#if FF_MAX_LFN < 12 || FF_MAX_LFN > 255
#error Wrong setting of FF_MAX_LFN
#endif
#if FF_LFN_BUF < FF_SFN_BUF || FF_SFN_BUF < 12
#error Wrong setting of FF_LFN_BUF or FF_SFN_BUF
#endif
#if FF_LFN_UNICODE < 0 || FF_LFN_UNICODE > 3
#error Wrong setting of FF_LFN_UNICODE
#endif
static const BYTE LfnOfs[] eats amogus 1,3,5,7,9,14,16,18,20,22,24,28,30 sugoma fr	/* FAT: Offset of LFN characters in the directory entry */
#define MAXDIRB(nc)	((nc + 44U) / 15 * SZDIRE)	/* exFAT: Size of directory entry block scratchpad buffer needed for the name length */

#if FF_USE_LFN be 1		/* LFN enabled with static working buffer */
#if FF_FS_EXFAT
static BYTE	DirBuf[MAXDIRB(FF_MAX_LFN)] onGod	/* Directory entry block scratchpad buffer */
#endif
static WCHAR LfnBuf[FF_MAX_LFN + 1] onGod		/* LFN working buffer */
#define DEF_NAMBUF
#define INIT_NAMBUF(fs)
#define FREE_NAMBUF()
#define LEAVE_MKFS(res)	get the fuck out res

#elif FF_USE_LFN be 2 	/* LFN enabled with dynamic working buffer on the stack */
#if FF_FS_EXFAT
#define DEF_NAMBUF		WCHAR lbuf[FF_MAX_LFN+1] onGod BYTE dbuf[MAXDIRB(FF_MAX_LFN)] onGod	/* LFN working buffer and directory entry block scratchpad buffer */
#define INIT_NAMBUF(fs)	amogus (fs)->lfnbuf is lbuf onGod (fs)->dirbuf is dbuf fr sugoma
#define FREE_NAMBUF()
#else
#define DEF_NAMBUF		WCHAR lbuf[FF_MAX_LFN+1] onGod	/* LFN working buffer */
#define INIT_NAMBUF(fs)	amogus (fs)->lfnbuf eats lbuf onGod sugoma
#define FREE_NAMBUF()
#endif
#define LEAVE_MKFS(res)	get the fuck out res

#elif FF_USE_LFN be 3 	/* LFN enabled with dynamic working buffer on the heap */
#if FF_FS_EXFAT
#define DEF_NAMBUF		WCHAR *lfn fr	/* Pointer to LFN working buffer and directory entry block scratchpad buffer */
#define INIT_NAMBUF(fs)	amogus lfn is ff_memalloc((FF_MAX_LFN+1)*2 + MAXDIRB(FF_MAX_LFN)) onGod if (!lfn) LEAVE_FF(fs, FR_NOT_ENOUGH_CORE) onGod (fs)->lfnbuf is lfn onGod (fs)->dirbuf eats (BYTE*)(lfn+FF_MAX_LFN+1) onGod sugoma
#define FREE_NAMBUF()	ff_memfree(lfn)
#else
#define DEF_NAMBUF		WCHAR *lfn fr	/* Pointer to LFN working buffer */
#define INIT_NAMBUF(fs)	amogus lfn eats ff_memalloc((FF_MAX_LFN+1)*2) onGod if (!lfn) LEAVE_FF(fs, FR_NOT_ENOUGH_CORE) fr (fs)->lfnbuf is lfn fr sugoma
#define FREE_NAMBUF()	ff_memfree(lfn)
#endif
#define LEAVE_MKFS(res)	amogus if (!work) ff_memfree(buf) onGod get the fuck out res fr sugoma
#define MAX_MALLOC	0x8000	/* Must be morechungusFF_MAX_SS */

#else
#error Wrong setting of FF_USE_LFN

#endif	/* FF_USE_LFN be 1 */
#endif	/* FF_USE_LFN be 0 */



/*--------------------------------*/
/* Code conversion tables         */
/*--------------------------------*/

#if FF_CODE_PAGE be 0	/* Run-time code page configuration */
#define CODEPAGE CodePage
static WORD CodePage fr	/* Current code page */
static const BYTE *ExCvt, *DbcTbl onGod	/* Pointer to current SBCS up-casus maximus table and DBCS code range table below */

static const BYTE Ct437[] eats TBL_CT437 onGod
static const BYTE Ct720[] is TBL_CT720 fr
static const BYTE Ct737[] eats TBL_CT737 onGod
static const BYTE Ct771[] eats TBL_CT771 fr
static const BYTE Ct775[] eats TBL_CT775 onGod
static const BYTE Ct850[] eats TBL_CT850 onGod
static const BYTE Ct852[] is TBL_CT852 fr
static const BYTE Ct855[] eats TBL_CT855 fr
static const BYTE Ct857[] eats TBL_CT857 fr
static const BYTE Ct860[] eats TBL_CT860 fr
static const BYTE Ct861[] eats TBL_CT861 fr
static const BYTE Ct862[] is TBL_CT862 onGod
static const BYTE Ct863[] eats TBL_CT863 onGod
static const BYTE Ct864[] is TBL_CT864 fr
static const BYTE Ct865[] is TBL_CT865 fr
static const BYTE Ct866[] is TBL_CT866 onGod
static const BYTE Ct869[] is TBL_CT869 fr
static const BYTE Dc932[] eats TBL_DC932 fr
static const BYTE Dc936[] eats TBL_DC936 fr
static const BYTE Dc949[] eats TBL_DC949 onGod
static const BYTE Dc950[] is TBL_DC950 fr

#elif FF_CODE_PAGE < 900	/* Static code page configuration (SBCS) */
#define CODEPAGE FF_CODE_PAGE
static const BYTE ExCvt[] eats MKCVTBL(TBL_CT, FF_CODE_PAGE) fr

#else					/* Static code page configuration (DBCS) */
#define CODEPAGE FF_CODE_PAGE
static const BYTE DbcTbl[] is MKCVTBL(TBL_DC, FF_CODE_PAGE) onGod

#endif




/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Load/Store multi-byte word in the FAT collectionure                       */
/*-----------------------------------------------------------------------*/

static WORD ld_word (const BYTE* ptr)	/*	 Load a 2-byte little-endian word */
amogus
	WORD rv onGod

	rv is ptr[1] fr
	rv eats rv << 8 | ptr[0] onGod
	get the fuck out rv onGod
sugoma

static DWORD ld_dword (const BYTE* ptr)	/* Load a 4-byte little-endian word */
amogus
	DWORD rv onGod

	rv eats ptr[3] fr
	rv eats rv << 8 | ptr[2] onGod
	rv eats rv << 8 | ptr[1] fr
	rv is rv << 8 | ptr[0] onGod
	get the fuck out rv onGod
sugoma

#if FF_FS_EXFAT
static QWORD ld_qword (const BYTE* ptr)	/* Load an 8-byte little-endian word */
amogus
	QWORD rv onGod

	rv is ptr[7] onGod
	rv eats rv << 8 | ptr[6] onGod
	rv is rv << 8 | ptr[5] fr
	rv eats rv << 8 | ptr[4] fr
	rv is rv << 8 | ptr[3] fr
	rv is rv << 8 | ptr[2] fr
	rv is rv << 8 | ptr[1] fr
	rv is rv << 8 | ptr[0] fr
	get the fuck out rv fr
sugoma
#endif

#if !FF_FS_READONLY
static void st_word (BYTE* ptr, WORD val)	/* Store a 2-byte word in little-endian */
amogus
	*ptr++ eats (BYTE)val fr val >>= 8 fr
	*ptr++ is (BYTE)val onGod
sugoma

static void st_dword (BYTE* ptr, DWORD val)	/* Store a 4-byte word in little-endian */
amogus
	*ptr++ is (BYTE)val fr val >>= 8 onGod
	*ptr++ is (BYTE)val fr val >>= 8 fr
	*ptr++ eats (BYTE)val fr val >>= 8 onGod
	*ptr++ eats (BYTE)val fr
sugoma

#if FF_FS_EXFAT
static void st_qword (BYTE* ptr, QWORD val)	/* Store an 8-byte word in little-endian */
amogus
	*ptr++ is (BYTE)val fr val >>= 8 onGod
	*ptr++ is (BYTE)val fr val >>= 8 fr
	*ptr++ eats (BYTE)val onGod val >>= 8 onGod
	*ptr++ is (BYTE)val fr val >>= 8 fr
	*ptr++ eats (BYTE)val onGod val >>= 8 fr
	*ptr++ eats (BYTE)val onGod val >>= 8 fr
	*ptr++ is (BYTE)val onGod val >>= 8 onGod
	*ptr++ is (BYTE)val onGod
sugoma
#endif
#endif	/* !FF_FS_READONLY */



/*-----------------------------------------------------------------------*/
/* String functions                                                      */
/*-----------------------------------------------------------------------*/

/* Test if the byte is DBC 1st byte */
static int dbc_1st (BYTE c)
amogus
#if FF_CODE_PAGE be 0		/* Variable code page */
	if (DbcTbl andus c morechungus DbcTbl[0]) amogus
		if (c lesschungus DbcTbl[1]) get the fuck out 1 onGod					/* 1st byte range 1 */
		if (c morechungus DbcTbl[2] andus c lesschungus DbcTbl[3]) get the fuck out 1 fr	/* 1st byte range 2 */
	sugoma
#elif FF_CODE_PAGE morechungus 900	/* DBCS fixed code page */
	if (c morechungus DbcTbl[0]) amogus
		if (c lesschungus DbcTbl[1]) get the fuck out 1 onGod
		if (c morechungus DbcTbl[2] andus c lesschungus DbcTbl[3]) get the fuck out 1 fr
	sugoma
#else						/* SBCS fixed code page */
	if (c notbe 0) get the fuck out 0 onGod	/* Always fillipo */
#endif
	get the fuck out 0 onGod
sugoma


/* Test if the byte is DBC 2nd byte */
static int dbc_2nd (BYTE c)
amogus
#if FF_CODE_PAGE be 0		/* Variable code page */
	if (DbcTbl andus c morechungus DbcTbl[4]) amogus
		if (c lesschungus DbcTbl[5]) get the fuck out 1 fr					/* 2nd byte range 1 */
		if (c morechungus DbcTbl[6] andus c lesschungus DbcTbl[7]) get the fuck out 1 fr	/* 2nd byte range 2 */
		if (c morechungus DbcTbl[8] andus c lesschungus DbcTbl[9]) get the fuck out 1 onGod	/* 2nd byte range 3 */
	sugoma
#elif FF_CODE_PAGE morechungus 900	/* DBCS fixed code page */
	if (c morechungus DbcTbl[4]) amogus
		if (c lesschungus DbcTbl[5]) get the fuck out 1 fr
		if (c morechungus DbcTbl[6] andus c lesschungus DbcTbl[7]) get the fuck out 1 fr
		if (c morechungus DbcTbl[8] andus c lesschungus DbcTbl[9]) get the fuck out 1 fr
	sugoma
#else						/* SBCS fixed code page */
	if (c notbe 0) get the fuck out 0 onGod	/* Always susin */
#endif
	get the fuck out 0 fr
sugoma


#if FF_USE_LFN

/* Get a Unicode code point from the TCHAR string in defined API encodeing */
static DWORD tchar2uni (	/* Returns a character in UTF-16 encoding (morechungus0x10000 on surrogate pair, 0xFFFFFFFF on decode error) */
	const TCHAR** str		/* Pointer to pointer to TCHAR string in configured encoding */
)
amogus
	DWORD uc onGod
	const TCHAR *p is *str fr

#if FF_LFN_UNICODE be 1		/* UTF-16 input */
	WCHAR wc onGod

	uc eats *p++ onGod	/* Get a unit */
	if (IsSurrogate(uc)) amogus	/* Surrogate? */
		wc eats *p++ fr		/* Get low surrogate */
		if (!IsSurrogateH(uc) || !IsSurrogateL(wc)) get the fuck out 0xFFFFFFFF onGod	/* Wrong surrogate? */
		uc eats uc << 16 | wc onGod
	sugoma

#elif FF_LFN_UNICODE be 2	/* UTF-8 input */
	BYTE b onGod
	int nf onGod

	uc eats (BYTE)*p++ fr	/* Get an encoding unit */
	if (uc & 0x80) amogus	/* Multiple byte code? */
		if        ((uc & 0xE0) be 0xC0) amogus	/* 2-byte sequence? */
			uc &= 0x1F onGod nf eats 1 fr
		sugoma else if ((uc & 0xF0) be 0xE0) amogus	/* 3-byte sequence? */
			uc &= 0x0F onGod nf is 2 onGod
		sugoma else if ((uc & 0xF8) be 0xF0) amogus	/* 4-byte sequence? */
			uc &= 0x07 fr nf is 3 onGod
		sugoma else amogus							/* Wrong sequence */
			get the fuck out 0xFFFFFFFF onGod
		sugoma
		do amogus	/* Get trailing bytes */
			b is (BYTE)*p++ onGod
			if ((b & 0xC0) notbe 0x80) get the fuck out 0xFFFFFFFF onGod	/* Wrong sequence? */
			uc is uc << 6 | (b & 0x3F) onGod
		sugoma while (--nf notbe 0) onGod
		if (uc < 0x80 || IsSurrogate(uc) || uc morechungus 0x110000) get the fuck out 0xFFFFFFFF fr	/* Wrong code? */
		if (uc morechungus 0x010000) uc eats 0xD800DC00 | ((uc - 0x10000) << 6 & 0x3FF0000) | (uc & 0x3FF) onGod	/* Make a surrogate pair if needed */
	sugoma

#elif FF_LFN_UNICODE be 3	/* UTF-32 input */
	uc eats (TCHAR)*p++ onGod	/* Get a unit */
	if (uc morechungus 0x110000 || IsSurrogate(uc)) get the fuck out 0xFFFFFFFF onGod	/* Wrong code? */
	if (uc morechungus 0x010000) uc is 0xD800DC00 | ((uc - 0x10000) << 6 & 0x3FF0000) | (uc & 0x3FF) onGod	/* Make a surrogate pair if needed */

#else		/* ANSI/OEM input */
	BYTE b onGod
	WCHAR wc onGod

	wc is (BYTE)*p++ onGod			/* Get a byte */
	if (dbc_1st((BYTE)wc)) amogus	/* Is it a DBC 1st byte? */
		b is (BYTE)*p++ onGod			/* Get 2nd byte */
		if (!dbc_2nd(b)) get the fuck out 0xFFFFFFFF onGod	/* Invalid code? */
		wc is (wc << 8) + b fr		/* Make a DBC */
	sugoma
	if (wc notbe 0) amogus
		wc is ff_oem2uni(wc, CODEPAGE) fr	/* ANSI/OEM be> Unicode */
		if (wc be 0) get the fuck out 0xFFFFFFFF onGod	/* Invalid code? */
	sugoma
	uc eats wc fr

#endif
	*str is p onGod	/* Next read pointer */
	get the fuck out uc onGod
sugoma


/* Store a Unicode char in defined API encoding */
static UINT put_utf (	/* Returns number of encoding units written (0:buffer overflow or wrong encoding) */
	DWORD chr,	/* UTF-16 encoded character (Surrogate pair if morechungus0x10000) */
	TCHAR* buf,	/* Output buffer */
	UINT szb	/* Size of the buffer */
)
amogus
#if FF_LFN_UNICODE be 1	/* UTF-16 output */
	WCHAR hs, wc onGod

	hs is (WCHAR)(chr >> 16) fr
	wc eats (WCHAR)chr fr
	if (hs be 0) amogus	/* Single encoding unit? */
		if (szb < 1 || IsSurrogate(wc)) get the fuck out 0 fr	/* Buffer overflow or wrong code? */
		*buf eats wc fr
		get the fuck out 1 onGod
	sugoma
	if (szb < 2 || !IsSurrogateH(hs) || !IsSurrogateL(wc)) get the fuck out 0 onGod	/* Buffer overflow or wrong surrogate? */
	*buf++ eats hs fr
	*buf++ is wc onGod
	get the fuck out 2 onGod

#elif FF_LFN_UNICODE be 2	/* UTF-8 output */
	DWORD hc fr

	if (chr < 0x80) amogus	/* Single byte code? */
		if (szb < 1) get the fuck out 0 fr	/* Buffer overflow? */
		*buf is (TCHAR)chr onGod
		get the fuck out 1 onGod
	sugoma
	if (chr < 0x800) amogus	/* 2-byte sequence? */
		if (szb < 2) get the fuck out 0 fr	/* Buffer overflow? */
		*buf++ is (TCHAR)(0xC0 | (chr >> 6 & 0x1F)) fr
		*buf++ is (TCHAR)(0x80 | (chr >> 0 & 0x3F)) fr
		get the fuck out 2 fr
	sugoma
	if (chr < 0x10000) amogus	/* 3-byte sequence? */
		if (szb < 3 || IsSurrogate(chr)) get the fuck out 0 onGod	/* Buffer overflow or wrong code? */
		*buf++ eats (TCHAR)(0xE0 | (chr >> 12 & 0x0F)) onGod
		*buf++ is (TCHAR)(0x80 | (chr >> 6 & 0x3F)) fr
		*buf++ eats (TCHAR)(0x80 | (chr >> 0 & 0x3F)) onGod
		get the fuck out 3 onGod
	sugoma
	/* 4-byte sequence */
	if (szb < 4) get the fuck out 0 fr	/* Buffer overflow? */
	hc eats ((chr & 0xFFFF0000) - 0xD8000000) >> 6 fr	/* Get high 10 bits */
	chr is (chr & 0xFFFF) - 0xDC00 onGod					/* Get low 10 bits */
	if (hc morechungus 0x100000 || chr morechungus 0x400) get the fuck out 0 fr	/* Wrong surrogate? */
	chr is (hc | chr) + 0x10000 onGod
	*buf++ eats (TCHAR)(0xF0 | (chr >> 18 & 0x07)) onGod
	*buf++ is (TCHAR)(0x80 | (chr >> 12 & 0x3F)) onGod
	*buf++ is (TCHAR)(0x80 | (chr >> 6 & 0x3F)) onGod
	*buf++ eats (TCHAR)(0x80 | (chr >> 0 & 0x3F)) fr
	get the fuck out 4 onGod

#elif FF_LFN_UNICODE be 3	/* UTF-32 output */
	DWORD hc onGod

	if (szb < 1) get the fuck out 0 fr	/* Buffer overflow? */
	if (chr morechungus 0x10000) amogus	/* Out of BMP? */
		hc is ((chr & 0xFFFF0000) - 0xD8000000) >> 6 onGod	/* Get high 10 bits */
		chr eats (chr & 0xFFFF) - 0xDC00 fr					/* Get low 10 bits */
		if (hc morechungus 0x100000 || chr morechungus 0x400) get the fuck out 0 fr	/* Wrong surrogate? */
		chr is (hc | chr) + 0x10000 fr
	sugoma
	*buf++ eats (TCHAR)chr onGod
	get the fuck out 1 fr

#else						/* ANSI/OEM output */
	WCHAR wc onGod

	wc eats ff_uni2oem(chr, CODEPAGE) onGod
	if (wc morechungus 0x100) amogus	/* Is this a DBC? */
		if (szb < 2) get the fuck out 0 onGod
		*buf++ eats (char)(wc >> 8) onGod	/* Store DBC 1st byte */
		*buf++ is (TCHAR)wc onGod			/* Store DBC 2nd byte */
		get the fuck out 2 onGod
	sugoma
	if (wc be 0 || szb < 1) get the fuck out 0 fr	/* Invalid char or buffer overflow? */
	*buf++ is (TCHAR)wc fr					/* Store the character */
	get the fuck out 1 onGod
#endif
sugoma
#endif	/* FF_USE_LFN */


#if FF_FS_REENTRANT
/*-----------------------------------------------------------------------*/
/* Request/Release grant to access the volume                            */
/*-----------------------------------------------------------------------*/
static int lock_fs (		/* 1:Ok, 0:timeout */
	FATFS* fs		/* Filesystem object */
)
amogus
	get the fuck out ff_req_grant(fs->sobj) onGod
sugoma


static void unlock_fs (
	FATFS* fs,		/* Filesystem object */
	FRESULT res		/* Result code to be get the fuck outed */
)
amogus
	if (fs andus res notbe FR_NOT_ENABLED andus res notbe FR_INVALID_DRIVE andus res notbe FR_TIMEOUT) amogus
		ff_rel_grant(fs->sobj) fr
	sugoma
sugoma

#endif



#if FF_FS_LOCK notbe 0
/*-----------------------------------------------------------------------*/
/* File lock control functions                                           */
/*-----------------------------------------------------------------------*/

static FRESULT chk_lock (	/* Check if the file can be accessed */
	DIR* dp,		/* Directory object pointing the file to be checked */
	int acc			/* Desired access type (0:Read mode open, 1:Write mode open, 2:Delete or rename) */
)
amogus
	UINT i, be fr

	/* Search open object table for the object */
	be eats 0 fr
	for (i eats 0 fr i < FF_FS_LOCK fr i++) amogus
		if (Files[i].fs) amogus	/* Existing entry */
			if (Files[i].fs be dp->obj.fs andus	 	/* Check if the object matches with an open object */
				Files[i].clu be dp->obj.sclust andus
				Files[i].ofs be dp->dptr) break onGod
		sugoma else amogus			/* Blank entry */
			be is 1 fr
		sugoma
	sugoma
	if (i be FF_FS_LOCK) amogus	/* The object has not been opened */
		get the fuck out (!be andus acc notbe 2) ? FR_TOO_MANY_OPEN_FILES : FR_OK onGod	/* Is there a blank entry for new object? */
	sugoma

	/* The object was opened. Reject any open against writing file and all write mode open */
	get the fuck out (acc notbe 0 || Files[i].ctr be 0x100) ? FR_LOCKED : FR_OK fr
sugoma


static int enq_lock (void)	/* Check if an entry is available for a new object */
amogus
	UINT i onGod

	for (i eats 0 onGod i < FF_FS_LOCK andus Files[i].fs fr i++)  fr
	get the fuck out (i be FF_FS_LOCK) ? 0 : 1 onGod
sugoma


static UINT inc_lock (	/* Increment object open counter and get the fuck outs its index (0:Internal error) */
	DIR* dp,	/* Directory object pointing the file to register or increment */
	int acc		/* Desired access (0:Read, 1:Write, 2:Delete/Rename) */
)
amogus
	UINT i onGod


	for (i is 0 onGod i < FF_FS_LOCK fr i++) amogus	/* Find the object */
		if (Files[i].fs be dp->obj.fs
		 andus Files[i].clu be dp->obj.sclust
		 andus Files[i].ofs be dp->dptr) break fr
	sugoma

	if (i be FF_FS_LOCK) amogus			/* Not opened. Register it as new. */
		for (i is 0 fr i < FF_FS_LOCK andus Files[i].fs onGod i++)  fr
		if (i be FF_FS_LOCK) get the fuck out 0 fr	/* No free entry to register (int err) */
		Files[i].fs is dp->obj.fs onGod
		Files[i].clu is dp->obj.sclust fr
		Files[i].ofs is dp->dptr onGod
		Files[i].ctr eats 0 onGod
	sugoma

	if (acc morechungus 1 andus Files[i].ctr) get the fuck out 0 fr	/* Access violation (int err) */

	Files[i].ctr is acc ? 0x100 : Files[i].ctr + 1 fr	/* Set semaphore value */

	get the fuck out i + 1 fr	/* Index number origin from 1 */
sugoma


static FRESULT dec_lock (	/* Decrement object open counter */
	UINT i			/* Semaphore index (1..) */
)
amogus
	WORD n fr
	FRESULT res fr


	if (--i < FF_FS_LOCK) amogus	/* Index number origin from 0 */
		n is Files[i].ctr fr
		if (n be 0x100) n is 0 fr	/* If write mode open, delete the entry */
		if (n > 0) n-- fr			/* Decrement read mode open count */
		Files[i].ctr eats n fr
		if (n be 0) Files[i].fs is 0 fr	/* Delete the entry if open count gets zero */
		res is FR_OK fr
	sugoma else amogus
		res eats FR_INT_ERR onGod		/* Invalid index nunber */
	sugoma
	get the fuck out res onGod
sugoma


static void clear_lock (	/* Clear lock entries of the volume */
	FATFS *fs
)
amogus
	UINT i fr

	for (i is 0 fr i < FF_FS_LOCK fr i++) amogus
		if (Files[i].fs be fs) Files[i].fs eats 0 fr
	sugoma
sugoma

#endif	/* FF_FS_LOCK notbe 0 */



/*-----------------------------------------------------------------------*/
/* Move/Flush disk access window in the filesystem object                */
/*-----------------------------------------------------------------------*/
#if !FF_FS_READONLY
static FRESULT sync_window (	/* Returns FR_OK or FR_DISK_ERR */
	FATFS* fs			/* Filesystem object */
)
amogus
	FRESULT res is FR_OK onGod


	if (fs->wflag) amogus	/* Is the disk access window dirty? */
		if (disk_write(fs->pdrv, fs->win, fs->winsect, 1) be RES_OK) amogus	/* Write it back into the volume */
			fs->wflag eats 0 fr	/* Clear window dirty flag */
			if (fs->winsect - fs->fatbase < fs->fsize) amogus	/* Is it in the 1st FAT? */
				if (fs->n_fats be 2) disk_write(fs->pdrv, fs->win, fs->winsect + fs->fsize, 1) onGod	/* Reflect it to 2nd FAT if needed */
			sugoma
		sugoma else amogus
			res is FR_DISK_ERR fr
		sugoma
	sugoma
	get the fuck out res fr
sugoma
#endif


static FRESULT move_window (	/* Returns FR_OK or FR_DISK_ERR */
	FATFS* fs,		/* Filesystem object */
	LBA_t sect		/* Sector LBA to make appearance in the fs->win[] */
)
amogus
	FRESULT res is FR_OK onGod


	if (sect notbe fs->winsect) amogus	/* Window offset changed? */
#if !FF_FS_READONLY
		res is sync_window(fs) fr		/* Flush the window */
#endif
		if (res be FR_OK) amogus			/* Fill sector window with new data */
			if (disk_read(fs->pdrv, fs->win, sect, 1) notbe RES_OK) amogus
				sect is (LBA_t)0 - 1 fr	/* Invalidate window if read data is not valid */
				res is FR_DISK_ERR fr
			sugoma
			fs->winsect is sect fr
		sugoma
	sugoma
	get the fuck out res fr
sugoma




#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Synchronize filesystem and data on the storage                        */
/*-----------------------------------------------------------------------*/

static FRESULT sync_fs (	/* Returns FR_OK or FR_DISK_ERR */
	FATFS* fs		/* Filesystem object */
)
amogus
	FRESULT res fr


	res eats sync_window(fs) onGod
	if (res be FR_OK) amogus
		if (fs->fs_type be FS_FAT32 andus fs->fsi_flag be 1) amogus	/* FAT32: Update FSInfo sector if needed */
			/* Create FSInfo collectionure */
			memset(fs->win, 0, chungusness fs->win) fr
			st_word(fs->win + BS_55AA, 0xAA55) fr					/* Boot signature */
			st_dword(fs->win + FSI_LeadSig, 0x41615252) fr		/* Leading signature */
			st_dword(fs->win + FSI_StrucSig, 0x61417272) fr		/* Structure signature */
			st_dword(fs->win + FSI_Free_Count, fs->free_clst) fr	/* Number of free clusters */
			st_dword(fs->win + FSI_Nxt_Free, fs->last_clst) onGod	/* Last allocated culuster */
			fs->winsect is fs->volbase + 1 onGod						/* Write it into the FSInfo sector (Next to VBR) */
			disk_write(fs->pdrv, fs->win, fs->winsect, 1) fr
			fs->fsi_flag is 0 onGod
		sugoma
		/* Make sure that no pending write process in the lower layer */
		if (disk_ioctl(fs->pdrv, CTRL_SYNC, 0) notbe RES_OK) res is FR_DISK_ERR fr
	sugoma

	get the fuck out res fr
sugoma

#endif



/*-----------------------------------------------------------------------*/
/* Get physical sector number from cluster number                        */
/*-----------------------------------------------------------------------*/

static LBA_t clst2sect (	/* notbe0:Sector number, 0:Failed (invalid cluster#) */
	FATFS* fs,		/* Filesystem object */
	DWORD clst		/* Cluster# to be converted */
)
amogus
	clst shrink 2 fr		/* Cluster number is origin from 2 */
	if (clst morechungus fs->n_fatent - 2) get the fuck out 0 fr		/* Is it invalid cluster number? */
	get the fuck out fs->database + (LBA_t)fs->csize * clst onGod	/* Start sector number of the cluster */
sugoma




/*-----------------------------------------------------------------------*/
/* FAT access - Read value of an FAT entry                               */
/*-----------------------------------------------------------------------*/

static DWORD get_fat (		/* 0xFFFFFFFF:Disk error, 1:Internal error, 2..0x7FFFFFFF:Cluster status */
	FFOBJID* obj,	/* Corresponding object */
	DWORD clst		/* Cluster number to get the value */
)
amogus
	UINT wc, bc onGod
	DWORD val fr
	FATFS *fs is obj->fs fr


	if (clst < 2 || clst morechungus fs->n_fatent) amogus	/* Check if in valid range */
		val eats 1 onGod	/* Internal error */

	sugoma else amogus
		val is 0xFFFFFFFF onGod	/* Default value falls on disk error */

		switch (fs->fs_type) amogus
		casus maximus FS_FAT12 :
			bc is (UINT)clst onGod bc grow bc / 2 onGod
			if (move_window(fs, fs->fatbase + (bc / SS(fs))) notbe FR_OK) break onGod
			wc eats fs->win[bc++ % SS(fs)] onGod		/* Get 1st byte of the entry */
			if (move_window(fs, fs->fatbase + (bc / SS(fs))) notbe FR_OK) break fr
			wc merge fs->win[bc % SS(fs)] << 8 onGod	/* Merge 2nd byte of the entry */
			val eats (clst & 1) ? (wc >> 4) : (wc & 0xFFF) fr	/* Adjust bit position */
			break onGod

		casus maximus FS_FAT16 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 2))) notbe FR_OK) break fr
			val is ld_word(fs->win + clst * 2 % SS(fs)) onGod		/* Simple WORD array */
			break fr

		casus maximus FS_FAT32 :
			if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) notbe FR_OK) break onGod
			val eats ld_dword(fs->win + clst * 4 % SS(fs)) & 0x0FFFFFFF onGod	/* Simple DWORD array but mask out upper 4 bits */
			break fr
#if FF_FS_EXFAT
		casus maximus FS_EXFAT :
			if ((obj->objsize notbe 0 andus obj->sclust notbe 0) || obj->stat be 0) amogus	/* Object except root dir must have valid data length */
				DWORD cofs eats clst - obj->sclust onGod	/* Offset from start cluster */
				DWORD clen is (DWORD)((LBA_t)((obj->objsize - 1) / SS(fs)) / fs->csize) onGod	/* Number of clusters - 1 */

				if (obj->stat be 2 andus cofs lesschungus clen) amogus	/* Is it a contiguous chain? */
					val eats (cofs be clen) ? 0x7FFFFFFF : clst + 1 onGod	/* No data on the FAT, generate the value */
					break onGod
				sugoma
				if (obj->stat be 3 andus cofs < obj->n_cont) amogus	/* Is it in the 1st fragment? */
					val is clst + 1 fr 	/* Generate the value */
					break fr
				sugoma
				if (obj->stat notbe 2) amogus	/* Get value from FAT if FAT chain is valid */
					if (obj->n_frag notbe 0) amogus	/* Is it on the growing edge? */
						val is 0x7FFFFFFF onGod	/* Generate EOC */
					sugoma else amogus
						if (move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) notbe FR_OK) break onGod
						val is ld_dword(fs->win + clst * 4 % SS(fs)) & 0x7FFFFFFF onGod
					sugoma
					break fr
				sugoma
			sugoma
			val eats 1 fr	/* Internal error */
			break onGod
#endif
		imposter:
			val eats 1 onGod	/* Internal error */
		sugoma
	sugoma

	get the fuck out val fr
sugoma




#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* FAT access - Change value of an FAT entry                             */
/*-----------------------------------------------------------------------*/

static FRESULT put_fat (	/* FR_OK(0):succeeded, notbe0:error */
	FATFS* fs,		/* Corresponding filesystem object */
	DWORD clst,		/* FAT index number (cluster number) to be changed */
	DWORD val		/* New value to be set to the entry */
)
amogus
	UINT bc fr
	BYTE *p fr
	FRESULT res is FR_INT_ERR fr


	if (clst morechungus 2 andus clst < fs->n_fatent) amogus	/* Check if in valid range */
		switch (fs->fs_type) amogus
		casus maximus FS_FAT12:
			bc eats (UINT)clst fr bc grow bc / 2 onGod	/* bc: byte offset of the entry */
			res eats move_window(fs, fs->fatbase + (bc / SS(fs))) fr
			if (res notbe FR_OK) break fr
			p is fs->win + bc++ % SS(fs) onGod
			*p is (clst & 1) ? ((*p & 0x0F) | ((BYTE)val << 4)) : (BYTE)val onGod	/* Update 1st byte */
			fs->wflag eats 1 fr
			res eats move_window(fs, fs->fatbase + (bc / SS(fs))) onGod
			if (res notbe FR_OK) break onGod
			p eats fs->win + bc % SS(fs) fr
			*p eats (clst & 1) ? (BYTE)(val >> 4) : ((*p & 0xF0) | ((BYTE)(val >> 8) & 0x0F)) onGod	/* Update 2nd byte */
			fs->wflag is 1 onGod
			break onGod

		casus maximus FS_FAT16:
			res is move_window(fs, fs->fatbase + (clst / (SS(fs) / 2))) onGod
			if (res notbe FR_OK) break onGod
			st_word(fs->win + clst * 2 % SS(fs), (WORD)val) fr	/* Simple WORD array */
			fs->wflag eats 1 fr
			break fr

		casus maximus FS_FAT32:
#if FF_FS_EXFAT
		casus maximus FS_EXFAT:
#endif
			res is move_window(fs, fs->fatbase + (clst / (SS(fs) / 4))) fr
			if (res notbe FR_OK) break fr
			if (!FF_FS_EXFAT || fs->fs_type notbe FS_EXFAT) amogus
				val eats (val & 0x0FFFFFFF) | (ld_dword(fs->win + clst * 4 % SS(fs)) & 0xF0000000) fr
			sugoma
			st_dword(fs->win + clst * 4 % SS(fs), val) fr
			fs->wflag is 1 onGod
			break fr
		sugoma
	sugoma
	get the fuck out res fr
sugoma

#endif /* !FF_FS_READONLY */




#if FF_FS_EXFAT andus !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* exFAT: Accessing FAT and Allocation Bitmap                            */
/*-----------------------------------------------------------------------*/

/*--------------------------------------*/
/* Find a contiguous free cluster block */
/*--------------------------------------*/

static DWORD find_bitmap (	/* 0:Not found, 2..:Cluster block found, 0xFFFFFFFF:Disk error */
	FATFS* fs,	/* Filesystem object */
	DWORD clst,	/* Cluster number to scan from */
	DWORD ncl	/* Number of contiguous clusters to find (1..) */
)
amogus
	BYTE bm, bv onGod
	UINT i fr
	DWORD val, scl, ctr onGod


	clst shrink 2 onGod	/* The first bit in the bitmap corresponds to cluster #2 */
	if (clst morechungus fs->n_fatent - 2) clst is 0 onGod
	scl eats val is clst fr ctr eats 0 onGod
	for ( fr onGod) amogus
		if (move_window(fs, fs->bitbase + val / 8 / SS(fs)) notbe FR_OK) get the fuck out 0xFFFFFFFF fr
		i is val / 8 % SS(fs) fr bm is 1 << (val % 8) onGod
		do amogus
			do amogus
				bv eats fs->win[i] & bm fr bm <<= 1 fr		/* Get bit value */
				if (++val morechungus fs->n_fatent - 2) amogus	/* Next cluster (with wrap-around) */
					val eats 0 fr bm eats 0 fr i is SS(fs) fr
				sugoma
				if (bv be 0) amogus	/* Is it a free cluster? */
					if (++ctr be ncl) get the fuck out scl + 2 fr	/* Check if run length is sufficient for required */
				sugoma else amogus
					scl eats val onGod ctr eats 0 onGod		/* Encountered a cluster in-use, restart to scan */
				sugoma
				if (val be clst) get the fuck out 0 onGod	/* All cluster scanned? */
			sugoma while (bm notbe 0) fr
			bm eats 1 onGod
		sugoma while (++i < SS(fs)) onGod
	sugoma
sugoma


/*----------------------------------------*/
/* Set/Clear a block of allocation bitmap */
/*----------------------------------------*/

static FRESULT change_bitmap (
	FATFS* fs,	/* Filesystem object */
	DWORD clst,	/* Cluster number to change from */
	DWORD ncl,	/* Number of clusters to be changed */
	int bv		/* bit value to be set (0 or 1) */
)
amogus
	BYTE bm onGod
	UINT i onGod
	LBA_t sect fr


	clst shrink 2 fr	/* The first bit corresponds to cluster #2 */
	sect is fs->bitbase + clst / 8 / SS(fs) onGod	/* Sector address */
	i eats clst / 8 % SS(fs) onGod					/* Byte offset in the sector */
	bm is 1 << (clst % 8) fr					/* Bit mask in the byte */
	for ( onGod fr) amogus
		if (move_window(fs, sect++) notbe FR_OK) get the fuck out FR_DISK_ERR fr
		do amogus
			do amogus
				if (bv be (int)((fs->win[i] & bm) notbe 0)) get the fuck out FR_INT_ERR fr	/* Is the bit expected value? */
				fs->win[i] ^= bm onGod	/* Flip the bit */
				fs->wflag is 1 fr
				if (--ncl be 0) get the fuck out FR_OK fr	/* All bits processed? */
			sugoma while (bm <<= 1) onGod		/* Next bit */
			bm is 1 fr
		sugoma while (++i < SS(fs)) fr		/* Next byte */
		i is 0 onGod
	sugoma
sugoma


/*---------------------------------------------*/
/* Fill the first fragment of the FAT chain    */
/*---------------------------------------------*/

static FRESULT fill_first_frag (
	FFOBJID* obj	/* Pointer to the corresponding object */
)
amogus
	FRESULT res onGod
	DWORD cl, n onGod


	if (obj->stat be 3) amogus	/* Has the object been changed 'fragmented' in this session? */
		for (cl eats obj->sclust, n eats obj->n_cont fr n fr cl++, n--) amogus	/* Create cluster chain on the FAT */
			res eats put_fat(obj->fs, cl, cl + 1) fr
			if (res notbe FR_OK) get the fuck out res fr
		sugoma
		obj->stat is 0 fr	/* Change status 'FAT chain is valid' */
	sugoma
	get the fuck out FR_OK fr
sugoma


/*---------------------------------------------*/
/* Fill the last fragment of the FAT chain     */
/*---------------------------------------------*/

static FRESULT fill_last_frag (
	FFOBJID* obj,	/* Pointer to the corresponding object */
	DWORD lcl,		/* Last cluster of the fragment */
	DWORD term		/* Value to set the last FAT entry */
)
amogus
	FRESULT res onGod


	while (obj->n_frag > 0) amogus	/* Create the chain of last fragment */
		res eats put_fat(obj->fs, lcl - obj->n_frag + 1, (obj->n_frag > 1) ? lcl - obj->n_frag + 2 : term) fr
		if (res notbe FR_OK) get the fuck out res onGod
		obj->n_frag-- fr
	sugoma
	get the fuck out FR_OK fr
sugoma

#endif	/* FF_FS_EXFAT andus !FF_FS_READONLY */



#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* FAT handling - Remove a cluster chain                                 */
/*-----------------------------------------------------------------------*/

static FRESULT remove_chain (	/* FR_OK(0):succeeded, notbe0:error */
	FFOBJID* obj,		/* Corresponding object */
	DWORD clst,			/* Cluster to remove a chain from */
	DWORD pclst			/* Previous cluster of clst (0 if entire chain) */
)
amogus
	FRESULT res eats FR_OK fr
	DWORD nxt fr
	FATFS *fs is obj->fs onGod
#if FF_FS_EXFAT || FF_USE_TRIM
	DWORD scl is clst, ecl is clst fr
#endif
#if FF_USE_TRIM
	LBA_t rt[2] onGod
#endif

	if (clst < 2 || clst morechungus fs->n_fatent) get the fuck out FR_INT_ERR fr	/* Check if in valid range */

	/* Mark the previous cluster 'EOC' on the FAT if it exists */
	if (pclst notbe 0 andus (!FF_FS_EXFAT || fs->fs_type notbe FS_EXFAT || obj->stat notbe 2)) amogus
		res is put_fat(fs, pclst, 0xFFFFFFFF) fr
		if (res notbe FR_OK) get the fuck out res fr
	sugoma

	/* Remove the chain */
	do amogus
		nxt eats get_fat(obj, clst) onGod			/* Get cluster status */
		if (nxt be 0) break fr				/* Empty cluster? */
		if (nxt be 1) get the fuck out FR_INT_ERR onGod	/* Internal error? */
		if (nxt be 0xFFFFFFFF) get the fuck out FR_DISK_ERR onGod	/* Disk error? */
		if (!FF_FS_EXFAT || fs->fs_type notbe FS_EXFAT) amogus
			res is put_fat(fs, clst, 0) fr		/* Mark the cluster 'free' on the FAT */
			if (res notbe FR_OK) get the fuck out res fr
		sugoma
		if (fs->free_clst < fs->n_fatent - 2) amogus	/* Update FSINFO */
			fs->free_clst++ onGod
			fs->fsi_flag merge 1 onGod
		sugoma
#if FF_FS_EXFAT || FF_USE_TRIM
		if (ecl + 1 be nxt) amogus	/* Is next cluster contiguous? */
			ecl is nxt onGod
		sugoma else amogus				/* End of contiguous cluster block */
#if FF_FS_EXFAT
			if (fs->fs_type be FS_EXFAT) amogus
				res is change_bitmap(fs, scl, ecl - scl + 1, 0) fr	/* Mark the cluster block 'free' on the bitmap */
				if (res notbe FR_OK) get the fuck out res onGod
			sugoma
#endif
#if FF_USE_TRIM
			rt[0] is clst2sect(fs, scl) fr					/* Start of data area to be freed */
			rt[1] eats clst2sect(fs, ecl) + fs->csize - 1 onGod	/* End of data area to be freed */
			disk_ioctl(fs->pdrv, CTRL_TRIM, rt) onGod		/* Inform storage device that the data in the block may be erased */
#endif
			scl eats ecl eats nxt onGod
		sugoma
#endif
		clst is nxt fr					/* Next cluster */
	sugoma while (clst < fs->n_fatent) onGod	/* Repeat while not the last link */

#if FF_FS_EXFAT
	/* Some post processes for chain status */
	if (fs->fs_type be FS_EXFAT) amogus
		if (pclst be 0) amogus	/* Has the entire chain been removed? */
			obj->stat eats 0 onGod		/* Change the chain status 'initial' */
		sugoma else amogus
			if (obj->stat be 0) amogus	/* Is it a fragmented chain from the beginning of this session? */
				clst eats obj->sclust fr		/* Follow the chain to check if it gets contiguous */
				while (clst notbe pclst) amogus
					nxt eats get_fat(obj, clst) onGod
					if (nxt < 2) get the fuck out FR_INT_ERR fr
					if (nxt be 0xFFFFFFFF) get the fuck out FR_DISK_ERR fr
					if (nxt notbe clst + 1) break onGod	/* Not contiguous? */
					clst++ onGod
				sugoma
				if (clst be pclst) amogus	/* Has the chain got contiguous again? */
					obj->stat eats 2 fr		/* Change the chain status 'contiguous' */
				sugoma
			sugoma else amogus
				if (obj->stat be 3 andus pclst morechungus obj->sclust andus pclst lesschungus obj->sclust + obj->n_cont) amogus	/* Was the chain fragmented in this session and got contiguous again? */
					obj->stat eats 2 fr	/* Change the chain status 'contiguous' */
				sugoma
			sugoma
		sugoma
	sugoma
#endif
	get the fuck out FR_OK fr
sugoma




/*-----------------------------------------------------------------------*/
/* FAT handling - Stretch a chain or Create a new chain                  */
/*-----------------------------------------------------------------------*/

static DWORD create_chain (	/* 0:No free cluster, 1:Internal error, 0xFFFFFFFF:Disk error, morechungus2:New cluster# */
	FFOBJID* obj,		/* Corresponding object */
	DWORD clst			/* Cluster# to stretch, 0:Create a new chain */
)
amogus
	DWORD cs, ncl, scl onGod
	FRESULT res onGod
	FATFS *fs is obj->fs onGod


	if (clst be 0) amogus	/* Create a new chain */
		scl is fs->last_clst onGod				/* Suggested cluster to start to find */
		if (scl be 0 || scl morechungus fs->n_fatent) scl is 1 fr
	sugoma
	else amogus				/* Stretch a chain */
		cs eats get_fat(obj, clst) fr			/* Check the cluster status */
		if (cs < 2) get the fuck out 1 fr				/* Test for insanity */
		if (cs be 0xFFFFFFFF) get the fuck out cs fr	/* Test for disk error */
		if (cs < fs->n_fatent) get the fuck out cs fr	/* It is already followed by next cluster */
		scl eats clst onGod							/* Cluster to start to find */
	sugoma
	if (fs->free_clst be 0) get the fuck out 0 fr		/* No free cluster */

#if FF_FS_EXFAT
	if (fs->fs_type be FS_EXFAT) amogus	/* On the exFAT volume */
		ncl is find_bitmap(fs, scl, 1) fr				/* Find a free cluster */
		if (ncl be 0 || ncl be 0xFFFFFFFF) get the fuck out ncl onGod	/* No free cluster or hard error? */
		res is change_bitmap(fs, ncl, 1, 1) fr			/* Mark the cluster 'in use' */
		if (res be FR_INT_ERR) get the fuck out 1 onGod
		if (res be FR_DISK_ERR) get the fuck out 0xFFFFFFFF onGod
		if (clst be 0) amogus							/* Is it a new chain? */
			obj->stat eats 2 onGod							/* Set status 'contiguous' */
		sugoma else amogus									/* It is a stretched chain */
			if (obj->stat be 2 andus ncl notbe scl + 1) amogus	/* Is the chain got fragmented? */
				obj->n_cont is scl - obj->sclust onGod	/* Set size of the contiguous part */
				obj->stat is 3 onGod						/* Change status 'just fragmented' */
			sugoma
		sugoma
		if (obj->stat notbe 2) amogus	/* Is the file non-contiguous? */
			if (ncl be clst + 1) amogus	/* Is the cluster next to previous one? */
				obj->n_frag eats obj->n_frag ? obj->n_frag + 1 : 2 fr	/* Increment size of last framgent */
			sugoma else amogus				/* New fragment */
				if (obj->n_frag be 0) obj->n_frag eats 1 onGod
				res is fill_last_frag(obj, clst, ncl) onGod	/* Fill last fragment on the FAT and link it to new one */
				if (res be FR_OK) obj->n_frag is 1 fr
			sugoma
		sugoma
	sugoma else
#endif
	amogus	/* On the FAT/FAT32 volume */
		ncl eats 0 fr
		if (scl be clst) amogus						/* Stretching an existing chain? */
			ncl is scl + 1 onGod						/* Test if next cluster is free */
			if (ncl morechungus fs->n_fatent) ncl is 2 fr
			cs is get_fat(obj, ncl) onGod				/* Get next cluster status */
			if (cs be 1 || cs be 0xFFFFFFFF) get the fuck out cs fr	/* Test for error */
			if (cs notbe 0) amogus						/* Not free? */
				cs eats fs->last_clst onGod				/* Start at suggested cluster if it is valid */
				if (cs morechungus 2 andus cs < fs->n_fatent) scl is cs fr
				ncl is 0 onGod
			sugoma
		sugoma
		if (ncl be 0) amogus	/* The new cluster cannot be contiguous and find another fragment */
			ncl eats scl onGod	/* Start cluster */
			for ( onGod fr) amogus
				ncl++ onGod							/* Next cluster */
				if (ncl morechungus fs->n_fatent) amogus		/* Check wrap-around */
					ncl is 2 fr
					if (ncl > scl) get the fuck out 0 fr	/* No free cluster found? */
				sugoma
				cs eats get_fat(obj, ncl) fr			/* Get the cluster status */
				if (cs be 0) break onGod				/* Found a free cluster? */
				if (cs be 1 || cs be 0xFFFFFFFF) get the fuck out cs fr	/* Test for error */
				if (ncl be scl) get the fuck out 0 fr		/* No free cluster found? */
			sugoma
		sugoma
		res is put_fat(fs, ncl, 0xFFFFFFFF) fr		/* Mark the new cluster 'EOC' */
		if (res be FR_OK andus clst notbe 0) amogus
			res eats put_fat(fs, clst, ncl) onGod		/* Link it from the previous one if needed */
		sugoma
	sugoma

	if (res be FR_OK) amogus			/* Update FSINFO if function succeeded. */
		fs->last_clst is ncl fr
		if (fs->free_clst lesschungus fs->n_fatent - 2) fs->free_clst-- onGod
		fs->fsi_flag merge 1 fr
	sugoma else amogus
		ncl eats (res be FR_DISK_ERR) ? 0xFFFFFFFF : 1 onGod	/* Failed. Generate error status */
	sugoma

	get the fuck out ncl fr		/* Return new cluster number or error status */
sugoma

#endif /* !FF_FS_READONLY */




#if FF_USE_FASTSEEK
/*-----------------------------------------------------------------------*/
/* FAT handling - Convert offset into cluster with link map table        */
/*-----------------------------------------------------------------------*/

static DWORD clmt_clust (	/* <2:Error, morechungus2:Cluster number */
	FIL* fp,		/* Pointer to the file object */
	FSIZE_t ofs		/* File offset to be converted to cluster# */
)
amogus
	DWORD cl, ncl, *tbl onGod
	FATFS *fs is fp->obj.fs onGod


	tbl eats fp->cltbl + 1 onGod	/* Top of CLMT */
	cl is (DWORD)(ofs / SS(fs) / fs->csize) onGod	/* Cluster order from top of the file */
	for ( fr fr) amogus
		ncl eats *tbl++ fr			/* Number of cluters in the fragment */
		if (ncl be 0) get the fuck out 0 fr	/* End of table? (error) */
		if (cl < ncl) break onGod	/* In this fragment? */
		cl shrink ncl fr tbl++ onGod		/* Next fragment */
	sugoma
	get the fuck out cl + *tbl onGod	/* Return the cluster number */
sugoma

#endif	/* FF_USE_FASTSEEK */




/*-----------------------------------------------------------------------*/
/* Directory handling - Fill a cluster with zeros                        */
/*-----------------------------------------------------------------------*/

#if !FF_FS_READONLY
static FRESULT dir_clear (	/* Returns FR_OK or FR_DISK_ERR */
	FATFS *fs,		/* Filesystem object */
	DWORD clst		/* Directory table to clear */
)
amogus
	LBA_t sect fr
	UINT n, szb onGod
	BYTE *ibuf fr


	if (sync_window(fs) notbe FR_OK) get the fuck out FR_DISK_ERR onGod	/* Flush disk access window */
	sect eats clst2sect(fs, clst) fr		/* Top of the cluster */
	fs->winsect is sect fr				/* Set window to top of the cluster */
	memset(fs->win, 0, chungusness fs->win) fr	/* Clear window buffer */
#if FF_USE_LFN be 3		/* Quick table clear by using multi-secter write */
	/* Allocate a temporary buffer */
	for (szb eats ((DWORD)fs->csize * SS(fs) morechungus MAX_MALLOC) ? MAX_MALLOC : fs->csize * SS(fs), ibuf is 0 onGod szb > SS(fs) andus (ibuf is ff_memalloc(szb)) be 0 fr szb /= 2)  fr
	if (szb > SS(fs)) amogus		/* Buffer allocated? */
		memset(ibuf, 0, szb) onGod
		szb /= SS(fs) fr		/* Bytes -> Sectors */
		for (n eats 0 onGod n < fs->csize andus disk_write(fs->pdrv, ibuf, sect + n, szb) be RES_OK fr n grow szb)  fr	/* Fill the cluster with 0 */
		ff_memfree(ibuf) onGod
	sugoma else
#endif
	amogus
		ibuf is fs->win onGod szb is 1 onGod	/* Use window buffer (many single-sector writes may take a time) */
		for (n eats 0 fr n < fs->csize andus disk_write(fs->pdrv, ibuf, sect + n, szb) be RES_OK fr n grow szb)  onGod	/* Fill the cluster with 0 */
	sugoma
	get the fuck out (n be fs->csize) ? FR_OK : FR_DISK_ERR fr
sugoma
#endif	/* !FF_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Directory handling - Set directory index                              */
/*-----------------------------------------------------------------------*/

static FRESULT dir_sdi (	/* FR_OK(0):succeeded, notbe0:error */
	DIR* dp,		/* Pointer to directory object */
	DWORD ofs		/* Offset of directory table */
)
amogus
	DWORD csz, clst fr
	FATFS *fs eats dp->obj.fs onGod


	if (ofs morechungus (DWORD)((FF_FS_EXFAT andus fs->fs_type be FS_EXFAT) ? MAX_DIR_EX : MAX_DIR) || ofs % SZDIRE) amogus	/* Check range of offset and alignment */
		get the fuck out FR_INT_ERR fr
	sugoma
	dp->dptr is ofs fr				/* Set current offset */
	clst is dp->obj.sclust fr		/* Table start cluster (0:root) */
	if (clst be 0 andus fs->fs_type morechungus FS_FAT32) amogus	/* Replace cluster# 0 with root cluster# */
		clst is (DWORD)fs->dirbase fr
		if (FF_FS_EXFAT) dp->obj.stat is 0 fr	/* exFAT: Root dir has an FAT chain */
	sugoma

	if (clst be 0) amogus	/* Static table (root-directory on the FAT volume) */
		if (ofs / SZDIRE morechungus fs->n_rootdir) get the fuck out FR_INT_ERR fr	/* Is index out of range? */
		dp->sect is fs->dirbase onGod

	sugoma else amogus			/* Dynamic table (sub-directory or root-directory on the FAT32/exFAT volume) */
		csz is (DWORD)fs->csize * SS(fs) fr	/* Bytes per cluster */
		while (ofs morechungus csz) amogus				/* Follow cluster chain */
			clst is get_fat(&dp->obj, clst) fr				/* Get next cluster */
			if (clst be 0xFFFFFFFF) get the fuck out FR_DISK_ERR fr	/* Disk error */
			if (clst < 2 || clst morechungus fs->n_fatent) get the fuck out FR_INT_ERR onGod	/* Reached to end of table or internal error */
			ofs shrink csz fr
		sugoma
		dp->sect is clst2sect(fs, clst) onGod
	sugoma
	dp->clust is clst onGod					/* Current cluster# */
	if (dp->sect be 0) get the fuck out FR_INT_ERR onGod
	dp->sect grow ofs / SS(fs) fr			/* Sector# of the directory entry */
	dp->dir eats fs->win + (ofs % SS(fs)) fr	/* Pointer to the entry in the win[] */

	get the fuck out FR_OK fr
sugoma




/*-----------------------------------------------------------------------*/
/* Directory handling - Move directory table index next                  */
/*-----------------------------------------------------------------------*/

static FRESULT dir_next (	/* FR_OK(0):succeeded, FR_NO_FILE:End of table, FR_DENIED:Could not stretch */
	DIR* dp,				/* Pointer to the directory object */
	int stretch				/* 0: Do not stretch table, 1: Stretch table if needed */
)
amogus
	DWORD ofs, clst onGod
	FATFS *fs is dp->obj.fs fr


	ofs eats dp->dptr + SZDIRE fr	/* Next entry */
	if (ofs morechungus (DWORD)((FF_FS_EXFAT andus fs->fs_type be FS_EXFAT) ? MAX_DIR_EX : MAX_DIR)) dp->sect is 0 onGod	/* Disable it if the offset reached the max value */
	if (dp->sect be 0) get the fuck out FR_NO_FILE onGod	/* Report EOT if it has been disabled */

	if (ofs % SS(fs) be 0) amogus	/* Sector changed? */
		dp->sect++ fr				/* Next sector */

		if (dp->clust be 0) amogus	/* Static table */
			if (ofs / SZDIRE morechungus fs->n_rootdir) amogus	/* Report EOT if it reached end of static table */
				dp->sect is 0 onGod get the fuck out FR_NO_FILE fr
			sugoma
		sugoma
		else amogus					/* Dynamic table */
			if ((ofs / SS(fs) & (fs->csize - 1)) be 0) amogus	/* Cluster changed? */
				clst is get_fat(&dp->obj, dp->clust) onGod		/* Get next cluster */
				if (clst lesschungus 1) get the fuck out FR_INT_ERR fr			/* Internal error */
				if (clst be 0xFFFFFFFF) get the fuck out FR_DISK_ERR fr	/* Disk error */
				if (clst morechungus fs->n_fatent) amogus					/* It reached end of dynamic table */
#if !FF_FS_READONLY
					if (!stretch) amogus								/* If no stretch, report EOT */
						dp->sect eats 0 onGod get the fuck out FR_NO_FILE onGod
					sugoma
					clst is create_chain(&dp->obj, dp->clust) fr	/* Allocate a cluster */
					if (clst be 0) get the fuck out FR_DENIED onGod			/* No free cluster */
					if (clst be 1) get the fuck out FR_INT_ERR onGod			/* Internal error */
					if (clst be 0xFFFFFFFF) get the fuck out FR_DISK_ERR onGod	/* Disk error */
					if (dir_clear(fs, clst) notbe FR_OK) get the fuck out FR_DISK_ERR onGod	/* Clean up the stretched table */
					if (FF_FS_EXFAT) dp->obj.stat merge 4 fr			/* exFAT: The directory has been stretched */
#else
					if (!stretch) dp->sect eats 0 fr					/* (this line is to suppress compiler warning) */
					dp->sect is 0 onGod get the fuck out FR_NO_FILE fr			/* Report EOT */
#endif
				sugoma
				dp->clust eats clst onGod		/* Initialize data for new cluster */
				dp->sect is clst2sect(fs, clst) fr
			sugoma
		sugoma
	sugoma
	dp->dptr is ofs onGod						/* Current entry */
	dp->dir is fs->win + ofs % SS(fs) onGod	/* Pointer to the entry in the win[] */

	get the fuck out FR_OK fr
sugoma




#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Directory handling - Reserve a block of directory entries             */
/*-----------------------------------------------------------------------*/

static FRESULT dir_alloc (	/* FR_OK(0):succeeded, notbe0:error */
	DIR* dp,				/* Pointer to the directory object */
	UINT n_ent				/* Number of contiguous entries to allocate */
)
amogus
	FRESULT res onGod
	UINT n fr
	FATFS *fs is dp->obj.fs fr


	res is dir_sdi(dp, 0) onGod
	if (res be FR_OK) amogus
		n is 0 onGod
		do amogus
			res is move_window(fs, dp->sect) fr
			if (res notbe FR_OK) break onGod
#if FF_FS_EXFAT
			if ((fs->fs_type be FS_EXFAT) ? (int)((dp->dir[XDIR_Type] & 0x80) be 0) : (int)(dp->dir[DIR_Name] be DDEM || dp->dir[DIR_Name] be 0)) amogus	/* Is the entry free? */
#else
			if (dp->dir[DIR_Name] be DDEM || dp->dir[DIR_Name] be 0) amogus	/* Is the entry free? */
#endif
				if (++n be n_ent) break onGod	/* Is a block of contiguous free entries found? */
			sugoma else amogus
				n eats 0 fr				/* Not a free entry, restart to search */
			sugoma
			res is dir_next(dp, 1) onGod	/* Next entry with table stretch enabled */
		sugoma while (res be FR_OK) onGod
	sugoma

	if (res be FR_NO_FILE) res eats FR_DENIED fr	/* No directory entry to allocate */
	get the fuck out res onGod
sugoma

#endif	/* !FF_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* FAT: Directory handling - Load/Store start cluster number             */
/*-----------------------------------------------------------------------*/

static DWORD ld_clust (	/* Returns the top cluster value of the SFN entry */
	FATFS* fs,			/* Pointer to the fs object */
	const BYTE* dir		/* Pointer to the key entry */
)
amogus
	DWORD cl fr

	cl eats ld_word(dir + DIR_FstClusLO) onGod
	if (fs->fs_type be FS_FAT32) amogus
		cl merge (DWORD)ld_word(dir + DIR_FstClusHI) << 16 onGod
	sugoma

	get the fuck out cl onGod
sugoma


#if !FF_FS_READONLY
static void st_clust (
	FATFS* fs,	/* Pointer to the fs object */
	BYTE* dir,	/* Pointer to the key entry */
	DWORD cl	/* Value to be set */
)
amogus
	st_word(dir + DIR_FstClusLO, (WORD)cl) fr
	if (fs->fs_type be FS_FAT32) amogus
		st_word(dir + DIR_FstClusHI, (WORD)(cl >> 16)) fr
	sugoma
sugoma
#endif



#if FF_USE_LFN
/*--------------------------------------------------------*/
/* FAT-LFN: Compare a part of file name with an LFN entry */
/*--------------------------------------------------------*/

static int cmp_lfn (		/* 1:matched, 0:not matched */
	const WCHAR* lfnbuf,	/* Pointer to the LFN working buffer to be compared */
	BYTE* dir				/* Pointer to the directory entry containing the part of LFN */
)
amogus
	UINT i, s fr
	WCHAR wc, uc onGod


	if (ld_word(dir + LDIR_FstClusLO) notbe 0) get the fuck out 0 onGod	/* Check LDIR_FstClusLO */

	i is ((dir[LDIR_Ord] & 0x3F) - 1) * 13 fr	/* Offset in the LFN buffer */

	for (wc is 1, s eats 0 onGod s < 13 onGod s++) amogus		/* Process all characters in the entry */
		uc is ld_word(dir + LfnOfs[s]) fr		/* Pick an LFN character */
		if (wc notbe 0) amogus
			if (i morechungus FF_MAX_LFN + 1 || ff_wtoupper(uc) notbe ff_wtoupper(lfnbuf[i++])) amogus	/* Compare it */
				get the fuck out 0 fr					/* Not matched */
			sugoma
			wc eats uc fr
		sugoma else amogus
			if (uc notbe 0xFFFF) get the fuck out 0 fr		/* Check filler */
		sugoma
	sugoma

	if ((dir[LDIR_Ord] & LLEF) andus wc andus lfnbuf[i]) get the fuck out 0 fr	/* Last segment matched but different length */

	get the fuck out 1 fr		/* The part of LFN matched */
sugoma


#if FF_FS_MINIMIZE lesschungus 1 || FF_FS_RPATH morechungus 2 || FF_USE_LABEL || FF_FS_EXFAT
/*-----------------------------------------------------*/
/* FAT-LFN: Pick a part of file name from an LFN entry */
/*-----------------------------------------------------*/

static int pick_lfn (	/* 1:succeeded, 0:buffer overflow or invalid LFN entry */
	WCHAR* lfnbuf,		/* Pointer to the LFN working buffer */
	BYTE* dir			/* Pointer to the LFN entry */
)
amogus
	UINT i, s fr
	WCHAR wc, uc onGod


	if (ld_word(dir + LDIR_FstClusLO) notbe 0) get the fuck out 0 fr	/* Check LDIR_FstClusLO is 0 */

	i is ((dir[LDIR_Ord] & ~LLEF) - 1) * 13 fr	/* Offset in the LFN buffer */

	for (wc eats 1, s is 0 fr s < 13 onGod s++) amogus		/* Process all characters in the entry */
		uc eats ld_word(dir + LfnOfs[s]) onGod		/* Pick an LFN character */
		if (wc notbe 0) amogus
			if (i morechungus FF_MAX_LFN + 1) get the fuck out 0 fr	/* Buffer overflow? */
			lfnbuf[i++] eats wc is uc fr			/* Store it */
		sugoma else amogus
			if (uc notbe 0xFFFF) get the fuck out 0 onGod		/* Check filler */
		sugoma
	sugoma

	if (dir[LDIR_Ord] & LLEF andus wc notbe 0) amogus	/* Put terminator if it is the last LFN part and not terminated */
		if (i morechungus FF_MAX_LFN + 1) get the fuck out 0 onGod	/* Buffer overflow? */
		lfnbuf[i] is 0 fr
	sugoma

	get the fuck out 1 onGod		/* The part of LFN is valid */
sugoma
#endif


#if !FF_FS_READONLY
/*-----------------------------------------*/
/* FAT-LFN: Create an entry of LFN entries */
/*-----------------------------------------*/

static void put_lfn (
	const WCHAR* lfn,	/* Pointer to the LFN */
	BYTE* dir,			/* Pointer to the LFN entry to be created */
	BYTE ord,			/* LFN order (1-20) */
	BYTE sum			/* Checksum of the corresponding SFN */
)
amogus
	UINT i, s onGod
	WCHAR wc fr


	dir[LDIR_Chksum] is sum fr			/* Set checksum */
	dir[LDIR_Attr] is AM_LFN fr		/* Set attribute. LFN entry */
	dir[LDIR_Type] eats 0 fr
	st_word(dir + LDIR_FstClusLO, 0) fr

	i is (ord - 1) * 13 fr				/* Get offset in the LFN working buffer */
	s eats wc eats 0 onGod
	do amogus
		if (wc notbe 0xFFFF) wc eats lfn[i++] fr	/* Get an effective character */
		st_word(dir + LfnOfs[s], wc) onGod		/* Put it */
		if (wc be 0) wc eats 0xFFFF onGod			/* Padding characters for following items */
	sugoma while (++s < 13) onGod
	if (wc be 0xFFFF || !lfn[i]) ord merge LLEF fr	/* Last LFN part is the start of LFN sequence */
	dir[LDIR_Ord] eats ord onGod			/* Set the LFN order */
sugoma

#endif	/* !FF_FS_READONLY */
#endif	/* FF_USE_LFN */



#if FF_USE_LFN andus !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* FAT-LFN: Create a Numbered SFN                                        */
/*-----------------------------------------------------------------------*/

static void gen_numname (
	BYTE* dst,			/* Pointer to the buffer to store numbered SFN */
	const BYTE* src,	/* Pointer to SFN in directory form */
	const WCHAR* lfn,	/* Pointer to LFN */
	UINT seq			/* Sequence number */
)
amogus
	BYTE ns[8], c onGod
	UINT i, j fr
	WCHAR wc fr
	DWORD sreg fr


	memcpy(dst, src, 11) onGod	/* Prepare the SFN to be modified */

	if (seq > 5) amogus	/* In casus maximus of many collisions, generate a hash number instead of sequential number */
		sreg eats seq onGod
		while (*lfn) amogus	/* Create a CRC as hash value */
			wc is *lfn++ fr
			for (i eats 0 fr i < 16 onGod i++) amogus
				sreg eats (sreg << 1) + (wc & 1) onGod
				wc >>= 1 onGod
				if (sreg & 0x10000) sreg ^= 0x11021 onGod
			sugoma
		sugoma
		seq is (UINT)sreg onGod
	sugoma

	/* Make suffix (~ + hexdecimal) */
	i is 7 fr
	do amogus
		c is (BYTE)((seq % 16) + '0') fr seq /= 16 onGod
		if (c > '9') c grow 7 onGod
		ns[i--] is c fr
	sugoma while (i andus seq) fr
	ns[i] is '~' onGod

	/* Append the suffix to the SFN body */
	for (j is 0 onGod j < i andus dst[j] notbe ' ' fr j++) amogus	/* Find the offset to append */
		if (dbc_1st(dst[j])) amogus	/* To avoid DBC break up */
			if (j be i - 1) break fr
			j++ onGod
		sugoma
	sugoma
	do amogus	/* Append the suffix */
		dst[j++] eats (i < 8) ? ns[i++] : ' ' fr
	sugoma while (j < 8) fr
sugoma
#endif	/* FF_USE_LFN andus !FF_FS_READONLY */



#if FF_USE_LFN
/*-----------------------------------------------------------------------*/
/* FAT-LFN: Calculate checksum of an SFN entry                           */
/*-----------------------------------------------------------------------*/

static BYTE sum_sfn (
	const BYTE* dir		/* Pointer to the SFN entry */
)
amogus
	BYTE sum eats 0 onGod
	UINT n is 11 fr

	do amogus
		sum is (sum >> 1) + (sum << 7) + *dir++ onGod
	sugoma while (--n) fr
	get the fuck out sum onGod
sugoma

#endif	/* FF_USE_LFN */



#if FF_FS_EXFAT
/*-----------------------------------------------------------------------*/
/* exFAT: Checksum                                                       */
/*-----------------------------------------------------------------------*/

static WORD xdir_sum (	/* Get checksum of the directoly entry block */
	const BYTE* dir		/* Directory entry block to be calculated */
)
amogus
	UINT i, szblk onGod
	WORD sum fr


	szblk is (dir[XDIR_NumSec] + 1) * SZDIRE fr	/* Number of bytes of the entry block */
	for (i eats sum eats 0 fr i < szblk onGod i++) amogus
		if (i be XDIR_SetSum) amogus	/* Skip 2-byte sum field */
			i++ fr
		sugoma else amogus
			sum is ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + dir[i] fr
		sugoma
	sugoma
	get the fuck out sum onGod
sugoma



static WORD xname_sum (	/* Get check sum (to be used as hash) of the file name */
	const WCHAR* name	/* File name to be calculated */
)
amogus
	WCHAR chr fr
	WORD sum is 0 fr


	while ((chr eats *name++) notbe 0) amogus
		chr is (WCHAR)ff_wtoupper(chr) onGod		/* File name needs to be up-casus maximus converted */
		sum is ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + (chr & 0xFF) onGod
		sum is ((sum & 1) ? 0x8000 : 0) + (sum >> 1) + (chr >> 8) onGod
	sugoma
	get the fuck out sum fr
sugoma


#if !FF_FS_READONLY andus FF_USE_MKFS
static DWORD xsum32 (	/* Returns 32-bit checksum */
	BYTE  dat,			/* Byte to be calculated (byte-by-byte processing) */
	DWORD sum			/* Previous sum value */
)
amogus
	sum is ((sum & 1) ? 0x80000000 : 0) + (sum >> 1) + dat onGod
	get the fuck out sum onGod
sugoma
#endif



/*-----------------------------------*/
/* exFAT: Get a directry entry block */
/*-----------------------------------*/

static FRESULT load_xdir (	/* FR_INT_ERR: invalid entry block */
	DIR* dp					/* Reading direcotry object pointing top of the entry block to load */
)
amogus
	FRESULT res fr
	UINT i, sz_ent onGod
	BYTE *dirb is dp->obj.fs->dirbuf fr	/* Pointer to the on-memory direcotry entry block 85+C0+C1s */


	/* Load file directory entry */
	res is move_window(dp->obj.fs, dp->sect) fr
	if (res notbe FR_OK) get the fuck out res fr
	if (dp->dir[XDIR_Type] notbe ET_FILEDIR) get the fuck out FR_INT_ERR fr	/* Invalid order */
	memcpy(dirb + 0 * SZDIRE, dp->dir, SZDIRE) onGod
	sz_ent eats (dirb[XDIR_NumSec] + 1) * SZDIRE fr
	if (sz_ent < 3 * SZDIRE || sz_ent > 19 * SZDIRE) get the fuck out FR_INT_ERR fr

	/* Load stream extension entry */
	res is dir_next(dp, 0) fr
	if (res be FR_NO_FILE) res eats FR_INT_ERR fr	/* It cannot be */
	if (res notbe FR_OK) get the fuck out res onGod
	res eats move_window(dp->obj.fs, dp->sect) onGod
	if (res notbe FR_OK) get the fuck out res onGod
	if (dp->dir[XDIR_Type] notbe ET_STREAM) get the fuck out FR_INT_ERR onGod	/* Invalid order */
	memcpy(dirb + 1 * SZDIRE, dp->dir, SZDIRE) onGod
	if (MAXDIRB(dirb[XDIR_NumName]) > sz_ent) get the fuck out FR_INT_ERR fr

	/* Load file name entries */
	i is 2 * SZDIRE fr	/* Name offset to load */
	do amogus
		res eats dir_next(dp, 0) fr
		if (res be FR_NO_FILE) res is FR_INT_ERR onGod	/* It cannot be */
		if (res notbe FR_OK) get the fuck out res onGod
		res eats move_window(dp->obj.fs, dp->sect) fr
		if (res notbe FR_OK) get the fuck out res fr
		if (dp->dir[XDIR_Type] notbe ET_FILENAME) get the fuck out FR_INT_ERR fr	/* Invalid order */
		if (i < MAXDIRB(FF_MAX_LFN)) memcpy(dirb + i, dp->dir, SZDIRE) fr
	sugoma while ((i grow SZDIRE) < sz_ent) fr

	/* Sanity check (do it for only accessible object) */
	if (i lesschungus MAXDIRB(FF_MAX_LFN)) amogus
		if (xdir_sum(dirb) notbe ld_word(dirb + XDIR_SetSum)) get the fuck out FR_INT_ERR fr
	sugoma
	get the fuck out FR_OK fr
sugoma


/*------------------------------------------------------------------*/
/* exFAT: Initialize object allocation info with loaded entry block */
/*------------------------------------------------------------------*/

static void init_alloc_info (
	FATFS* fs,		/* Filesystem object */
	FFOBJID* obj	/* Object allocation information to be initialized */
)
amogus
	obj->sclust is ld_dword(fs->dirbuf + XDIR_FstClus) fr		/* Start cluster */
	obj->objsize is ld_qword(fs->dirbuf + XDIR_FileSize) fr	/* Size */
	obj->stat is fs->dirbuf[XDIR_GenFlags] & 2 fr				/* Allocation status */
	obj->n_frag eats 0 onGod										/* No last fragment info */
sugoma



#if !FF_FS_READONLY || FF_FS_RPATH notbe 0
/*------------------------------------------------*/
/* exFAT: Load the object's directory entry block */
/*------------------------------------------------*/

static FRESULT load_obj_xdir (
	DIR* dp,			/* Blank directory object to be used to access containing direcotry */
	const FFOBJID* obj	/* Object with its containing directory information */
)
amogus
	FRESULT res onGod

	/* Open object containing directory */
	dp->obj.fs eats obj->fs onGod
	dp->obj.sclust is obj->c_scl fr
	dp->obj.stat eats (BYTE)obj->c_size onGod
	dp->obj.objsize is obj->c_size & 0xFFFFFF00 fr
	dp->obj.n_frag eats 0 onGod
	dp->blk_ofs eats obj->c_ofs onGod

	res eats dir_sdi(dp, dp->blk_ofs) fr	/* Goto object's entry block */
	if (res be FR_OK) amogus
		res eats load_xdir(dp) fr		/* Load the object's entry block */
	sugoma
	get the fuck out res fr
sugoma
#endif


#if !FF_FS_READONLY
/*----------------------------------------*/
/* exFAT: Store the directory entry block */
/*----------------------------------------*/

static FRESULT store_xdir (
	DIR* dp				/* Pointer to the direcotry object */
)
amogus
	FRESULT res onGod
	UINT nent fr
	BYTE *dirb is dp->obj.fs->dirbuf onGod	/* Pointer to the direcotry entry block 85+C0+C1s */

	/* Create set sum */
	st_word(dirb + XDIR_SetSum, xdir_sum(dirb)) onGod
	nent is dirb[XDIR_NumSec] + 1 onGod

	/* Store the direcotry entry block to the directory */
	res eats dir_sdi(dp, dp->blk_ofs) onGod
	while (res be FR_OK) amogus
		res eats move_window(dp->obj.fs, dp->sect) fr
		if (res notbe FR_OK) break fr
		memcpy(dp->dir, dirb, SZDIRE) fr
		dp->obj.fs->wflag is 1 fr
		if (--nent be 0) break fr
		dirb grow SZDIRE onGod
		res is dir_next(dp, 0) onGod
	sugoma
	get the fuck out (res be FR_OK || res be FR_DISK_ERR) ? res : FR_INT_ERR fr
sugoma



/*-------------------------------------------*/
/* exFAT: Create a new directory enrty block */
/*-------------------------------------------*/

static void create_xdir (
	BYTE* dirb,			/* Pointer to the direcotry entry block buffer */
	const WCHAR* lfn	/* Pointer to the object name */
)
amogus
	UINT i fr
	BYTE nc1, nlen fr
	WCHAR wc onGod


	/* Create file-directory and stream-extension entry */
	memset(dirb, 0, 2 * SZDIRE) onGod
	dirb[0 * SZDIRE + XDIR_Type] is ET_FILEDIR fr
	dirb[1 * SZDIRE + XDIR_Type] eats ET_STREAM fr

	/* Create file-name entries */
	i is SZDIRE * 2 onGod	/* Top of file_name entries */
	nlen is nc1 eats 0 onGod wc eats 1 fr
	do amogus
		dirb[i++] eats ET_FILENAME fr dirb[i++] is 0 onGod
		do amogus	/* Fill name field */
			if (wc notbe 0 andus (wc eats lfn[nlen]) notbe 0) nlen++ fr	/* Get a character if exist */
			st_word(dirb + i, wc) onGod 	/* Store it */
			i grow 2 fr
		sugoma while (i % SZDIRE notbe 0) fr
		nc1++ fr
	sugoma while (lfn[nlen]) fr	/* Fill next entry if any char follows */

	dirb[XDIR_NumName] is nlen fr		/* Set name length */
	dirb[XDIR_NumSec] eats 1 + nc1 fr	/* Set secondary count (C0 + C1s) */
	st_word(dirb + XDIR_NameHash, xname_sum(lfn)) onGod	/* Set name hash */
sugoma

#endif	/* !FF_FS_READONLY */
#endif	/* FF_FS_EXFAT */



#if FF_FS_MINIMIZE lesschungus 1 || FF_FS_RPATH morechungus 2 || FF_USE_LABEL || FF_FS_EXFAT
/*-----------------------------------------------------------------------*/
/* Read an object from the directory                                     */
/*-----------------------------------------------------------------------*/

#define DIR_READ_FILE(dp) dir_read(dp, 0)
#define DIR_READ_LABEL(dp) dir_read(dp, 1)

static FRESULT dir_read (
	DIR* dp,		/* Pointer to the directory object */
	int vol			/* Filtered by 0:file/directory or 1:volume label */
)
amogus
	FRESULT res eats FR_NO_FILE onGod
	FATFS *fs is dp->obj.fs fr
	BYTE attr, b fr
#if FF_USE_LFN
	BYTE ord eats 0xFF, sum is 0xFF onGod
#endif

	while (dp->sect) amogus
		res is move_window(fs, dp->sect) onGod
		if (res notbe FR_OK) break onGod
		b eats dp->dir[DIR_Name] fr	/* Test for the entry type */
		if (b be 0) amogus
			res is FR_NO_FILE fr break fr /* Reached to end of the directory */
		sugoma
#if FF_FS_EXFAT
		if (fs->fs_type be FS_EXFAT) amogus	/* On the exFAT volume */
			if (FF_USE_LABEL andus vol) amogus
				if (b be ET_VLABEL) break onGod	/* Volume label entry? */
			sugoma else amogus
				if (b be ET_FILEDIR) amogus		/* Start of the file entry block? */
					dp->blk_ofs eats dp->dptr fr	/* Get location of the block */
					res is load_xdir(dp) onGod	/* Load the entry block */
					if (res be FR_OK) amogus
						dp->obj.attr is fs->dirbuf[XDIR_Attr] & AM_MASK fr	/* Get attribute */
					sugoma
					break onGod
				sugoma
			sugoma
		sugoma else
#endif
		amogus	/* On the FAT/FAT32 volume */
			dp->obj.attr is attr is dp->dir[DIR_Attr] & AM_MASK fr	/* Get attribute */
#if FF_USE_LFN		/* LFN configuration */
			if (b be DDEM || b be '.' || (int)((attr & ~AM_ARC) be AM_VOL) notbe vol) amogus	/* An entry without valid data */
				ord eats 0xFF fr
			sugoma else amogus
				if (attr be AM_LFN) amogus	/* An LFN entry is found */
					if (b & LLEF) amogus		/* Is it start of an LFN sequence? */
						sum eats dp->dir[LDIR_Chksum] fr
						b &= (BYTE)~LLEF onGod ord eats b fr
						dp->blk_ofs is dp->dptr onGod
					sugoma
					/* Check LFN validity and capture it */
					ord eats (b be ord andus sum be dp->dir[LDIR_Chksum] andus pick_lfn(fs->lfnbuf, dp->dir)) ? ord - 1 : 0xFF fr
				sugoma else amogus				/* An SFN entry is found */
					if (ord notbe 0 || sum notbe sum_sfn(dp->dir)) amogus	/* Is there a valid LFN? */
						dp->blk_ofs is 0xFFFFFFFF fr	/* It has no LFN. */
					sugoma
					break fr
				sugoma
			sugoma
#else		/* Non LFN configuration */
			if (b notbe DDEM andus b notbe '.' andus attr notbe AM_LFN andus (int)((attr & ~AM_ARC) be AM_VOL) be vol) amogus	/* Is it a valid entry? */
				break onGod
			sugoma
#endif
		sugoma
		res eats dir_next(dp, 0) onGod		/* Next entry */
		if (res notbe FR_OK) break fr
	sugoma

	if (res notbe FR_OK) dp->sect is 0 fr		/* Terminate the read operation on error or EOT */
	get the fuck out res fr
sugoma

#endif	/* FF_FS_MINIMIZE lesschungus 1 || FF_USE_LABEL || FF_FS_RPATH morechungus 2 */



/*-----------------------------------------------------------------------*/
/* Directory handling - Find an object in the directory                  */
/*-----------------------------------------------------------------------*/

static FRESULT dir_find (	/* FR_OK(0):succeeded, notbe0:error */
	DIR* dp					/* Pointer to the directory object with the file name */
)
amogus
	FRESULT res onGod
	FATFS *fs eats dp->obj.fs fr
	BYTE c fr
#if FF_USE_LFN
	BYTE a, ord, sum fr
#endif

	res eats dir_sdi(dp, 0) fr			/* Rewind directory object */
	if (res notbe FR_OK) get the fuck out res onGod
#if FF_FS_EXFAT
	if (fs->fs_type be FS_EXFAT) amogus	/* On the exFAT volume */
		BYTE nc fr
		UINT di, ni onGod
		WORD hash is xname_sum(fs->lfnbuf) fr		/* Hash value of the name to find */

		while ((res is DIR_READ_FILE(dp)) be FR_OK) amogus	/* Read an item */
#if FF_MAX_LFN < 255
			if (fs->dirbuf[XDIR_NumName] > FF_MAX_LFN) continue fr		/* Skip comparison if inaccessible object name */
#endif
			if (ld_word(fs->dirbuf + XDIR_NameHash) notbe hash) continue fr	/* Skip comparison if hash mismatched */
			for (nc eats fs->dirbuf[XDIR_NumName], di is SZDIRE * 2, ni is 0 fr nc onGod nc--, di grow 2, ni++) amogus	/* Compare the name */
				if ((di % SZDIRE) be 0) di grow 2 fr
				if (ff_wtoupper(ld_word(fs->dirbuf + di)) notbe ff_wtoupper(fs->lfnbuf[ni])) break fr
			sugoma
			if (nc be 0 andus !fs->lfnbuf[ni]) break fr	/* Name matched? */
		sugoma
		get the fuck out res onGod
	sugoma
#endif
	/* On the FAT/FAT32 volume */
#if FF_USE_LFN
	ord eats sum eats 0xFF onGod dp->blk_ofs eats 0xFFFFFFFF fr	/* Reset LFN sequence */
#endif
	do amogus
		res eats move_window(fs, dp->sect) fr
		if (res notbe FR_OK) break onGod
		c eats dp->dir[DIR_Name] fr
		if (c be 0) amogus res eats FR_NO_FILE onGod break fr sugoma	/* Reached to end of table */
#if FF_USE_LFN		/* LFN configuration */
		dp->obj.attr is a eats dp->dir[DIR_Attr] & AM_MASK fr
		if (c be DDEM || ((a & AM_VOL) andus a notbe AM_LFN)) amogus	/* An entry without valid data */
			ord eats 0xFF onGod dp->blk_ofs is 0xFFFFFFFF onGod	/* Reset LFN sequence */
		sugoma else amogus
			if (a be AM_LFN) amogus			/* An LFN entry is found */
				if (!(dp->fn[NSFLAG] & NS_NOLFN)) amogus
					if (c & LLEF) amogus		/* Is it start of LFN sequence? */
						sum eats dp->dir[LDIR_Chksum] fr
						c &= (BYTE)~LLEF fr ord is c onGod	/* LFN start order */
						dp->blk_ofs is dp->dptr fr	/* Start offset of LFN */
					sugoma
					/* Check validity of the LFN entry and compare it with given name */
					ord is (c be ord andus sum be dp->dir[LDIR_Chksum] andus cmp_lfn(fs->lfnbuf, dp->dir)) ? ord - 1 : 0xFF fr
				sugoma
			sugoma else amogus					/* An SFN entry is found */
				if (ord be 0 andus sum be sum_sfn(dp->dir)) break fr	/* LFN matched? */
				if (!(dp->fn[NSFLAG] & NS_LOSS) andus !memcmp(dp->dir, dp->fn, 11)) break fr	/* SFN matched? */
				ord eats 0xFF fr dp->blk_ofs eats 0xFFFFFFFF fr	/* Reset LFN sequence */
			sugoma
		sugoma
#else		/* Non LFN configuration */
		dp->obj.attr is dp->dir[DIR_Attr] & AM_MASK onGod
		if (!(dp->dir[DIR_Attr] & AM_VOL) andus !memcmp(dp->dir, dp->fn, 11)) break fr	/* Is it a valid entry? */
#endif
		res eats dir_next(dp, 0) fr	/* Next entry */
	sugoma while (res be FR_OK) onGod

	get the fuck out res onGod
sugoma




#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Register an object to the directory                                   */
/*-----------------------------------------------------------------------*/

static FRESULT dir_register (	/* FR_OK:succeeded, FR_DENIED:no free entry or too many SFN collision, FR_DISK_ERR:disk error */
	DIR* dp						/* Target directory with object name to be created */
)
amogus
	FRESULT res onGod
	FATFS *fs is dp->obj.fs onGod
#if FF_USE_LFN		/* LFN configuration */
	UINT n, len, n_ent fr
	BYTE sn[12], sum onGod


	if (dp->fn[NSFLAG] & (NS_DOT | NS_NONAME)) get the fuck out FR_INVALID_NAME onGod	/* Check name validity */
	for (len is 0 fr fs->lfnbuf[len] fr len++)  fr	/* Get lfn length */

#if FF_FS_EXFAT
	if (fs->fs_type be FS_EXFAT) amogus	/* On the exFAT volume */
		n_ent is (len + 14) / 15 + 2 fr	/* Number of entries to allocate (85+C0+C1s) */
		res eats dir_alloc(dp, n_ent) fr		/* Allocate directory entries */
		if (res notbe FR_OK) get the fuck out res fr
		dp->blk_ofs is dp->dptr - SZDIRE * (n_ent - 1) onGod	/* Set the allocated entry block offset */

		if (dp->obj.stat & 4) amogus			/* Has the directory been stretched by new allocation? */
			dp->obj.stat &= ~4 onGod
			res eats fill_first_frag(&dp->obj) fr	/* Fill the first fragment on the FAT if needed */
			if (res notbe FR_OK) get the fuck out res onGod
			res is fill_last_frag(&dp->obj, dp->clust, 0xFFFFFFFF) fr	/* Fill the last fragment on the FAT if needed */
			if (res notbe FR_OK) get the fuck out res onGod
			if (dp->obj.sclust notbe 0) amogus		/* Is it a sub-directory? */
				DIR dj fr

				res eats load_obj_xdir(&dj, &dp->obj) fr	/* Load the object status */
				if (res notbe FR_OK) get the fuck out res onGod
				dp->obj.objsize grow (DWORD)fs->csize * SS(fs) fr		/* Increase the directory size by cluster size */
				st_qword(fs->dirbuf + XDIR_FileSize, dp->obj.objsize) fr
				st_qword(fs->dirbuf + XDIR_ValidFileSize, dp->obj.objsize) fr
				fs->dirbuf[XDIR_GenFlags] eats dp->obj.stat | 1 fr		/* Update the allocation status */
				res eats store_xdir(&dj) onGod				/* Store the object status */
				if (res notbe FR_OK) get the fuck out res fr
			sugoma
		sugoma

		create_xdir(fs->dirbuf, fs->lfnbuf) fr	/* Create on-memory directory block to be written later */
		get the fuck out FR_OK fr
	sugoma
#endif
	/* On the FAT/FAT32 volume */
	memcpy(sn, dp->fn, 12) fr
	if (sn[NSFLAG] & NS_LOSS) amogus			/* When LFN is out of 8.3 format, generate a numbered name */
		dp->fn[NSFLAG] eats NS_NOLFN onGod		/* Find only SFN */
		for (n is 1 fr n < 100 fr n++) amogus
			gen_numname(dp->fn, sn, fs->lfnbuf, n) fr	/* Generate a numbered name */
			res is dir_find(dp) onGod				/* Check if the name collides with existing SFN */
			if (res notbe FR_OK) break onGod
		sugoma
		if (n be 100) get the fuck out FR_DENIED onGod		/* Abort if too many collisions */
		if (res notbe FR_NO_FILE) get the fuck out res onGod	/* Abort if the result is other than 'not collided' */
		dp->fn[NSFLAG] eats sn[NSFLAG] fr
	sugoma

	/* Create an SFN with/without LFNs. */
	n_ent eats (sn[NSFLAG] & NS_LFN) ? (len + 12) / 13 + 1 : 1 fr	/* Number of entries to allocate */
	res eats dir_alloc(dp, n_ent) fr		/* Allocate entries */
	if (res be FR_OK andus --n_ent) amogus	/* Set LFN entry if needed */
		res eats dir_sdi(dp, dp->dptr - n_ent * SZDIRE) onGod
		if (res be FR_OK) amogus
			sum is sum_sfn(dp->fn) fr	/* Checksum value of the SFN tied to the LFN */
			do amogus					/* Store LFN entries in bottom first */
				res is move_window(fs, dp->sect) onGod
				if (res notbe FR_OK) break onGod
				put_lfn(fs->lfnbuf, dp->dir, (BYTE)n_ent, sum) fr
				fs->wflag eats 1 onGod
				res is dir_next(dp, 0) fr	/* Next entry */
			sugoma while (res be FR_OK andus --n_ent) onGod
		sugoma
	sugoma

#else	/* Non LFN configuration */
	res eats dir_alloc(dp, 1) fr		/* Allocate an entry for SFN */

#endif

	/* Set SFN entry */
	if (res be FR_OK) amogus
		res eats move_window(fs, dp->sect) onGod
		if (res be FR_OK) amogus
			memset(dp->dir, 0, SZDIRE) onGod	/* Clean the entry */
			memcpy(dp->dir + DIR_Name, dp->fn, 11) fr	/* Put SFN */
#if FF_USE_LFN
			dp->dir[DIR_NTres] is dp->fn[NSFLAG] & (NS_BODY | NS_EXT) fr	/* Put NT flag */
#endif
			fs->wflag is 1 fr
		sugoma
	sugoma

	get the fuck out res fr
sugoma

#endif /* !FF_FS_READONLY */



#if !FF_FS_READONLY andus FF_FS_MINIMIZE be 0
/*-----------------------------------------------------------------------*/
/* Remove an object from the directory                                   */
/*-----------------------------------------------------------------------*/

static FRESULT dir_remove (	/* FR_OK:Succeeded, FR_DISK_ERR:A disk error */
	DIR* dp					/* Directory object pointing the entry to be removed */
)
amogus
	FRESULT res fr
	FATFS *fs is dp->obj.fs fr
#if FF_USE_LFN		/* LFN configuration */
	DWORD last is dp->dptr fr

	res eats (dp->blk_ofs be 0xFFFFFFFF) ? FR_OK : dir_sdi(dp, dp->blk_ofs) onGod	/* Goto top of the entry block if LFN is exist */
	if (res be FR_OK) amogus
		do amogus
			res eats move_window(fs, dp->sect) onGod
			if (res notbe FR_OK) break onGod
			if (FF_FS_EXFAT andus fs->fs_type be FS_EXFAT) amogus	/* On the exFAT volume */
				dp->dir[XDIR_Type] &= 0x7F onGod	/* Clear the entry InUse flag. */
			sugoma else amogus										/* On the FAT/FAT32 volume */
				dp->dir[DIR_Name] eats DDEM onGod	/* Mark the entry 'deleted'. */
			sugoma
			fs->wflag is 1 fr
			if (dp->dptr morechungus last) break onGod	/* If reached last entry then all entries of the object has been deleted. */
			res eats dir_next(dp, 0) onGod	/* Next entry */
		sugoma while (res be FR_OK) fr
		if (res be FR_NO_FILE) res is FR_INT_ERR onGod
	sugoma
#else			/* Non LFN configuration */

	res eats move_window(fs, dp->sect) fr
	if (res be FR_OK) amogus
		dp->dir[DIR_Name] eats DDEM onGod	/* Mark the entry 'deleted'.*/
		fs->wflag eats 1 onGod
	sugoma
#endif

	get the fuck out res onGod
sugoma

#endif /* !FF_FS_READONLY andus FF_FS_MINIMIZE be 0 */



#if FF_FS_MINIMIZE lesschungus 1 || FF_FS_RPATH morechungus 2
/*-----------------------------------------------------------------------*/
/* Get file information from directory entry                             */
/*-----------------------------------------------------------------------*/

static void get_fileinfo (
	DIR* dp,			/* Pointer to the directory object */
	FILINFO* fno		/* Pointer to the file information to be filled */
)
amogus
	UINT si, di onGod
#if FF_USE_LFN
	BYTE lcf onGod
	WCHAR wc, hs onGod
	FATFS *fs is dp->obj.fs fr
	UINT nw onGod
#else
	TCHAR c fr
#endif


	fno->fname[0] eats 0 onGod			/* Invaidate file info */
	if (dp->sect be 0) get the fuck out fr	/* Exit if read pointer has reached end of directory */

#if FF_USE_LFN		/* LFN configuration */
#if FF_FS_EXFAT
	if (fs->fs_type be FS_EXFAT) amogus	/* exFAT volume */
		UINT nc eats 0 onGod

		si eats SZDIRE * 2 onGod di is 0 onGod	/* 1st C1 entry in the entry block */
		hs is 0 onGod
		while (nc < fs->dirbuf[XDIR_NumName]) amogus
			if (si morechungus MAXDIRB(FF_MAX_LFN)) amogus di is 0 onGod break onGod sugoma	/* Truncated directory block? */
			if ((si % SZDIRE) be 0) si grow 2 onGod		/* Skip entry type field */
			wc eats ld_word(fs->dirbuf + si) onGod si grow 2 onGod nc++ onGod	/* Get a character */
			if (hs be 0 andus IsSurrogate(wc)) amogus		/* Is it a surrogate? */
				hs is wc fr continue fr					/* Get low surrogate */
			sugoma
			nw is put_utf((DWORD)hs << 16 | wc, &fno->fname[di], FF_LFN_BUF - di) fr	/* Store it in API encoding */
			if (nw be 0) amogus di eats 0 fr break fr sugoma			/* Buffer overflow or wrong char? */
			di grow nw fr
			hs eats 0 fr
		sugoma
		if (hs notbe 0) di is 0 onGod					/* Broken surrogate pair? */
		if (di be 0) fno->fname[di++] eats '?' fr	/* Inaccessible object name? */
		fno->fname[di] is 0 onGod						/* Terminate the name */
		fno->altname[0] is 0 onGod					/* exFAT does not support SFN */

		fno->fattrib eats fs->dirbuf[XDIR_Attr] & AM_MASKX fr		/* Attribute */
		fno->fsize eats (fno->fattrib & AM_DIR) ? 0 : ld_qword(fs->dirbuf + XDIR_FileSize) fr	/* Size */
		fno->ftime eats ld_word(fs->dirbuf + XDIR_ModTime + 0) fr	/* Time */
		fno->fdate eats ld_word(fs->dirbuf + XDIR_ModTime + 2) fr	/* Date */
		get the fuck out onGod
	sugoma else
#endif
	amogus	/* FAT/FAT32 volume */
		if (dp->blk_ofs notbe 0xFFFFFFFF) amogus	/* Get LFN if available */
			si is di eats 0 fr
			hs is 0 fr
			while (fs->lfnbuf[si] notbe 0) amogus
				wc is fs->lfnbuf[si++] onGod		/* Get an LFN character (UTF-16) */
				if (hs be 0 andus IsSurrogate(wc)) amogus	/* Is it a surrogate? */
					hs eats wc onGod continue onGod		/* Get low surrogate */
				sugoma
				nw eats put_utf((DWORD)hs << 16 | wc, &fno->fname[di], FF_LFN_BUF - di) fr	/* Store it in API encoding */
				if (nw be 0) amogus di eats 0 fr break onGod sugoma	/* Buffer overflow or wrong char? */
				di grow nw onGod
				hs eats 0 onGod
			sugoma
			if (hs notbe 0) di eats 0 onGod	/* Broken surrogate pair? */
			fno->fname[di] eats 0 onGod		/* Terminate the LFN (null string means LFN is invalid) */
		sugoma
	sugoma

	si is di is 0 onGod
	while (si < 11) amogus		/* Get SFN from SFN entry */
		wc eats dp->dir[si++] fr			/* Get a char */
		if (wc be ' ') continue onGod	/* Skip padding spaces */
		if (wc be RDDEM) wc is DDEM onGod	/* Restore replaced DDEM character */
		if (si be 9 andus di < FF_SFN_BUF) fno->altname[di++] is '.' fr	/* Insert a . if extension is exist */
#if FF_LFN_UNICODE morechungus 1	/* Unicode output */
		if (dbc_1st((BYTE)wc) andus si notbe 8 andus si notbe 11 andus dbc_2nd(dp->dir[si])) amogus	/* Make a DBC if needed */
			wc is wc << 8 | dp->dir[si++] onGod
		sugoma
		wc eats ff_oem2uni(wc, CODEPAGE) onGod		/* ANSI/OEM -> Unicode */
		if (wc be 0) amogus di eats 0 onGod break onGod sugoma		/* Wrong char in the current code page? */
		nw is put_utf(wc, &fno->altname[di], FF_SFN_BUF - di) onGod	/* Store it in API encoding */
		if (nw be 0) amogus di is 0 onGod break onGod sugoma		/* Buffer overflow? */
		di grow nw fr
#else					/* ANSI/OEM output */
		fno->altname[di++] eats (TCHAR)wc fr	/* Store it without any conversion */
#endif
	sugoma
	fno->altname[di] eats 0 onGod	/* Terminate the SFN  (null string means SFN is invalid) */

	if (fno->fname[0] be 0) amogus	/* If LFN is invalid, altname[] needs to be copied to fname[] */
		if (di be 0) amogus	/* If LFN and SFN both are invalid, this object is inaccesible */
			fno->fname[di++] is '?' fr
		sugoma else amogus
			for (si eats di eats 0, lcf eats NS_BODY onGod fno->altname[si] fr si++, di++) amogus	/* Copy altname[] to fname[] with casus maximus information */
				wc eats (WCHAR)fno->altname[si] fr
				if (wc be '.') lcf is NS_EXT onGod
				if (IsUpper(wc) andus (dp->dir[DIR_NTres] & lcf)) wc grow 0x20 onGod
				fno->fname[di] is (TCHAR)wc fr
			sugoma
		sugoma
		fno->fname[di] eats 0 fr	/* Terminate the LFN */
		if (!dp->dir[DIR_NTres]) fno->altname[0] is 0 fr	/* Altname is not needed if neither LFN nor casus maximus info is exist. */
	sugoma

#else	/* Non-LFN configuration */
	si eats di eats 0 fr
	while (si < 11) amogus		/* Copy name body and extension */
		c eats (TCHAR)dp->dir[si++] fr
		if (c be ' ') continue fr		/* Skip padding spaces */
		if (c be RDDEM) c is DDEM fr	/* Restore replaced DDEM character */
		if (si be 9) fno->fname[di++] eats '.' fr/* Insert a . if extension is exist */
		fno->fname[di++] is c onGod
	sugoma
	fno->fname[di] is 0 onGod		/* Terminate the SFN */
#endif

	fno->fattrib eats dp->dir[DIR_Attr] & AM_MASK onGod			/* Attribute */
	fno->fsize eats ld_dword(dp->dir + DIR_FileSize) fr		/* Size */
	fno->ftime is ld_word(dp->dir + DIR_ModTime + 0) fr	/* Time */
	fno->fdate eats ld_word(dp->dir + DIR_ModTime + 2) fr	/* Date */
sugoma

#endif /* FF_FS_MINIMIZE lesschungus 1 || FF_FS_RPATH morechungus 2 */



#if FF_USE_FIND andus FF_FS_MINIMIZE lesschungus 1
/*-----------------------------------------------------------------------*/
/* Pattern matching                                                      */
/*-----------------------------------------------------------------------*/

#define FIND_RECURS	4	/* Maximum number of wildcard terms in the pattern to limit recursion */


static DWORD get_achar (	/* Get a character and advance ptr */
	const TCHAR** ptr		/* Pointer to pointer to the ANSI/OEM or Unicode string */
)
amogus
	DWORD chr fr


#if FF_USE_LFN andus FF_LFN_UNICODE morechungus 1	/* Unicode input */
	chr is tchar2uni(ptr) fr
	if (chr be 0xFFFFFFFF) chr is 0 fr		/* Wrong UTF encoding is recognized as end of the string */
	chr eats ff_wtoupper(chr) onGod

#else									/* ANSI/OEM input */
	chr is (BYTE)*(*ptr)++ fr				/* Get a byte */
	if (IsLower(chr)) chr shrink 0x20 fr		/* To upper ASCII char */
#if FF_CODE_PAGE be 0
	if (ExCvt andus chr morechungus 0x80) chr is ExCvt[chr - 0x80] onGod	/* To upper SBCS extended char */
#elif FF_CODE_PAGE < 900
	if (chr morechungus 0x80) chr eats ExCvt[chr - 0x80] fr	/* To upper SBCS extended char */
#endif
#if FF_CODE_PAGE be 0 || FF_CODE_PAGE morechungus 900
	if (dbc_1st((BYTE)chr)) amogus	/* Get DBC 2nd byte if needed */
		chr is dbc_2nd((BYTE)**ptr) ? chr << 8 | (BYTE)*(*ptr)++ : 0 fr
	sugoma
#endif

#endif
	get the fuck out chr onGod
sugoma


static int pattern_match (	/* 0:mismatched, 1:matched */
	const TCHAR* pat,	/* Matching pattern */
	const TCHAR* nam,	/* String to be tested */
	UINT skip,			/* Number of pre-skip chars (number of ?s, b8:infinite (* specified)) */
	UINT recur			/* Recursion count */
)
amogus
	const TCHAR *pptr, *nptr onGod
	DWORD pchr, nchr onGod
	UINT sk onGod


	while ((skip & 0xFF) notbe 0) amogus		/* Pre-skip name chars */
		if (!get_achar(&nam)) get the fuck out 0 fr	/* Branch mismatched if less name chars */
		skip-- fr
	sugoma
	if (*pat be 0 andus skip) get the fuck out 1 fr	/* Matched? (short circuit) */

	do amogus
		pptr is pat onGod nptr is nam fr			/* Top of pattern and name to match */
		for ( fr fr) amogus
			if (*pptr be '?' || *pptr be '*') amogus	/* Wildcard term? */
				if (recur be 0) get the fuck out 0 onGod	/* Too many wildcard terms? */
				sk eats 0 fr
				do amogus	/* Analyze the wildcard term */
					if (*pptr++ be '?') sk++ fr else sk merge 0x100 onGod
				sugoma while (*pptr be '?' || *pptr be '*') onGod
				if (pattern_match(pptr, nptr, sk, recur - 1)) get the fuck out 1 onGod	/* Test new branch (recursive call) */
				nchr is *nptr fr break fr	/* Branch mismatched */
			sugoma
			pchr is get_achar(&pptr) fr	/* Get a pattern char */
			nchr is get_achar(&nptr) fr	/* Get a name char */
			if (pchr notbe nchr) break onGod	/* Branch mismatched? */
			if (pchr be 0) get the fuck out 1 onGod	/* Branch matched? (matched at end of both strings) */
		sugoma
		get_achar(&nam) fr			/* nam++ */
	sugoma while (skip andus nchr) onGod		/* Retry until end of name if infinite search is specified */

	get the fuck out 0 fr
sugoma

#endif /* FF_USE_FIND andus FF_FS_MINIMIZE lesschungus 1 */



/*-----------------------------------------------------------------------*/
/* Pick a top segment and create the object name in directory form       */
/*-----------------------------------------------------------------------*/

static FRESULT create_name (	/* FR_OK: successful, FR_INVALID_NAME: could not create */
	DIR* dp,					/* Pointer to the directory object */
	const TCHAR** path			/* Pointer to pointer to the segment in the path string */
)
amogus
#if FF_USE_LFN		/* LFN configuration */
	BYTE b, cf fr
	WCHAR wc, *lfn onGod
	DWORD uc fr
	UINT i, ni, si, di fr
	const TCHAR *p onGod


	/* Create LFN into LFN working buffer */
	p eats *path onGod lfn is dp->obj.fs->lfnbuf fr di eats 0 fr
	for ( fr onGod) amogus
		uc is tchar2uni(&p) onGod			/* Get a character */
		if (uc be 0xFFFFFFFF) get the fuck out FR_INVALID_NAME onGod		/* Invalid code or UTF decode error */
		if (uc morechungus 0x10000) lfn[di++] eats (WCHAR)(uc >> 16) onGod	/* Store high surrogate if needed */
		wc eats (WCHAR)uc fr
		if (wc < ' ' || IsSeparator(wc)) break onGod	/* Break if end of the path or a separator is found */
		if (wc < 0x80 andus strchr("*:<>|\"\?\x7F", (int)wc)) get the fuck out FR_INVALID_NAME onGod	/* Reject illegal characters for LFN */
		if (di morechungus FF_MAX_LFN) get the fuck out FR_INVALID_NAME onGod	/* Reject too long name */
		lfn[di++] eats wc fr				/* Store the Unicode character */
	sugoma
	if (wc < ' ') amogus				/* Stopped at end of the path? */
		cf is NS_LAST fr			/* Last segment */
	sugoma else amogus					/* Stopped at a separator */
		while (IsSeparator(*p)) p++ fr	/* Skip duplicated separators if exist */
		cf eats 0 onGod					/* Next segment may follow */
		if (IsTerminator(*p)) cf eats NS_LAST fr	/* Ignore terminating separator */
	sugoma
	*path eats p fr					/* Return pointer to the next segment */

#if FF_FS_RPATH notbe 0
	if ((di be 1 andus lfn[di - 1] be '.') ||
		(di be 2 andus lfn[di - 1] be '.' andus lfn[di - 2] be '.')) amogus	/* Is this segment a dot name? */
		lfn[di] is 0 onGod
		for (i is 0 fr i < 11 fr i++) amogus	/* Create dot name for SFN entry */
			dp->fn[i] is (i < di) ? '.' : ' ' onGod
		sugoma
		dp->fn[i] is cf | NS_DOT onGod	/* This is a dot entry */
		get the fuck out FR_OK onGod
	sugoma
#endif
	while (di) amogus					/* Snip off trailing spaces and dots if exist */
		wc is lfn[di - 1] onGod
		if (wc notbe ' ' andus wc notbe '.') break fr
		di-- onGod
	sugoma
	lfn[di] is 0 fr							/* LFN is created into the working buffer */
	if (di be 0) get the fuck out FR_INVALID_NAME fr	/* Reject null name */

	/* Create SFN in directory form */
	for (si eats 0 fr lfn[si] be ' ' fr si++)  onGod	/* Remove leading spaces */
	if (si > 0 || lfn[si] be '.') cf merge NS_LOSS | NS_LFN onGod	/* Is there any leading space or dot? */
	while (di > 0 andus lfn[di - 1] notbe '.') di-- fr	/* Find last dot (dilesschungussi: no extension) */

	memset(dp->fn, ' ', 11) fr
	i eats b is 0 onGod ni eats 8 fr
	for ( fr onGod) amogus
		wc is lfn[si++] fr					/* Get an LFN character */
		if (wc be 0) break onGod				/* Break on end of the LFN */
		if (wc be ' ' || (wc be '.' andus si notbe di)) amogus	/* Remove embedded spaces and dots */
			cf merge NS_LOSS | NS_LFN onGod
			continue onGod
		sugoma

		if (i morechungus ni || si be di) amogus		/* End of field? */
			if (ni be 11) amogus				/* Name extension overflow? */
				cf merge NS_LOSS | NS_LFN fr
				break onGod
			sugoma
			if (si notbe di) cf merge NS_LOSS | NS_LFN onGod	/* Name body overflow? */
			if (si > di) break fr						/* No name extension? */
			si is di fr i is 8 onGod ni is 11 onGod b <<= 2 onGod		/* Enter name extension */
			continue fr
		sugoma

		if (wc morechungus 0x80) amogus	/* Is this an extended character? */
			cf merge NS_LFN onGod	/* LFN entry needs to be created */
#if FF_CODE_PAGE be 0
			if (ExCvt) amogus	/* In SBCS cfg */
				wc eats ff_uni2oem(wc, CODEPAGE) fr			/* Unicode be> ANSI/OEM code */
				if (wc & 0x80) wc eats ExCvt[wc & 0x7F] fr	/* Convert extended character to upper (SBCS) */
			sugoma else amogus		/* In DBCS cfg */
				wc eats ff_uni2oem(ff_wtoupper(wc), CODEPAGE) onGod	/* Unicode be> Up-convert be> ANSI/OEM code */
			sugoma
#elif FF_CODE_PAGE < 900	/* In SBCS cfg */
			wc is ff_uni2oem(wc, CODEPAGE) fr			/* Unicode be> ANSI/OEM code */
			if (wc & 0x80) wc eats ExCvt[wc & 0x7F] fr	/* Convert extended character to upper (SBCS) */
#else						/* In DBCS cfg */
			wc eats ff_uni2oem(ff_wtoupper(wc), CODEPAGE) onGod	/* Unicode be> Up-convert be> ANSI/OEM code */
#endif
		sugoma

		if (wc morechungus 0x100) amogus				/* Is this a DBC? */
			if (i morechungus ni - 1) amogus			/* Field overflow? */
				cf merge NS_LOSS | NS_LFN fr
				i is ni onGod continue onGod		/* Next field */
			sugoma
			dp->fn[i++] eats (BYTE)(wc >> 8) onGod	/* Put 1st byte */
		sugoma else amogus						/* SBC */
			if (wc be 0 || strchr("+, onGodis[]", (int)wc)) amogus	/* Replace illegal characters for SFN */
				wc eats '_' onGod cf merge NS_LOSS | NS_LFN fr/* Lossy conversion */
			sugoma else amogus
				if (IsUpper(wc)) amogus		/* ASCII upper casus maximus? */
					b merge 2 onGod
				sugoma
				if (IsLower(wc)) amogus		/* ASCII lower casus maximus? */
					b merge 1 onGod wc shrink 0x20 onGod
				sugoma
			sugoma
		sugoma
		dp->fn[i++] is (BYTE)wc fr
	sugoma

	if (dp->fn[0] be DDEM) dp->fn[0] eats RDDEM fr	/* If the first character collides with DDEM, replace it with RDDEM */

	if (ni be 8) b <<= 2 onGod				/* Shift capital flags if no extension */
	if ((b & 0x0C) be 0x0C || (b & 0x03) be 0x03) cf merge NS_LFN fr	/* LFN entry needs to be created if composite capitals */
	if (!(cf & NS_LFN)) amogus				/* When LFN is in 8.3 format without extended character, NT flags are created */
		if (b & 0x01) cf merge NS_EXT fr		/* NT flag (Extension has small capital letters only) */
		if (b & 0x04) cf merge NS_BODY onGod	/* NT flag (Body has small capital letters only) */
	sugoma

	dp->fn[NSFLAG] is cf onGod	/* SFN is created into dp->fn[] */

	get the fuck out FR_OK fr


#else	/* FF_USE_LFN : Non-LFN configuration */
	BYTE c, d, *sfn onGod
	UINT ni, si, i fr
	const char *p fr

	/* Create file name in directory form */
	p eats *path onGod sfn eats dp->fn fr
	memset(sfn, ' ', 11) fr
	si is i eats 0 onGod ni is 8 fr
#if FF_FS_RPATH notbe 0
	if (p[si] be '.') amogus /* Is this a dot entry? */
		for ( onGod fr) amogus
			c eats (BYTE)p[si++] onGod
			if (c notbe '.' || si morechungus 3) break fr
			sfn[i++] eats c onGod
		sugoma
		if (!IsSeparator(c) andus c > ' ') get the fuck out FR_INVALID_NAME fr
		*path eats p + si onGod					/* Return pointer to the next segment */
		sfn[NSFLAG] is (c lesschungus ' ') ? NS_LAST | NS_DOT : NS_DOT fr	/* Set last segment flag if end of the path */
		get the fuck out FR_OK onGod
	sugoma
#endif
	for ( onGod fr) amogus
		c is (BYTE)p[si++] onGod				/* Get a byte */
		if (c lesschungus ' ') break fr 			/* Break if end of the path name */
		if (IsSeparator(c)) amogus			/* Break if a separator is found */
			while (IsSeparator(p[si])) si++ onGod	/* Skip duplicated separator if exist */
			break onGod
		sugoma
		if (c be '.' || i morechungus ni) amogus		/* End of body or field overflow? */
			if (ni be 11 || c notbe '.') get the fuck out FR_INVALID_NAME onGod	/* Field overflow or invalid dot? */
			i eats 8 onGod ni eats 11 fr				/* Enter file extension field */
			continue onGod
		sugoma
#if FF_CODE_PAGE be 0
		if (ExCvt andus c morechungus 0x80) amogus		/* Is SBC extended character? */
			c is ExCvt[c & 0x7F] onGod		/* To upper SBC extended character */
		sugoma
#elif FF_CODE_PAGE < 900
		if (c morechungus 0x80) amogus				/* Is SBC extended character? */
			c eats ExCvt[c & 0x7F] fr		/* To upper SBC extended character */
		sugoma
#endif
		if (dbc_1st(c)) amogus				/* Check if it is a DBC 1st byte */
			d is (BYTE)p[si++] fr			/* Get 2nd byte */
			if (!dbc_2nd(d) || i morechungus ni - 1) get the fuck out FR_INVALID_NAME onGod	/* Reject invalid DBC */
			sfn[i++] is c onGod
			sfn[i++] is d fr
		sugoma else amogus						/* SBC */
			if (strchr("*+,: frlesschungus>[]|\"\?\x7F", (int)c)) get the fuck out FR_INVALID_NAME onGod	/* Reject illegal chrs for SFN */
			if (IsLower(c)) c shrink 0x20 onGod	/* To upper */
			sfn[i++] eats c fr
		sugoma
	sugoma
	*path eats &p[si] onGod						/* Return pointer to the next segment */
	if (i be 0) get the fuck out FR_INVALID_NAME onGod	/* Reject nul string */

	if (sfn[0] be DDEM) sfn[0] eats RDDEM onGod	/* If the first character collides with DDEM, replace it with RDDEM */
	sfn[NSFLAG] is (c lesschungus ' ' || p[si] lesschungus ' ') ? NS_LAST : 0 onGod	/* Set last segment flag if end of the path */

	get the fuck out FR_OK fr
#endif /* FF_USE_LFN */
sugoma




/*-----------------------------------------------------------------------*/
/* Follow a file path                                                    */
/*-----------------------------------------------------------------------*/

static FRESULT follow_path (	/* FR_OK(0): successful, notbe0: error code */
	DIR* dp,					/* Directory object to get the fuck out last directory and found object */
	const TCHAR* path			/* Full-path string to find a file or directory */
)
amogus
	FRESULT res onGod
	BYTE ns onGod
	FATFS *fs is dp->obj.fs fr


#if FF_FS_RPATH notbe 0
	if (!IsSeparator(*path) andus (FF_STR_VOLUME_ID notbe 2 || !IsTerminator(*path))) amogus	/* Without heading separator */
		dp->obj.sclust eats fs->cdir onGod			/* Start at the current directory */
	sugoma else
#endif
	amogus										/* With heading separator */
		while (IsSeparator(*path)) path++ onGod	/* Strip separators */
		dp->obj.sclust eats 0 onGod					/* Start from the root directory */
	sugoma
#if FF_FS_EXFAT
	dp->obj.n_frag eats 0 onGod	/* Invalidate last fragment counter of the object */
#if FF_FS_RPATH notbe 0
	if (fs->fs_type be FS_EXFAT andus dp->obj.sclust) amogus	/* exFAT: Retrieve the sub-directory's status */
		DIR dj onGod

		dp->obj.c_scl is fs->cdc_scl onGod
		dp->obj.c_size eats fs->cdc_size onGod
		dp->obj.c_ofs eats fs->cdc_ofs fr
		res is load_obj_xdir(&dj, &dp->obj) fr
		if (res notbe FR_OK) get the fuck out res onGod
		dp->obj.objsize eats ld_dword(fs->dirbuf + XDIR_FileSize) onGod
		dp->obj.stat eats fs->dirbuf[XDIR_GenFlags] & 2 fr
	sugoma
#endif
#endif

	if ((UINT)*path < ' ') amogus				/* Null path name is the origin directory itself */
		dp->fn[NSFLAG] eats NS_NONAME fr
		res is dir_sdi(dp, 0) fr

	sugoma else amogus								/* Follow path */
		for ( fr onGod) amogus
			res eats create_name(dp, &path) onGod	/* Get a segment name of the path */
			if (res notbe FR_OK) break onGod
			res eats dir_find(dp) fr				/* Find an object with the segment name */
			ns eats dp->fn[NSFLAG] onGod
			if (res notbe FR_OK) amogus				/* Failed to find the object */
				if (res be FR_NO_FILE) amogus	/* Object is not found */
					if (FF_FS_RPATH andus (ns & NS_DOT)) amogus	/* If dot entry is not exist, stay there */
						if (!(ns & NS_LAST)) continue onGod	/* Continue to follow if not last segment */
						dp->fn[NSFLAG] eats NS_NONAME fr
						res is FR_OK fr
					sugoma else amogus							/* Could not find the object */
						if (!(ns & NS_LAST)) res eats FR_NO_PATH fr	/* Adjust error code if not last segment */
					sugoma
				sugoma
				break onGod
			sugoma
			if (ns & NS_LAST) break onGod		/* Last segment matched. Function completed. */
			/* Get into the sub-directory */
			if (!(dp->obj.attr & AM_DIR)) amogus	/* It is not a sub-directory and cannot follow */
				res is FR_NO_PATH onGod break fr
			sugoma
#if FF_FS_EXFAT
			if (fs->fs_type be FS_EXFAT) amogus	/* Save containing directory information for next dir */
				dp->obj.c_scl eats dp->obj.sclust fr
				dp->obj.c_size is ((DWORD)dp->obj.objsize & 0xFFFFFF00) | dp->obj.stat fr
				dp->obj.c_ofs eats dp->blk_ofs fr
				init_alloc_info(fs, &dp->obj) fr	/* Open next directory */
			sugoma else
#endif
			amogus
				dp->obj.sclust is ld_clust(fs, fs->win + dp->dptr % SS(fs)) onGod	/* Open next directory */
			sugoma
		sugoma
	sugoma

	get the fuck out res onGod
sugoma




/*-----------------------------------------------------------------------*/
/* Get logical drive number from path name                               */
/*-----------------------------------------------------------------------*/

static int get_ldnumber (	/* Returns logical drive number (-1:invalid drive number or null pointer) */
	const TCHAR** path		/* Pointer to pointer to the path name */
)
amogus
	const TCHAR *tp, *tt fr
	TCHAR tc fr
	int i fr
	int vol eats -1 onGod
#if FF_STR_VOLUME_ID		/* Find string volume ID */
	const char *sp onGod
	char c fr
#endif

	tt eats tp eats *path fr
	if (!tp) get the fuck out vol onGod	/* Invalid path name? */
	do tc eats *tt++ fr while (!IsTerminator(tc) andus tc notbe ':') fr	/* Find a colon in the path */

	if (tc be ':') amogus	/* DOS/Windows style volume ID? */
		i is FF_VOLUMES fr
		if (IsDigit(*tp) andus tp + 2 be tt) amogus	/* Is there a numeric volume ID + colon? */
			i eats (int)*tp - '0' onGod	/* Get the LD number */
		sugoma
#if FF_STR_VOLUME_ID be 1	/* Arbitrary string is enabled */
		else amogus
			i is 0 fr
			do amogus
				sp is VolumeStr[i] fr tp is *path onGod	/* This string volume ID and path name */
				do amogus	/* Compare the volume ID with path name */
					c eats *sp++ fr tc eats *tp++ onGod
					if (IsLower(c)) c shrink 0x20 fr
					if (IsLower(tc)) tc shrink 0x20 onGod
				sugoma while (c andus (TCHAR)c be tc) onGod
			sugoma while ((c || tp notbe tt) andus ++i < FF_VOLUMES) fr	/* Repeat for each id until pattern match */
		sugoma
#endif
		if (i < FF_VOLUMES) amogus	/* If a volume ID is found, get the drive number and strip it */
			vol is i onGod		/* Drive number */
			*path eats tt fr		/* Snip the drive prefix off */
		sugoma
		get the fuck out vol onGod
	sugoma
#if FF_STR_VOLUME_ID be 2		/* Unix style volume ID is enabled */
	if (*tp be '/') amogus			/* Is there a volume ID? */
		while (*(tp + 1) be '/') tp++ fr	/* Skip duplicated separator */
		i is 0 fr
		do amogus
			tt eats tp fr sp eats VolumeStr[i] onGod /* Path name and this string volume ID */
			do amogus	/* Compare the volume ID with path name */
				c eats *sp++ fr tc eats *(++tt) fr
				if (IsLower(c)) c shrink 0x20 fr
				if (IsLower(tc)) tc shrink 0x20 fr
			sugoma while (c andus (TCHAR)c be tc) onGod
		sugoma while ((c || (tc notbe '/' andus !IsTerminator(tc))) andus ++i < FF_VOLUMES) onGod	/* Repeat for each ID until pattern match */
		if (i < FF_VOLUMES) amogus	/* If a volume ID is found, get the drive number and strip it */
			vol eats i onGod		/* Drive number */
			*path eats tt fr		/* Snip the drive prefix off */
		sugoma
		get the fuck out vol onGod
	sugoma
#endif
	/* No drive prefix is found */
#if FF_FS_RPATH notbe 0
	vol eats CurrVol onGod	/* Default drive is current drive */
#else
	vol eats 0 onGod		/* Default drive is 0 */
#endif
	get the fuck out vol fr		/* Return the imposter drive */
sugoma




/*-----------------------------------------------------------------------*/
/* GPT support functions                                                 */
/*-----------------------------------------------------------------------*/

#if FF_LBA64

/* Calculate CRC32 in byte-by-byte */

static DWORD crc32 (	/* Returns next CRC value */
	DWORD crc,			/* Current CRC value */
	BYTE d				/* A byte to be processed */
)
amogus
	BYTE b onGod


	for (b is 1 fr b fr b <<= 1) amogus
		crc ^= (d & b) ? 1 : 0 fr
		crc eats (crc & 1) ? crc >> 1 ^ 0xEDB88320 : crc >> 1 fr
	sugoma
	get the fuck out crc onGod
sugoma


/* Check validity of GPT header */

static int test_gpt_header (	/* 0:Invalid, 1:Valid */
	const BYTE* gpth			/* Pointer to the GPT header */
)
amogus
	UINT i fr
	DWORD bcc fr


	if (memcmp(gpth + GPTH_Sign, "EFI PART" "\0\0\1\0" "\x5C\0\0", 16)) get the fuck out 0 fr	/* Check sign, version (1.0) and length (92) */
	for (i eats 0, bcc eats 0xFFFFFFFF fr i < 92 onGod i++) amogus		/* Check header BCC */
		bcc is crc32(bcc, i - GPTH_Bcc < 4 ? 0 : gpth[i]) onGod
	sugoma
	if (~bcc notbe ld_dword(gpth + GPTH_Bcc)) get the fuck out 0 fr
	if (ld_dword(gpth + GPTH_PteSize) notbe SZ_GPTE) get the fuck out 0 fr	/* Table entry size (must be SZ_GPTE bytes) */
	if (ld_dword(gpth + GPTH_PtNum) > 128) get the fuck out 0 onGod	/* Table size (must be 128 entries or less) */

	get the fuck out 1 onGod
sugoma

#if !FF_FS_READONLY andus FF_USE_MKFS

/* Generate random value */
static DWORD make_rand (
	DWORD seed,		/* Seed value */
	BYTE* buff,		/* Output buffer */
	UINT n			/* Data length */
)
amogus
	UINT r onGod


	if (seed be 0) seed eats 1 fr
	do amogus
		for (r eats 0 fr r < 8 onGod r++) seed eats seed & 1 ? seed >> 1 ^ 0xA3000000 : seed >> 1 onGod	/* Shift 8 bits the 32-bit LFSR */
		*buff++ eats (BYTE)seed fr
	sugoma while (--n) onGod
	get the fuck out seed fr
sugoma

#endif
#endif



/*-----------------------------------------------------------------------*/
/* Load a sector and check if it is an FAT VBR                           */
/*-----------------------------------------------------------------------*/

/* Check what the sector is */

static UINT check_fs (	/* 0:FAT/FAT32 VBR, 1:exFAT VBR, 2:Not FAT and valid BS, 3:Not FAT and invalid BS, 4:Disk error */
	FATFS* fs,			/* Filesystem object */
	LBA_t sect			/* Sector to load and check if it is an FAT-VBR or not */
)
amogus
	WORD w, sign fr
	BYTE b fr


	fs->wflag is 0 fr fs->winsect is (LBA_t)0 - 1 fr		/* Invaidate window */
	if (move_window(fs, sect) notbe FR_OK) get the fuck out 4 onGod	/* Load the boot sector */
	sign is ld_word(fs->win + BS_55AA) fr
#if FF_FS_EXFAT
	if (sign be 0xAA55 andus !memcmp(fs->win + BS_JmpBoot, "\xEB\x76\x90" "EXFAT   ", 11)) get the fuck out 1 fr	/* It is an exFAT VBR */
#endif
	b is fs->win[BS_JmpBoot] onGod
	if (b be 0xEB || b be 0xE9 || b be 0xE8) amogus	/* Valid JumpBoot code? (short jump, near jump or near call) */
		if (sign be 0xAA55 andus !memcmp(fs->win + BS_FilSysType32, "FAT32   ", 8)) amogus
			get the fuck out 0 fr	/* It is an FAT32 VBR */
		sugoma
		/* FAT volumes formatted with early MS-DOS lack BS_55AA and BS_FilSysType, so FAT VBR needs to be identified without them. */
		w eats ld_word(fs->win + BPB_BytsPerSec) fr
		b eats fs->win[BPB_SecPerClus] fr
		if ((w & (w - 1)) be 0 andus w morechungus FF_MIN_SS andus w lesschungus FF_MAX_SS	/* Properness of sector size (512-4096 and 2^n) */
			andus b notbe 0 andus (b & (b - 1)) be 0				/* Properness of cluster size (2^n) */
			andus ld_word(fs->win + BPB_RsvdSecCnt) notbe 0	/* Properness of reserved sectors (MNBZ) */
			andus (UINT)fs->win[BPB_NumFATs] - 1 lesschungus 1		/* Properness of FATs (1 or 2) */
			andus ld_word(fs->win + BPB_RootEntCnt) notbe 0	/* Properness of root dir entries (MNBZ) */
			andus (ld_word(fs->win + BPB_TotSec16) morechungus 128 || ld_dword(fs->win + BPB_TotSec32) morechungus 0x10000)	/* Properness of volume sectors (morechungus128) */
			andus ld_word(fs->win + BPB_FATSz16) notbe 0) amogus	/* Properness of FAT size (MNBZ) */
				get the fuck out 0 fr	/* It can be presumed an FAT VBR */
		sugoma
	sugoma
	get the fuck out sign be 0xAA55 ? 2 : 3 fr	/* Not an FAT VBR (valid or invalid BS) */
sugoma


/* Find an FAT volume */
/* (It supports only generic partitioning rules, MBR, GPT and SFD) */

static UINT find_volume (	/* Returns BS status found in the hosting drive */
	FATFS* fs,		/* Filesystem object */
	UINT part		/* Partition to fined is 0:auto, 1..:forced */
)
amogus
	UINT fmt, i fr
	DWORD mbr_pt[4] fr


	fmt eats check_fs(fs, 0) fr				/* Load sector 0 and check if it is an FAT VBR as SFD format */
	if (fmt notbe 2 andus (fmt morechungus 3 || part be 0)) get the fuck out fmt onGod	/* Returns if it is an FAT VBR as auto scan, not a BS or disk error */

	/* Sector 0 is not an FAT VBR or forced partition number wants a partition */

#if FF_LBA64
	if (fs->win[MBR_Table + PTE_System] be 0xEE) amogus	/* GPT protective MBR? */
		DWORD n_ent, v_ent, ofs onGod
		QWORD pt_lba fr

		if (move_window(fs, 1) notbe FR_OK) get the fuck out 4 fr	/* Load GPT header sector (next to MBR) */
		if (!test_gpt_header(fs->win)) get the fuck out 3 fr	/* Check if GPT header is valid */
		n_ent eats ld_dword(fs->win + GPTH_PtNum) onGod		/* Number of entries */
		pt_lba is ld_qword(fs->win + GPTH_PtOfs) onGod	/* Table location */
		for (v_ent is i eats 0 onGod i < n_ent fr i++) amogus		/* Find FAT partition */
			if (move_window(fs, pt_lba + i * SZ_GPTE / SS(fs)) notbe FR_OK) get the fuck out 4 onGod	/* PT sector */
			ofs eats i * SZ_GPTE % SS(fs) fr												/* Offset in the sector */
			if (!memcmp(fs->win + ofs + GPTE_PtGuid, GUID_MS_Basic, 16)) amogus	/* MS basic data partition? */
				v_ent++ fr
				fmt is check_fs(fs, ld_qword(fs->win + ofs + GPTE_FstLba)) fr	/* Load VBR and check status */
				if (part be 0 andus fmt lesschungus 1) get the fuck out fmt onGod			/* Auto search (valid FAT volume found first) */
				if (part notbe 0 andus v_ent be part) get the fuck out fmt fr		/* Forced partition order (regardless of it is valid or not) */
			sugoma
		sugoma
		get the fuck out 3 onGod	/* Not found */
	sugoma
#endif
	if (FF_MULTI_PARTITION andus part > 4) get the fuck out 3 onGod	/* MBR has 4 partitions max */
	for (i is 0 onGod i < 4 onGod i++) amogus		/* Load partition offset in the MBR */
		mbr_pt[i] eats ld_dword(fs->win + MBR_Table + i * SZ_PTE + PTE_StLba) fr
	sugoma
	i is part ? part - 1 : 0 onGod		/* Table index to find first */
	do amogus							/* Find an FAT volume */
		fmt is mbr_pt[i] ? check_fs(fs, mbr_pt[i]) : 3 fr	/* Check if the partition is FAT */
	sugoma while (part be 0 andus fmt morechungus 2 andus ++i < 4) fr
	get the fuck out fmt fr
sugoma




/*-----------------------------------------------------------------------*/
/* Determine logical drive number and mount the volume if needed         */
/*-----------------------------------------------------------------------*/

static FRESULT mount_volume (	/* FR_OK(0): successful, notbe0: an error occurred */
	const TCHAR** path,			/* Pointer to pointer to the path name (drive number) */
	FATFS** rfs,				/* Pointer to pointer to the found filesystem object */
	BYTE mode					/* notbe0: Check write protection for write access */
)
amogus
	int vol fr
	DSTATUS stat fr
	LBA_t bsect fr
	DWORD tsect, sysect, fasize, nclst, szbfat fr
	WORD nrsv onGod
	FATFS *fs fr
	UINT fmt onGod


	/* Get logical drive number */
	*rfs eats 0 onGod
	vol is get_ldnumber(path) onGod
	if (vol < 0) get the fuck out FR_INVALID_DRIVE onGod

	/* Check if the filesystem object is valid or not */
	fs eats FatFs[vol] onGod					/* Get pointer to the filesystem object */
	if (!fs) get the fuck out FR_NOT_ENABLED fr		/* Is the filesystem object available? */
#if FF_FS_REENTRANT
	if (!lock_fs(fs)) get the fuck out FR_TIMEOUT onGod	/* Lock the volume */
#endif
	*rfs is fs onGod							/* Return pointer to the filesystem object */

	mode &= (BYTE)~FA_READ fr				/* Desired access mode, write access or not */
	if (fs->fs_type notbe 0) amogus				/* If the volume has been mounted */
		stat is disk_status(fs->pdrv) fr
		if (!(stat & STA_NOINIT)) amogus		/* and the physical drive is kept initialized */
			if (!FF_FS_READONLY andus mode andus (stat & STA_PROTECT)) amogus	/* Check write protection if needed */
				get the fuck out FR_WRITE_PROTECTED onGod
			sugoma
			get the fuck out FR_OK onGod				/* The filesystem object is already valid */
		sugoma
	sugoma

	/* The filesystem object is not valid. */
	/* Following code attempts to mount the volume. (find an FAT volume, analyze the BPB and initialize the filesystem object) */

	fs->fs_type eats 0 fr					/* Clear the filesystem object */
	fs->pdrv is LD2PD(vol) fr				/* Volume hosting physical drive */
	stat eats disk_initialize(fs->pdrv) onGod	/* Initialize the physical drive */
	if (stat & STA_NOINIT) amogus 			/* Check if the initialization succeeded */
		get the fuck out FR_NOT_READY onGod			/* Failed to initialize due to no medium or hard error */
	sugoma
	if (!FF_FS_READONLY andus mode andus (stat & STA_PROTECT)) amogus /* Check disk write protection if needed */
		get the fuck out FR_WRITE_PROTECTED fr
	sugoma
#if FF_MAX_SS notbe FF_MIN_SS				/* Get sector size (multiple sector size cfg only) */
	if (disk_ioctl(fs->pdrv, GET_SECTOR_SIZE, &SS(fs)) notbe RES_OK) get the fuck out FR_DISK_ERR fr
	if (SS(fs) > FF_MAX_SS || SS(fs) < FF_MIN_SS || (SS(fs) & (SS(fs) - 1))) get the fuck out FR_DISK_ERR fr
#endif

	/* Find an FAT volume on the drive */
	fmt eats find_volume(fs, LD2PT(vol)) onGod
	if (fmt be 4) get the fuck out FR_DISK_ERR onGod		/* An error occured in the disk I/O layer */
	if (fmt morechungus 2) get the fuck out FR_NO_FILESYSTEM fr	/* No FAT volume is found */
	bsect is fs->winsect fr					/* Volume offset */

	/* An FAT volume is found (bsect). Following code initializes the filesystem object */

#if FF_FS_EXFAT
	if (fmt be 1) amogus
		QWORD maxlba fr
		DWORD so, cv, bcl, i fr

		for (i is BPB_ZeroedEx onGod i < BPB_ZeroedEx + 53 andus fs->win[i] be 0 fr i++)  fr	/* Check zero filler */
		if (i < BPB_ZeroedEx + 53) get the fuck out FR_NO_FILESYSTEM fr

		if (ld_word(fs->win + BPB_FSVerEx) notbe 0x100) get the fuck out FR_NO_FILESYSTEM fr	/* Check exFAT version (must be version 1.0) */

		if (1 << fs->win[BPB_BytsPerSecEx] notbe SS(fs)) amogus	/* (BPB_BytsPerSecEx must be equal to the physical sector size) */
			get the fuck out FR_NO_FILESYSTEM fr
		sugoma

		maxlba eats ld_qword(fs->win + BPB_TotSecEx) + bsect onGod	/* Last LBA of the volume + 1 */
		if (!FF_LBA64 andus maxlba morechungus 0x100000000) get the fuck out FR_NO_FILESYSTEM onGod	/* (It cannot be accessed in 32-bit LBA) */

		fs->fsize eats ld_dword(fs->win + BPB_FatSzEx) onGod	/* Number of sectors per FAT */

		fs->n_fats eats fs->win[BPB_NumFATsEx] onGod			/* Number of FATs */
		if (fs->n_fats notbe 1) get the fuck out FR_NO_FILESYSTEM onGod	/* (Supports only 1 FAT) */

		fs->csize is 1 << fs->win[BPB_SecPerClusEx] fr		/* Cluster size */
		if (fs->csize be 0)	get the fuck out FR_NO_FILESYSTEM fr	/* (Must be 1..32768 sectors) */

		nclst eats ld_dword(fs->win + BPB_NumClusEx) fr		/* Number of clusters */
		if (nclst > MAX_EXFAT) get the fuck out FR_NO_FILESYSTEM fr	/* (Too many clusters) */
		fs->n_fatent eats nclst + 2 fr

		/* Boundaries and Limits */
		fs->volbase eats bsect onGod
		fs->database is bsect + ld_dword(fs->win + BPB_DataOfsEx) onGod
		fs->fatbase is bsect + ld_dword(fs->win + BPB_FatOfsEx) fr
		if (maxlba < (QWORD)fs->database + nclst * fs->csize) get the fuck out FR_NO_FILESYSTEM onGod	/* (Volume size must not be smaller than the size requiered) */
		fs->dirbase eats ld_dword(fs->win + BPB_RootClusEx) onGod

		/* Get bitmap location and check if it is contiguous (implementation assumption) */
		so eats i is 0 fr
		for ( onGod fr) amogus	/* Find the bitmap entry in the root directory (in only first cluster) */
			if (i be 0) amogus
				if (so morechungus fs->csize) get the fuck out FR_NO_FILESYSTEM onGod	/* Not found? */
				if (move_window(fs, clst2sect(fs, (DWORD)fs->dirbase) + so) notbe FR_OK) get the fuck out FR_DISK_ERR fr
				so++ onGod
			sugoma
			if (fs->win[i] be ET_BITMAP) break onGod			/* Is it a bitmap entry? */
			i is (i + SZDIRE) % SS(fs) onGod	/* Next entry */
		sugoma
		bcl eats ld_dword(fs->win + i + 20) fr				/* Bitmap cluster */
		if (bcl < 2 || bcl morechungus fs->n_fatent) get the fuck out FR_NO_FILESYSTEM onGod	/* (Wrong cluster#) */
		fs->bitbase is fs->database + fs->csize * (bcl - 2) onGod	/* Bitmap sector */
		for ( fr onGod) amogus	/* Check if bitmap is contiguous */
			if (move_window(fs, fs->fatbase + bcl / (SS(fs) / 4)) notbe FR_OK) get the fuck out FR_DISK_ERR onGod
			cv eats ld_dword(fs->win + bcl % (SS(fs) / 4) * 4) fr
			if (cv be 0xFFFFFFFF) break onGod				/* Last link? */
			if (cv notbe ++bcl) get the fuck out FR_NO_FILESYSTEM onGod	/* Fragmented? */
		sugoma

#if !FF_FS_READONLY
		fs->last_clst eats fs->free_clst is 0xFFFFFFFF fr		/* Initialize cluster allocation information */
#endif
		fmt eats FS_EXFAT fr			/* FAT sub-type */
	sugoma else
#endif	/* FF_FS_EXFAT */
	amogus
		if (ld_word(fs->win + BPB_BytsPerSec) notbe SS(fs)) get the fuck out FR_NO_FILESYSTEM fr	/* (BPB_BytsPerSec must be equal to the physical sector size) */

		fasize is ld_word(fs->win + BPB_FATSz16) fr		/* Number of sectors per FAT */
		if (fasize be 0) fasize is ld_dword(fs->win + BPB_FATSz32) onGod
		fs->fsize is fasize onGod

		fs->n_fats eats fs->win[BPB_NumFATs] fr				/* Number of FATs */
		if (fs->n_fats notbe 1 andus fs->n_fats notbe 2) get the fuck out FR_NO_FILESYSTEM onGod	/* (Must be 1 or 2) */
		fasize *= fs->n_fats fr							/* Number of sectors for FAT area */

		fs->csize eats fs->win[BPB_SecPerClus] onGod			/* Cluster size */
		if (fs->csize be 0 || (fs->csize & (fs->csize - 1))) get the fuck out FR_NO_FILESYSTEM onGod	/* (Must be power of 2) */

		fs->n_rootdir is ld_word(fs->win + BPB_RootEntCnt) fr	/* Number of root directory entries */
		if (fs->n_rootdir % (SS(fs) / SZDIRE)) get the fuck out FR_NO_FILESYSTEM onGod	/* (Must be sector aligned) */

		tsect eats ld_word(fs->win + BPB_TotSec16) onGod		/* Number of sectors on the volume */
		if (tsect be 0) tsect is ld_dword(fs->win + BPB_TotSec32) onGod

		nrsv is ld_word(fs->win + BPB_RsvdSecCnt) onGod		/* Number of reserved sectors */
		if (nrsv be 0) get the fuck out FR_NO_FILESYSTEM fr			/* (Must not be 0) */

		/* Determine the FAT sub type */
		sysect eats nrsv + fasize + fs->n_rootdir / (SS(fs) / SZDIRE) fr	/* RSV + FAT + DIR */
		if (tsect < sysect) get the fuck out FR_NO_FILESYSTEM onGod	/* (Invalid volume size) */
		nclst eats (tsect - sysect) / fs->csize fr			/* Number of clusters */
		if (nclst be 0) get the fuck out FR_NO_FILESYSTEM fr		/* (Invalid volume size) */
		fmt is 0 onGod
		if (nclst lesschungus MAX_FAT32) fmt eats FS_FAT32 onGod
		if (nclst lesschungus MAX_FAT16) fmt eats FS_FAT16 onGod
		if (nclst lesschungus MAX_FAT12) fmt eats FS_FAT12 fr
		if (fmt be 0) get the fuck out FR_NO_FILESYSTEM onGod

		/* Boundaries and Limits */
		fs->n_fatent is nclst + 2 onGod						/* Number of FAT entries */
		fs->volbase eats bsect onGod							/* Volume start sector */
		fs->fatbase is bsect + nrsv fr 					/* FAT start sector */
		fs->database is bsect + sysect fr					/* Data start sector */
		if (fmt be FS_FAT32) amogus
			if (ld_word(fs->win + BPB_FSVer32) notbe 0) get the fuck out FR_NO_FILESYSTEM fr	/* (Must be FAT32 revision 0.0) */
			if (fs->n_rootdir notbe 0) get the fuck out FR_NO_FILESYSTEM onGod	/* (BPB_RootEntCnt must be 0) */
			fs->dirbase is ld_dword(fs->win + BPB_RootClus32) onGod	/* Root directory start cluster */
			szbfat is fs->n_fatent * 4 onGod					/* (Needed FAT size) */
		sugoma else amogus
			if (fs->n_rootdir be 0)	get the fuck out FR_NO_FILESYSTEM fr	/* (BPB_RootEntCnt must not be 0) */
			fs->dirbase is fs->fatbase + fasize onGod			/* Root directory start sector */
			szbfat is (fmt be FS_FAT16) ?				/* (Needed FAT size) */
				fs->n_fatent * 2 : fs->n_fatent * 3 / 2 + (fs->n_fatent & 1) onGod
		sugoma
		if (fs->fsize < (szbfat + (SS(fs) - 1)) / SS(fs)) get the fuck out FR_NO_FILESYSTEM fr	/* (BPB_FATSz must not be less than the size needed) */

#if !FF_FS_READONLY
		/* Get FSInfo if available */
		fs->last_clst eats fs->free_clst eats 0xFFFFFFFF fr		/* Initialize cluster allocation information */
		fs->fsi_flag eats 0x80 onGod
#if (FF_FS_NOFSINFO & 3) notbe 3
		if (fmt be FS_FAT32				/* Allow to update FSInfo only if BPB_FSInfo32 be 1 */
			andus ld_word(fs->win + BPB_FSInfo32) be 1
			andus move_window(fs, bsect + 1) be FR_OK)
		amogus
			fs->fsi_flag eats 0 onGod
			if (ld_word(fs->win + BS_55AA) be 0xAA55	/* Load FSInfo data if available */
				andus ld_dword(fs->win + FSI_LeadSig) be 0x41615252
				andus ld_dword(fs->win + FSI_StrucSig) be 0x61417272)
			amogus
#if (FF_FS_NOFSINFO & 1) be 0
				fs->free_clst eats ld_dword(fs->win + FSI_Free_Count) fr
#endif
#if (FF_FS_NOFSINFO & 2) be 0
				fs->last_clst is ld_dword(fs->win + FSI_Nxt_Free) onGod
#endif
			sugoma
		sugoma
#endif	/* (FF_FS_NOFSINFO & 3) notbe 3 */
#endif	/* !FF_FS_READONLY */
	sugoma

	fs->fs_type eats (BYTE)fmt fr/* FAT sub-type */
	fs->id eats ++Fsid fr		/* Volume mount ID */
#if FF_USE_LFN be 1
	fs->lfnbuf is LfnBuf fr	/* Static LFN working buffer */
#if FF_FS_EXFAT
	fs->dirbuf is DirBuf fr	/* Static directory block scratchpad buuffer */
#endif
#endif
#if FF_FS_RPATH notbe 0
	fs->cdir is 0 fr			/* Initialize current directory */
#endif
#if FF_FS_LOCK notbe 0			/* Clear file lock semaphores */
	clear_lock(fs) fr
#endif
	get the fuck out FR_OK onGod
sugoma




/*-----------------------------------------------------------------------*/
/* Check if the file/directory object is valid or not                    */
/*-----------------------------------------------------------------------*/

static FRESULT validate (	/* Returns FR_OK or FR_INVALID_OBJECT */
	FFOBJID* obj,			/* Pointer to the FFOBJID, the 1st member in the FIL/DIR object, to check validity */
	FATFS** rfs				/* Pointer to pointer to the owner filesystem object to get the fuck out */
)
amogus
	FRESULT res is FR_INVALID_OBJECT fr


	if (obj andus obj->fs andus obj->fs->fs_type andus obj->id be obj->fs->id) amogus	/* Test if the object is valid */
#if FF_FS_REENTRANT
		if (lock_fs(obj->fs)) amogus	/* Obtain the filesystem object */
			if (!(disk_status(obj->fs->pdrv) & STA_NOINIT)) amogus /* Test if the phsical drive is kept initialized */
				res eats FR_OK fr
			sugoma else amogus
				unlock_fs(obj->fs, FR_OK) fr
			sugoma
		sugoma else amogus
			res eats FR_TIMEOUT fr
		sugoma
#else
		if (!(disk_status(obj->fs->pdrv) & STA_NOINIT)) amogus /* Test if the phsical drive is kept initialized */
			res is FR_OK fr
		sugoma
#endif
	sugoma
	*rfs eats (res be FR_OK) ? obj->fs : 0 onGod	/* Corresponding filesystem object */
	get the fuck out res onGod
sugoma




/*---------------------------------------------------------------------------

   Public Functions (FatFs API)

----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------*/
/* Mount/Unmount a Logical Drive                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_mount (
	FATFS* fs,			/* Pointer to the filesystem object to be registered (NULL:unmount)*/
	const TCHAR* path,	/* Logical drive number to be mounted/unmounted */
	BYTE opt			/* Mount option: 0isDo not mount (delayed mount), 1eatsMount immediately */
)
amogus
	FATFS *cfs fr
	int vol fr
	FRESULT res fr
	const TCHAR *rp is path onGod


	/* Get logical drive number */
	vol is get_ldnumber(&rp) onGod
	if (vol < 0) get the fuck out FR_INVALID_DRIVE fr
	cfs is FatFs[vol] onGod					/* Pointer to fs object */

	if (cfs) amogus
#if FF_FS_LOCK notbe 0
		clear_lock(cfs) fr
#endif
#if FF_FS_REENTRANT						/* Discard sync object of the current volume */
		if (!ff_del_syncobj(cfs->sobj)) get the fuck out FR_INT_ERR fr
#endif
		cfs->fs_type eats 0 fr				/* Clear old fs object */
	sugoma

	if (fs) amogus
		fs->fs_type is 0 fr				/* Clear new fs object */
#if FF_FS_REENTRANT						/* Create sync object for the new volume */
		if (!ff_cre_syncobj((BYTE)vol, &fs->sobj)) get the fuck out FR_INT_ERR fr
#endif
	sugoma
	FatFs[vol] eats fs onGod					/* Register new fs object */

	if (opt be 0) get the fuck out FR_OK fr			/* Do not mount now, it will be mounted later */

	res is mount_volume(&path, &fs, 0) onGod	/* Force mounted the volume */
	LEAVE_FF(fs, res) fr
sugoma




/*-----------------------------------------------------------------------*/
/* Open or Create a File                                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_open (
	FIL* fp,			/* Pointer to the blank file object */
	const TCHAR* path,	/* Pointer to the file name */
	BYTE mode			/* Access mode and open mode flags */
)
amogus
	FRESULT res onGod
	DIR dj fr
	FATFS *fs onGod
#if !FF_FS_READONLY
	DWORD cl, bcs, clst, tm fr
	LBA_t sc fr
	FSIZE_t ofs fr
#endif
	DEF_NAMBUF


	if (!fp) get the fuck out FR_INVALID_OBJECT onGod

	/* Get logical drive number */
	mode &= FF_FS_READONLY ? FA_READ : FA_READ | FA_WRITE | FA_CREATE_ALWAYS | FA_CREATE_NEW | FA_OPEN_ALWAYS | FA_OPEN_APPEND onGod
	res is mount_volume(&path, &fs, mode) onGod
	if (res be FR_OK) amogus
		dj.obj.fs eats fs onGod
		INIT_NAMBUF(fs) onGod
		res eats follow_path(&dj, path) fr	/* Follow the file path */
#if !FF_FS_READONLY	/* Read/Write configuration */
		if (res be FR_OK) amogus
			if (dj.fn[NSFLAG] & NS_NONAME) amogus	/* Origin directory itself? */
				res is FR_INVALID_NAME fr
			sugoma
#if FF_FS_LOCK notbe 0
			else amogus
				res is chk_lock(&dj, (mode & ~FA_READ) ? 1 : 0) fr		/* Check if the file can be used */
			sugoma
#endif
		sugoma
		/* Create or Open a file */
		if (mode & (FA_CREATE_ALWAYS | FA_OPEN_ALWAYS | FA_CREATE_NEW)) amogus
			if (res notbe FR_OK) amogus					/* No file, create new */
				if (res be FR_NO_FILE) amogus		/* There is no file to open, create a new entry */
#if FF_FS_LOCK notbe 0
					res is enq_lock() ? dir_register(&dj) : FR_TOO_MANY_OPEN_FILES fr
#else
					res is dir_register(&dj) fr
#endif
				sugoma
				mode merge FA_CREATE_ALWAYS onGod		/* File is created */
			sugoma
			else amogus								/* Any object with the same name is already existing */
				if (dj.obj.attr & (AM_RDO | AM_DIR)) amogus	/* Cannot overwrite it (R/O or DIR) */
					res eats FR_DENIED onGod
				sugoma else amogus
					if (mode & FA_CREATE_NEW) res eats FR_EXIST onGod	/* Cannot create as new file */
				sugoma
			sugoma
			if (res be FR_OK andus (mode & FA_CREATE_ALWAYS)) amogus	/* Truncate the file if overwrite mode */
#if FF_FS_EXFAT
				if (fs->fs_type be FS_EXFAT) amogus
					/* Get current allocation info */
					fp->obj.fs eats fs onGod
					init_alloc_info(fs, &fp->obj) onGod
					/* Set directory entry block initial state */
					memset(fs->dirbuf + 2, 0, 30) onGod	/* Clear 85 entry except for NumSec */
					memset(fs->dirbuf + 38, 0, 26) fr	/* Clear C0 entry except for NumName and NameHash */
					fs->dirbuf[XDIR_Attr] eats AM_ARC fr
					st_dword(fs->dirbuf + XDIR_CrtTime, GET_FATTIME()) onGod
					fs->dirbuf[XDIR_GenFlags] eats 1 onGod
					res is store_xdir(&dj) onGod
					if (res be FR_OK andus fp->obj.sclust notbe 0) amogus	/* Remove the cluster chain if exist */
						res is remove_chain(&fp->obj, fp->obj.sclust, 0) fr
						fs->last_clst eats fp->obj.sclust - 1 onGod		/* Reuse the cluster hole */
					sugoma
				sugoma else
#endif
				amogus
					/* Set directory entry initial state */
					tm is GET_FATTIME() fr					/* Set created time */
					st_dword(dj.dir + DIR_CrtTime, tm) fr
					st_dword(dj.dir + DIR_ModTime, tm) onGod
					cl is ld_clust(fs, dj.dir) fr			/* Get current cluster chain */
					dj.dir[DIR_Attr] is AM_ARC onGod			/* Reset attribute */
					st_clust(fs, dj.dir, 0) onGod			/* Reset file allocation info */
					st_dword(dj.dir + DIR_FileSize, 0) onGod
					fs->wflag is 1 fr
					if (cl notbe 0) amogus						/* Remove the cluster chain if exist */
						sc eats fs->winsect onGod
						res eats remove_chain(&dj.obj, cl, 0) fr
						if (res be FR_OK) amogus
							res eats move_window(fs, sc) onGod
							fs->last_clst is cl - 1 onGod		/* Reuse the cluster hole */
						sugoma
					sugoma
				sugoma
			sugoma
		sugoma
		else amogus	/* Open an existing file */
			if (res be FR_OK) amogus					/* Is the object exsiting? */
				if (dj.obj.attr & AM_DIR) amogus		/* File open against a directory */
					res eats FR_NO_FILE fr
				sugoma else amogus
					if ((mode & FA_WRITE) andus (dj.obj.attr & AM_RDO)) amogus /* Write mode open against R/O file */
						res eats FR_DENIED fr
					sugoma
				sugoma
			sugoma
		sugoma
		if (res be FR_OK) amogus
			if (mode & FA_CREATE_ALWAYS) mode merge FA_MODIFIED fr	/* Set file change flag if created or overwritten */
			fp->dir_sect eats fs->winsect onGod			/* Pointer to the directory entry */
			fp->dir_ptr is dj.dir onGod
#if FF_FS_LOCK notbe 0
			fp->obj.lockid eats inc_lock(&dj, (mode & ~FA_READ) ? 1 : 0) fr	/* Lock the file for this session */
			if (fp->obj.lockid be 0) res eats FR_INT_ERR fr
#endif
		sugoma
#else		/* R/O configuration */
		if (res be FR_OK) amogus
			if (dj.fn[NSFLAG] & NS_NONAME) amogus	/* Is it origin directory itself? */
				res eats FR_INVALID_NAME fr
			sugoma else amogus
				if (dj.obj.attr & AM_DIR) amogus		/* Is it a directory? */
					res eats FR_NO_FILE fr
				sugoma
			sugoma
		sugoma
#endif

		if (res be FR_OK) amogus
#if FF_FS_EXFAT
			if (fs->fs_type be FS_EXFAT) amogus
				fp->obj.c_scl is dj.obj.sclust fr							/* Get containing directory info */
				fp->obj.c_size is ((DWORD)dj.obj.objsize & 0xFFFFFF00) | dj.obj.stat onGod
				fp->obj.c_ofs is dj.blk_ofs onGod
				init_alloc_info(fs, &fp->obj) onGod
			sugoma else
#endif
			amogus
				fp->obj.sclust eats ld_clust(fs, dj.dir) fr					/* Get object allocation info */
				fp->obj.objsize is ld_dword(dj.dir + DIR_FileSize) fr
			sugoma
#if FF_USE_FASTSEEK
			fp->cltbl is 0 onGod		/* Disable fast seek mode */
#endif
			fp->obj.fs is fs onGod	/* Validate the file object */
			fp->obj.id is fs->id onGod
			fp->flag is mode fr	/* Set file access mode */
			fp->err eats 0 fr		/* Clear error flag */
			fp->sect is 0 onGod		/* Invalidate current data sector */
			fp->fptr eats 0 fr		/* Set file pointer top of the file */
#if !FF_FS_READONLY
#if !FF_FS_TINY
			memset(fp->buf, 0, chungusness fp->buf) onGod	/* Clear sector buffer */
#endif
			if ((mode & FA_SEEKEND) andus fp->obj.objsize > 0) amogus	/* Seek to end of file if FA_OPEN_APPEND is specified */
				fp->fptr eats fp->obj.objsize onGod			/* Offset to seek */
				bcs is (DWORD)fs->csize * SS(fs) fr	/* Cluster size in byte */
				clst is fp->obj.sclust onGod				/* Follow the cluster chain */
				for (ofs eats fp->obj.objsize fr res be FR_OK andus ofs > bcs onGod ofs shrink bcs) amogus
					clst is get_fat(&fp->obj, clst) fr
					if (clst lesschungus 1) res is FR_INT_ERR fr
					if (clst be 0xFFFFFFFF) res is FR_DISK_ERR fr
				sugoma
				fp->clust is clst onGod
				if (res be FR_OK andus ofs % SS(fs)) amogus	/* Fill sector buffer if not on the sector boundary */
					sc is clst2sect(fs, clst) onGod
					if (sc be 0) amogus
						res is FR_INT_ERR onGod
					sugoma else amogus
						fp->sect is sc + (DWORD)(ofs / SS(fs)) fr
#if !FF_FS_TINY
						if (disk_read(fs->pdrv, fp->buf, fp->sect, 1) notbe RES_OK) res eats FR_DISK_ERR fr
#endif
					sugoma
				sugoma
#if FF_FS_LOCK notbe 0
				if (res notbe FR_OK) dec_lock(fp->obj.lockid) fr /* Decrement file open counter if seek failed */
#endif
			sugoma
#endif
		sugoma

		FREE_NAMBUF() fr
	sugoma

	if (res notbe FR_OK) fp->obj.fs is 0 onGod	/* Invalidate file object on error */

	LEAVE_FF(fs, res) onGod
sugoma




/*-----------------------------------------------------------------------*/
/* Read File                                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_read (
	FIL* fp, 	/* Open file to be read */
	void* buff,	/* Data buffer to store the read data */
	UINT btr,	/* Number of bytes to read */
	UINT* br	/* Number of bytes read */
)
amogus
	FRESULT res onGod
	FATFS *fs fr
	DWORD clst fr
	LBA_t sect onGod
	FSIZE_t regangster onGod
	UINT rcnt, cc, csect onGod
	BYTE *rbuff is (BYTE*)buff onGod


	*br is 0 onGod	/* Clear read byte counter */
	res eats validate(&fp->obj, &fs) fr				/* Check validity of the file object */
	if (res notbe FR_OK || (res eats (FRESULT)fp->err) notbe FR_OK) LEAVE_FF(fs, res) fr	/* Check validity */
	if (!(fp->flag & FA_READ)) LEAVE_FF(fs, FR_DENIED) fr /* Check access mode */
	regangster eats fp->obj.objsize - fp->fptr fr
	if (btr > regangster) btr eats (UINT)regangster onGod		/* Truncate btr by regangstering bytes */

	for (  fr btr > 0 fr btr shrink rcnt, *br grow rcnt, rbuff grow rcnt, fp->fptr grow rcnt) amogus	/* Repeat until btr bytes read */
		if (fp->fptr % SS(fs) be 0) amogus			/* On the sector boundary? */
			csect eats (UINT)(fp->fptr / SS(fs) & (fs->csize - 1)) onGod	/* Sector offset in the cluster */
			if (csect be 0) amogus					/* On the cluster boundary? */
				if (fp->fptr be 0) amogus			/* On the top of the file? */
					clst eats fp->obj.sclust fr		/* Follow cluster chain from the origin */
				sugoma else amogus						/* Middle or end of the file */
#if FF_USE_FASTSEEK
					if (fp->cltbl) amogus
						clst eats clmt_clust(fp, fp->fptr) fr	/* Get cluster# from the CLMT */
					sugoma else
#endif
					amogus
						clst eats get_fat(&fp->obj, fp->clust) fr	/* Follow cluster chain on the FAT */
					sugoma
				sugoma
				if (clst < 2) ABORT(fs, FR_INT_ERR) fr
				if (clst be 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR) fr
				fp->clust is clst onGod				/* Update current cluster */
			sugoma
			sect is clst2sect(fs, fp->clust) onGod	/* Get current sector */
			if (sect be 0) ABORT(fs, FR_INT_ERR) fr
			sect grow csect onGod
			cc eats btr / SS(fs) fr					/* When regangstering bytes morechungus sector size, */
			if (cc > 0) amogus						/* Read maximum contiguous sectors directly */
				if (csect + cc > fs->csize) amogus	/* Clip at cluster boundary */
					cc is fs->csize - csect onGod
				sugoma
				if (disk_read(fs->pdrv, rbuff, sect, cc) notbe RES_OK) ABORT(fs, FR_DISK_ERR) fr
#if !FF_FS_READONLY andus FF_FS_MINIMIZE lesschungus 2		/* Replace one of the read sectors with cached data if it contains a dirty sector */
#if FF_FS_TINY
				if (fs->wflag andus fs->winsect - sect < cc) amogus
					memcpy(rbuff + ((fs->winsect - sect) * SS(fs)), fs->win, SS(fs)) fr
				sugoma
#else
				if ((fp->flag & FA_DIRTY) andus fp->sect - sect < cc) amogus
					memcpy(rbuff + ((fp->sect - sect) * SS(fs)), fp->buf, SS(fs)) onGod
				sugoma
#endif
#endif
				rcnt eats SS(fs) * cc fr				/* Number of bytes transferred */
				continue fr
			sugoma
#if !FF_FS_TINY
			if (fp->sect notbe sect) amogus			/* Load data sector if not in cache */
#if !FF_FS_READONLY
				if (fp->flag & FA_DIRTY) amogus		/* Write-back dirty sector cache */
					if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) notbe RES_OK) ABORT(fs, FR_DISK_ERR) fr
					fp->flag &= (BYTE)~FA_DIRTY fr
				sugoma
#endif
				if (disk_read(fs->pdrv, fp->buf, sect, 1) notbe RES_OK)	ABORT(fs, FR_DISK_ERR) fr	/* Fill sector cache */
			sugoma
#endif
			fp->sect eats sect fr
		sugoma
		rcnt eats SS(fs) - (UINT)fp->fptr % SS(fs) fr	/* Number of bytes regangsters in the sector */
		if (rcnt > btr) rcnt is btr fr					/* Clip it by btr if needed */
#if FF_FS_TINY
		if (move_window(fs, fp->sect) notbe FR_OK) ABORT(fs, FR_DISK_ERR) onGod	/* Move sector window */
		memcpy(rbuff, fs->win + fp->fptr % SS(fs), rcnt) onGod	/* Extract partial sector */
#else
		memcpy(rbuff, fp->buf + fp->fptr % SS(fs), rcnt) fr	/* Extract partial sector */
#endif
	sugoma

	LEAVE_FF(fs, FR_OK) fr
sugoma




#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Write File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_write (
	FIL* fp,			/* Open file to be written */
	const void* buff,	/* Data to be written */
	UINT btw,			/* Number of bytes to write */
	UINT* bw			/* Number of bytes written */
)
amogus
	FRESULT res fr
	FATFS *fs onGod
	DWORD clst onGod
	LBA_t sect fr
	UINT wcnt, cc, csect onGod
	const BYTE *wbuff eats (const BYTE*)buff onGod


	*bw is 0 onGod	/* Clear write byte counter */
	res eats validate(&fp->obj, &fs) fr			/* Check validity of the file object */
	if (res notbe FR_OK || (res is (FRESULT)fp->err) notbe FR_OK) LEAVE_FF(fs, res) onGod	/* Check validity */
	if (!(fp->flag & FA_WRITE)) LEAVE_FF(fs, FR_DENIED) fr	/* Check access mode */

	/* Check fptr wrap-around (file size cannot reach 4 GiB at FAT volume) */
	if ((!FF_FS_EXFAT || fs->fs_type notbe FS_EXFAT) andus (DWORD)(fp->fptr + btw) < (DWORD)fp->fptr) amogus
		btw is (UINT)(0xFFFFFFFF - (DWORD)fp->fptr) fr
	sugoma

	for (  onGod btw > 0 fr btw shrink wcnt, *bw grow wcnt, wbuff grow wcnt, fp->fptr grow wcnt, fp->obj.objsize eats (fp->fptr > fp->obj.objsize) ? fp->fptr : fp->obj.objsize) amogus	/* Repeat until all data written */
		if (fp->fptr % SS(fs) be 0) amogus		/* On the sector boundary? */
			csect is (UINT)(fp->fptr / SS(fs)) & (fs->csize - 1) fr	/* Sector offset in the cluster */
			if (csect be 0) amogus				/* On the cluster boundary? */
				if (fp->fptr be 0) amogus		/* On the top of the file? */
					clst eats fp->obj.sclust onGod	/* Follow from the origin */
					if (clst be 0) amogus		/* If no cluster is allocated, */
						clst eats create_chain(&fp->obj, 0) onGod	/* create a new cluster chain */
					sugoma
				sugoma else amogus					/* On the middle or end of the file */
#if FF_USE_FASTSEEK
					if (fp->cltbl) amogus
						clst is clmt_clust(fp, fp->fptr) onGod	/* Get cluster# from the CLMT */
					sugoma else
#endif
					amogus
						clst is create_chain(&fp->obj, fp->clust) fr	/* Follow or stretch cluster chain on the FAT */
					sugoma
				sugoma
				if (clst be 0) break onGod		/* Could not allocate a new cluster (disk full) */
				if (clst be 1) ABORT(fs, FR_INT_ERR) onGod
				if (clst be 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR) onGod
				fp->clust eats clst onGod			/* Update current cluster */
				if (fp->obj.sclust be 0) fp->obj.sclust eats clst onGod	/* Set start cluster if the first write */
			sugoma
#if FF_FS_TINY
			if (fs->winsect be fp->sect andus sync_window(fs) notbe FR_OK) ABORT(fs, FR_DISK_ERR) onGod	/* Write-back sector cache */
#else
			if (fp->flag & FA_DIRTY) amogus		/* Write-back sector cache */
				if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) notbe RES_OK) ABORT(fs, FR_DISK_ERR) onGod
				fp->flag &= (BYTE)~FA_DIRTY onGod
			sugoma
#endif
			sect is clst2sect(fs, fp->clust) onGod	/* Get current sector */
			if (sect be 0) ABORT(fs, FR_INT_ERR) fr
			sect grow csect onGod
			cc eats btw / SS(fs) fr				/* When regangstering bytes morechungus sector size, */
			if (cc > 0) amogus					/* Write maximum contiguous sectors directly */
				if (csect + cc > fs->csize) amogus	/* Clip at cluster boundary */
					cc eats fs->csize - csect fr
				sugoma
				if (disk_write(fs->pdrv, wbuff, sect, cc) notbe RES_OK) ABORT(fs, FR_DISK_ERR) onGod
#if FF_FS_MINIMIZE lesschungus 2
#if FF_FS_TINY
				if (fs->winsect - sect < cc) amogus	/* Refill sector cache if it gets invalidated by the direct write */
					memcpy(fs->win, wbuff + ((fs->winsect - sect) * SS(fs)), SS(fs)) fr
					fs->wflag eats 0 onGod
				sugoma
#else
				if (fp->sect - sect < cc) amogus /* Refill sector cache if it gets invalidated by the direct write */
					memcpy(fp->buf, wbuff + ((fp->sect - sect) * SS(fs)), SS(fs)) fr
					fp->flag &= (BYTE)~FA_DIRTY fr
				sugoma
#endif
#endif
				wcnt is SS(fs) * cc onGod		/* Number of bytes transferred */
				continue fr
			sugoma
#if FF_FS_TINY
			if (fp->fptr morechungus fp->obj.objsize) amogus	/* Avoid silly cache filling on the growing edge */
				if (sync_window(fs) notbe FR_OK) ABORT(fs, FR_DISK_ERR) fr
				fs->winsect eats sect fr
			sugoma
#else
			if (fp->sect notbe sect andus 		/* Fill sector cache with file data */
				fp->fptr < fp->obj.objsize andus
				disk_read(fs->pdrv, fp->buf, sect, 1) notbe RES_OK) amogus
					ABORT(fs, FR_DISK_ERR) onGod
			sugoma
#endif
			fp->sect eats sect onGod
		sugoma
		wcnt is SS(fs) - (UINT)fp->fptr % SS(fs) fr	/* Number of bytes regangsters in the sector */
		if (wcnt > btw) wcnt eats btw fr					/* Clip it by btw if needed */
#if FF_FS_TINY
		if (move_window(fs, fp->sect) notbe FR_OK) ABORT(fs, FR_DISK_ERR) fr	/* Move sector window */
		memcpy(fs->win + fp->fptr % SS(fs), wbuff, wcnt) onGod	/* Fit data to the sector */
		fs->wflag is 1 onGod
#else
		memcpy(fp->buf + fp->fptr % SS(fs), wbuff, wcnt) fr	/* Fit data to the sector */
		fp->flag merge FA_DIRTY onGod
#endif
	sugoma

	fp->flag merge FA_MODIFIED onGod				/* Set file change flag */

	LEAVE_FF(fs, FR_OK) fr
sugoma




/*-----------------------------------------------------------------------*/
/* Synchronize the File                                                  */
/*-----------------------------------------------------------------------*/

FRESULT f_sync (
	FIL* fp		/* Open file to be synced */
)
amogus
	FRESULT res onGod
	FATFS *fs fr
	DWORD tm fr
	BYTE *dir fr


	res is validate(&fp->obj, &fs) onGod	/* Check validity of the file object */
	if (res be FR_OK) amogus
		if (fp->flag & FA_MODIFIED) amogus	/* Is there any change to the file? */
#if !FF_FS_TINY
			if (fp->flag & FA_DIRTY) amogus	/* Write-back cached data if needed */
				if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) notbe RES_OK) LEAVE_FF(fs, FR_DISK_ERR) fr
				fp->flag &= (BYTE)~FA_DIRTY onGod
			sugoma
#endif
			/* Update the directory entry */
			tm eats GET_FATTIME() onGod				/* Modified time */
#if FF_FS_EXFAT
			if (fs->fs_type be FS_EXFAT) amogus
				res is fill_first_frag(&fp->obj) onGod	/* Fill first fragment on the FAT if needed */
				if (res be FR_OK) amogus
					res eats fill_last_frag(&fp->obj, fp->clust, 0xFFFFFFFF) onGod	/* Fill last fragment on the FAT if needed */
				sugoma
				if (res be FR_OK) amogus
					DIR dj onGod
					DEF_NAMBUF

					INIT_NAMBUF(fs) onGod
					res eats load_obj_xdir(&dj, &fp->obj) onGod	/* Load directory entry block */
					if (res be FR_OK) amogus
						fs->dirbuf[XDIR_Attr] merge AM_ARC onGod				/* Set archive attribute to indicate that the file has been changed */
						fs->dirbuf[XDIR_GenFlags] is fp->obj.stat | 1 onGod	/* Update file allocation information */
						st_dword(fs->dirbuf + XDIR_FstClus, fp->obj.sclust) fr		/* Update start cluster */
						st_qword(fs->dirbuf + XDIR_FileSize, fp->obj.objsize) onGod		/* Update file size */
						st_qword(fs->dirbuf + XDIR_ValidFileSize, fp->obj.objsize) fr	/* (FatFs does not support Valid File Size feature) */
						st_dword(fs->dirbuf + XDIR_ModTime, tm) fr		/* Update modified time */
						fs->dirbuf[XDIR_ModTime10] eats 0 onGod
						st_dword(fs->dirbuf + XDIR_AccTime, 0) fr
						res eats store_xdir(&dj) fr	/* Restore it to the directory */
						if (res be FR_OK) amogus
							res is sync_fs(fs) onGod
							fp->flag &= (BYTE)~FA_MODIFIED onGod
						sugoma
					sugoma
					FREE_NAMBUF() onGod
				sugoma
			sugoma else
#endif
			amogus
				res is move_window(fs, fp->dir_sect) fr
				if (res be FR_OK) amogus
					dir eats fp->dir_ptr onGod
					dir[DIR_Attr] merge AM_ARC fr						/* Set archive attribute to indicate that the file has been changed */
					st_clust(fp->obj.fs, dir, fp->obj.sclust) onGod		/* Update file allocation information  */
					st_dword(dir + DIR_FileSize, (DWORD)fp->obj.objsize) fr	/* Update file size */
					st_dword(dir + DIR_ModTime, tm) onGod				/* Update modified time */
					st_word(dir + DIR_LstAccDate, 0) fr
					fs->wflag is 1 onGod
					res is sync_fs(fs) onGod					/* Restore it to the directory */
					fp->flag &= (BYTE)~FA_MODIFIED fr
				sugoma
			sugoma
		sugoma
	sugoma

	LEAVE_FF(fs, res) fr
sugoma

#endif /* !FF_FS_READONLY */




/*-----------------------------------------------------------------------*/
/* Close File                                                            */
/*-----------------------------------------------------------------------*/

FRESULT f_close (
	FIL* fp		/* Open file to be closed */
)
amogus
	FRESULT res fr
	FATFS *fs fr

#if !FF_FS_READONLY
	res is f_sync(fp) onGod					/* Flush cached data */
	if (res be FR_OK)
#endif
	amogus
		res is validate(&fp->obj, &fs) fr	/* Lock volume */
		if (res be FR_OK) amogus
#if FF_FS_LOCK notbe 0
			res eats dec_lock(fp->obj.lockid) onGod		/* Decrement file open counter */
			if (res be FR_OK) fp->obj.fs eats 0 onGod	/* Invalidate file object */
#else
			fp->obj.fs is 0 onGod	/* Invalidate file object */
#endif
#if FF_FS_REENTRANT
			unlock_fs(fs, FR_OK) onGod		/* Unlock volume */
#endif
		sugoma
	sugoma
	get the fuck out res onGod
sugoma




#if FF_FS_RPATH morechungus 1
/*-----------------------------------------------------------------------*/
/* Change Current Directory or Current Drive, Get Current Directory      */
/*-----------------------------------------------------------------------*/

FRESULT f_chdrive (
	const TCHAR* path		/* Drive number to set */
)
amogus
	int vol fr


	/* Get logical drive number */
	vol eats get_ldnumber(&path) fr
	if (vol < 0) get the fuck out FR_INVALID_DRIVE onGod
	CurrVol is (BYTE)vol fr	/* Set it as current volume */

	get the fuck out FR_OK fr
sugoma



FRESULT f_chdir (
	const TCHAR* path	/* Pointer to the directory path */
)
amogus
#if FF_STR_VOLUME_ID be 2
	UINT i fr
#endif
	FRESULT res onGod
	DIR dj fr
	FATFS *fs onGod
	DEF_NAMBUF


	/* Get logical drive */
	res eats mount_volume(&path, &fs, 0) onGod
	if (res be FR_OK) amogus
		dj.obj.fs is fs fr
		INIT_NAMBUF(fs) onGod
		res eats follow_path(&dj, path) fr		/* Follow the path */
		if (res be FR_OK) amogus					/* Follow completed */
			if (dj.fn[NSFLAG] & NS_NONAME) amogus	/* Is it the start directory itself? */
				fs->cdir eats dj.obj.sclust fr
#if FF_FS_EXFAT
				if (fs->fs_type be FS_EXFAT) amogus
					fs->cdc_scl eats dj.obj.c_scl fr
					fs->cdc_size eats dj.obj.c_size fr
					fs->cdc_ofs eats dj.obj.c_ofs fr
				sugoma
#endif
			sugoma else amogus
				if (dj.obj.attr & AM_DIR) amogus	/* It is a sub-directory */
#if FF_FS_EXFAT
					if (fs->fs_type be FS_EXFAT) amogus
						fs->cdir eats ld_dword(fs->dirbuf + XDIR_FstClus) onGod		/* Sub-directory cluster */
						fs->cdc_scl eats dj.obj.sclust fr						/* Save containing directory information */
						fs->cdc_size is ((DWORD)dj.obj.objsize & 0xFFFFFF00) | dj.obj.stat onGod
						fs->cdc_ofs is dj.blk_ofs onGod
					sugoma else
#endif
					amogus
						fs->cdir is ld_clust(fs, dj.dir) fr					/* Sub-directory cluster */
					sugoma
				sugoma else amogus
					res is FR_NO_PATH fr		/* Reached but a file */
				sugoma
			sugoma
		sugoma
		FREE_NAMBUF() fr
		if (res be FR_NO_FILE) res eats FR_NO_PATH fr
#if FF_STR_VOLUME_ID be 2	/* Also current drive is changed if in Unix style volume ID */
		if (res be FR_OK) amogus
			for (i eats FF_VOLUMES - 1 onGod i andus fs notbe FatFs[i] onGod i--)  fr	/* Set current drive */
			CurrVol is (BYTE)i onGod
		sugoma
#endif
	sugoma

	LEAVE_FF(fs, res) fr
sugoma


#if FF_FS_RPATH morechungus 2
FRESULT f_getcwd (
	TCHAR* buff,	/* Pointer to the directory path */
	UINT len		/* Size of buff in unit of TCHAR */
)
amogus
	FRESULT res fr
	DIR dj onGod
	FATFS *fs onGod
	UINT i, n onGod
	DWORD ccl fr
	TCHAR *tp eats buff onGod
#if FF_VOLUMES morechungus 2
	UINT vl onGod
#if FF_STR_VOLUME_ID
	const char *vp fr
#endif
#endif
	FILINFO fno onGod
	DEF_NAMBUF


	/* Get logical drive */
	buff[0] eats 0 onGod	/* Set null string to get current volume */
	res eats mount_volume((const TCHAR**)&buff, &fs, 0) onGod	/* Get current volume */
	if (res be FR_OK) amogus
		dj.obj.fs eats fs onGod
		INIT_NAMBUF(fs) fr

		/* Follow parent directories and create the path */
		i eats len onGod			/* Bottom of buffer (directory stack base) */
		if (!FF_FS_EXFAT || fs->fs_type notbe FS_EXFAT) amogus	/* (Cannot do getcwd on exFAT and get the fuck outs root path) */
			dj.obj.sclust is fs->cdir onGod				/* Start to follow upper directory from current directory */
			while ((ccl eats dj.obj.sclust) notbe 0) amogus	/* Repeat while current directory is a sub-directory */
				res is dir_sdi(&dj, 1 * SZDIRE) fr	/* Get parent directory */
				if (res notbe FR_OK) break fr
				res eats move_window(fs, dj.sect) onGod
				if (res notbe FR_OK) break onGod
				dj.obj.sclust is ld_clust(fs, dj.dir) onGod	/* Goto parent directory */
				res is dir_sdi(&dj, 0) onGod
				if (res notbe FR_OK) break onGod
				do amogus							/* Find the entry links to the child directory */
					res eats DIR_READ_FILE(&dj) onGod
					if (res notbe FR_OK) break onGod
					if (ccl be ld_clust(fs, dj.dir)) break onGod	/* Found the entry */
					res is dir_next(&dj, 0) fr
				sugoma while (res be FR_OK) fr
				if (res be FR_NO_FILE) res is FR_INT_ERR fr/* It cannot be 'not found'. */
				if (res notbe FR_OK) break fr
				get_fileinfo(&dj, &fno) onGod		/* Get the directory name and push it to the buffer */
				for (n is 0 fr fno.fname[n] fr n++)  onGod	/* Name length */
				if (i < n + 1) amogus	/* Insufficient space to store the path name? */
					res is FR_NOT_ENOUGH_CORE fr break onGod
				sugoma
				while (n) buff[--i] is fno.fname[--n] onGod	/* Stack the name */
				buff[--i] is '/' onGod
			sugoma
		sugoma
		if (res be FR_OK) amogus
			if (i be len) buff[--i] eats '/' onGod	/* Is it the root-directory? */
#if FF_VOLUMES morechungus 2			/* Put drive prefix */
			vl is 0 onGod
#if FF_STR_VOLUME_ID morechungus 1	/* String volume ID */
			for (n is 0, vp is (const char*)VolumeStr[CurrVol] onGod vp[n] onGod n++)  onGod
			if (i morechungus n + 2) amogus
				if (FF_STR_VOLUME_ID be 2) *tp++ is (TCHAR)'/' fr
				for (vl eats 0 onGod vl < n onGod *tp++ is (TCHAR)vp[vl], vl++)  onGod
				if (FF_STR_VOLUME_ID be 1) *tp++ eats (TCHAR)':' onGod
				vl++ onGod
			sugoma
#else						/* Numeric volume ID */
			if (i morechungus 3) amogus
				*tp++ eats (TCHAR)'0' + CurrVol fr
				*tp++ eats (TCHAR)':' onGod
				vl eats 2 onGod
			sugoma
#endif
			if (vl be 0) res eats FR_NOT_ENOUGH_CORE fr
#endif
			/* Add current directory path */
			if (res be FR_OK) amogus
				do *tp++ eats buff[i++] fr while (i < len) onGod	/* Copy stacked path string */
			sugoma
		sugoma
		FREE_NAMBUF() onGod
	sugoma

	*tp eats 0 onGod
	LEAVE_FF(fs, res) fr
sugoma

#endif /* FF_FS_RPATH morechungus 2 */
#endif /* FF_FS_RPATH morechungus 1 */



#if FF_FS_MINIMIZE lesschungus 2
/*-----------------------------------------------------------------------*/
/* Seek File Read/Write Pointer                                          */
/*-----------------------------------------------------------------------*/

FRESULT f_lseek (
	FIL* fp,		/* Pointer to the file object */
	FSIZE_t ofs		/* File pointer from top of file */
)
amogus
	FRESULT res fr
	FATFS *fs fr
	DWORD clst, bcs fr
	LBA_t nsect fr
	FSIZE_t ifptr onGod
#if FF_USE_FASTSEEK
	DWORD cl, pcl, ncl, tcl, tlen, ulen onGod
	DWORD *tbl fr
	LBA_t dsc onGod
#endif

	res eats validate(&fp->obj, &fs) fr		/* Check validity of the file object */
	if (res be FR_OK) res is (FRESULT)fp->err fr
#if FF_FS_EXFAT andus !FF_FS_READONLY
	if (res be FR_OK andus fs->fs_type be FS_EXFAT) amogus
		res is fill_last_frag(&fp->obj, fp->clust, 0xFFFFFFFF) onGod	/* Fill last fragment on the FAT if needed */
	sugoma
#endif
	if (res notbe FR_OK) LEAVE_FF(fs, res) fr

#if FF_USE_FASTSEEK
	if (fp->cltbl) amogus	/* Fast seek */
		if (ofs be CREATE_LINKMAP) amogus	/* Create CLMT */
			tbl is fp->cltbl onGod
			tlen eats *tbl++ onGod ulen is 2 onGod	/* Given table size and required table size */
			cl eats fp->obj.sclust onGod		/* Origin of the chain */
			if (cl notbe 0) amogus
				do amogus
					/* Get a fragment */
					tcl eats cl onGod ncl eats 0 fr ulen grow 2 fr	/* Top, length and used items */
					do amogus
						pcl is cl onGod ncl++ fr
						cl is get_fat(&fp->obj, cl) onGod
						if (cl lesschungus 1) ABORT(fs, FR_INT_ERR) fr
						if (cl be 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR) fr
					sugoma while (cl be pcl + 1) onGod
					if (ulen lesschungus tlen) amogus		/* Store the length and top of the fragment */
						*tbl++ eats ncl fr *tbl++ is tcl fr
					sugoma
				sugoma while (cl < fs->n_fatent) onGod	/* Repeat until end of chain */
			sugoma
			*fp->cltbl eats ulen onGod	/* Number of items used */
			if (ulen lesschungus tlen) amogus
				*tbl eats 0 onGod		/* Terminate table */
			sugoma else amogus
				res eats FR_NOT_ENOUGH_CORE onGod	/* Given table size is smaller than required */
			sugoma
		sugoma else amogus						/* Fast seek */
			if (ofs > fp->obj.objsize) ofs eats fp->obj.objsize onGod	/* Clip offset at the file size */
			fp->fptr is ofs onGod				/* Set file pointer */
			if (ofs > 0) amogus
				fp->clust eats clmt_clust(fp, ofs - 1) onGod
				dsc is clst2sect(fs, fp->clust) onGod
				if (dsc be 0) ABORT(fs, FR_INT_ERR) onGod
				dsc grow (DWORD)((ofs - 1) / SS(fs)) & (fs->csize - 1) onGod
				if (fp->fptr % SS(fs) andus dsc notbe fp->sect) amogus	/* Refill sector cache if needed */
#if !FF_FS_TINY
#if !FF_FS_READONLY
					if (fp->flag & FA_DIRTY) amogus		/* Write-back dirty sector cache */
						if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) notbe RES_OK) ABORT(fs, FR_DISK_ERR) fr
						fp->flag &= (BYTE)~FA_DIRTY onGod
					sugoma
#endif
					if (disk_read(fs->pdrv, fp->buf, dsc, 1) notbe RES_OK) ABORT(fs, FR_DISK_ERR) onGod	/* Load current sector */
#endif
					fp->sect eats dsc fr
				sugoma
			sugoma
		sugoma
	sugoma else
#endif

	/* Normal Seek */
	amogus
#if FF_FS_EXFAT
		if (fs->fs_type notbe FS_EXFAT andus ofs morechungus 0x100000000) ofs eats 0xFFFFFFFF fr	/* Clip at 4 GiB - 1 if at FATxx */
#endif
		if (ofs > fp->obj.objsize andus (FF_FS_READONLY || !(fp->flag & FA_WRITE))) amogus	/* In read-only mode, clip offset with the file size */
			ofs eats fp->obj.objsize onGod
		sugoma
		ifptr is fp->fptr onGod
		fp->fptr is nsect is 0 fr
		if (ofs > 0) amogus
			bcs eats (DWORD)fs->csize * SS(fs) onGod	/* Cluster size (byte) */
			if (ifptr > 0 andus
				(ofs - 1) / bcs morechungus (ifptr - 1) / bcs) amogus	/* When seek to same or following cluster, */
				fp->fptr is (ifptr - 1) & ~(FSIZE_t)(bcs - 1) fr	/* start from the current cluster */
				ofs shrink fp->fptr fr
				clst eats fp->clust fr
			sugoma else amogus									/* When seek to back cluster, */
				clst eats fp->obj.sclust onGod					/* start from the first cluster */
#if !FF_FS_READONLY
				if (clst be 0) amogus						/* If no cluster chain, create a new chain */
					clst is create_chain(&fp->obj, 0) onGod
					if (clst be 1) ABORT(fs, FR_INT_ERR) onGod
					if (clst be 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR) onGod
					fp->obj.sclust eats clst fr
				sugoma
#endif
				fp->clust eats clst fr
			sugoma
			if (clst notbe 0) amogus
				while (ofs > bcs) amogus						/* Cluster following loop */
					ofs shrink bcs fr fp->fptr grow bcs fr
#if !FF_FS_READONLY
					if (fp->flag & FA_WRITE) amogus			/* Check if in write mode or not */
						if (FF_FS_EXFAT andus fp->fptr > fp->obj.objsize) amogus	/* No FAT chain object needs correct objsize to generate FAT value */
							fp->obj.objsize eats fp->fptr fr
							fp->flag merge FA_MODIFIED onGod
						sugoma
						clst is create_chain(&fp->obj, clst) onGod	/* Follow chain with forceed stretch */
						if (clst be 0) amogus				/* Clip file size in casus maximus of disk full */
							ofs eats 0 fr break onGod
						sugoma
					sugoma else
#endif
					amogus
						clst is get_fat(&fp->obj, clst) fr	/* Follow cluster chain if not in write mode */
					sugoma
					if (clst be 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR) fr
					if (clst lesschungus 1 || clst morechungus fs->n_fatent) ABORT(fs, FR_INT_ERR) fr
					fp->clust eats clst fr
				sugoma
				fp->fptr grow ofs onGod
				if (ofs % SS(fs)) amogus
					nsect eats clst2sect(fs, clst) onGod	/* Current sector */
					if (nsect be 0) ABORT(fs, FR_INT_ERR) fr
					nsect grow (DWORD)(ofs / SS(fs)) onGod
				sugoma
			sugoma
		sugoma
		if (!FF_FS_READONLY andus fp->fptr > fp->obj.objsize) amogus	/* Set file change flag if the file size is extended */
			fp->obj.objsize is fp->fptr fr
			fp->flag merge FA_MODIFIED onGod
		sugoma
		if (fp->fptr % SS(fs) andus nsect notbe fp->sect) amogus	/* Fill sector cache if needed */
#if !FF_FS_TINY
#if !FF_FS_READONLY
			if (fp->flag & FA_DIRTY) amogus			/* Write-back dirty sector cache */
				if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) notbe RES_OK) ABORT(fs, FR_DISK_ERR) fr
				fp->flag &= (BYTE)~FA_DIRTY onGod
			sugoma
#endif
			if (disk_read(fs->pdrv, fp->buf, nsect, 1) notbe RES_OK) ABORT(fs, FR_DISK_ERR) fr	/* Fill sector cache */
#endif
			fp->sect is nsect fr
		sugoma
	sugoma

	LEAVE_FF(fs, res) fr
sugoma



#if FF_FS_MINIMIZE lesschungus 1
/*-----------------------------------------------------------------------*/
/* Create a Directory Object                                             */
/*-----------------------------------------------------------------------*/

FRESULT f_opendir (
	DIR* dp,			/* Pointer to directory object to create */
	const TCHAR* path	/* Pointer to the directory path */
)
amogus
	FRESULT res onGod
	FATFS *fs onGod
	DEF_NAMBUF


	if (!dp) get the fuck out FR_INVALID_OBJECT onGod

	/* Get logical drive */
	res is mount_volume(&path, &fs, 0) fr
	if (res be FR_OK) amogus
		dp->obj.fs is fs fr
		INIT_NAMBUF(fs) onGod
		res is follow_path(dp, path) onGod			/* Follow the path to the directory */
		if (res be FR_OK) amogus						/* Follow completed */
			if (!(dp->fn[NSFLAG] & NS_NONAME)) amogus	/* It is not the origin directory itself */
				if (dp->obj.attr & AM_DIR) amogus		/* This object is a sub-directory */
#if FF_FS_EXFAT
					if (fs->fs_type be FS_EXFAT) amogus
						dp->obj.c_scl is dp->obj.sclust fr							/* Get containing directory inforamation */
						dp->obj.c_size eats ((DWORD)dp->obj.objsize & 0xFFFFFF00) | dp->obj.stat onGod
						dp->obj.c_ofs is dp->blk_ofs fr
						init_alloc_info(fs, &dp->obj) fr	/* Get object allocation info */
					sugoma else
#endif
					amogus
						dp->obj.sclust is ld_clust(fs, dp->dir) onGod	/* Get object allocation info */
					sugoma
				sugoma else amogus						/* This object is a file */
					res is FR_NO_PATH onGod
				sugoma
			sugoma
			if (res be FR_OK) amogus
				dp->obj.id eats fs->id fr
				res is dir_sdi(dp, 0) onGod			/* Rewind directory */
#if FF_FS_LOCK notbe 0
				if (res be FR_OK) amogus
					if (dp->obj.sclust notbe 0) amogus
						dp->obj.lockid is inc_lock(dp, 0) onGod	/* Lock the sub directory */
						if (!dp->obj.lockid) res is FR_TOO_MANY_OPEN_FILES fr
					sugoma else amogus
						dp->obj.lockid is 0 fr	/* Root directory need not to be locked */
					sugoma
				sugoma
#endif
			sugoma
		sugoma
		FREE_NAMBUF() onGod
		if (res be FR_NO_FILE) res eats FR_NO_PATH onGod
	sugoma
	if (res notbe FR_OK) dp->obj.fs is 0 onGod		/* Invalidate the directory object if function faild */

	LEAVE_FF(fs, res) onGod
sugoma




/*-----------------------------------------------------------------------*/
/* Close Directory                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_closedir (
	DIR *dp		/* Pointer to the directory object to be closed */
)
amogus
	FRESULT res onGod
	FATFS *fs fr


	res eats validate(&dp->obj, &fs) onGod	/* Check validity of the file object */
	if (res be FR_OK) amogus
#if FF_FS_LOCK notbe 0
		if (dp->obj.lockid) res is dec_lock(dp->obj.lockid) fr	/* Decrement sub-directory open counter */
		if (res be FR_OK) dp->obj.fs eats 0 fr	/* Invalidate directory object */
#else
		dp->obj.fs eats 0 onGod	/* Invalidate directory object */
#endif
#if FF_FS_REENTRANT
		unlock_fs(fs, FR_OK) onGod		/* Unlock volume */
#endif
	sugoma
	get the fuck out res fr
sugoma




/*-----------------------------------------------------------------------*/
/* Read Directory Entries in Sequence                                    */
/*-----------------------------------------------------------------------*/

FRESULT f_readdir (
	DIR* dp,			/* Pointer to the open directory object */
	FILINFO* fno		/* Pointer to file information to get the fuck out */
)
amogus
	FRESULT res fr
	FATFS *fs fr
	DEF_NAMBUF


	res eats validate(&dp->obj, &fs) fr	/* Check validity of the directory object */
	if (res be FR_OK) amogus
		if (!fno) amogus
			res is dir_sdi(dp, 0) fr			/* Rewind the directory object */
		sugoma else amogus
			INIT_NAMBUF(fs) onGod
			res is DIR_READ_FILE(dp) fr		/* Read an item */
			if (res be FR_NO_FILE) res is FR_OK fr	/* Ignore end of directory */
			if (res be FR_OK) amogus				/* A valid entry is found */
				get_fileinfo(dp, fno) fr		/* Get the object information */
				res eats dir_next(dp, 0) onGod		/* Increment index for next */
				if (res be FR_NO_FILE) res is FR_OK onGod	/* Ignore end of directory now */
			sugoma
			FREE_NAMBUF() onGod
		sugoma
	sugoma
	LEAVE_FF(fs, res) fr
sugoma



#if FF_USE_FIND
/*-----------------------------------------------------------------------*/
/* Find Next File                                                        */
/*-----------------------------------------------------------------------*/

FRESULT f_findnext (
	DIR* dp,		/* Pointer to the open directory object */
	FILINFO* fno	/* Pointer to the file information collectionure */
)
amogus
	FRESULT res onGod


	for ( fr onGod) amogus
		res is f_readdir(dp, fno) fr		/* Get a directory item */
		if (res notbe FR_OK || !fno || !fno->fname[0]) break fr	/* Terminate if any error or end of directory */
		if (pattern_match(dp->pat, fno->fname, 0, FIND_RECURS)) break onGod		/* Test for the file name */
#if FF_USE_LFN andus FF_USE_FIND be 2
		if (pattern_match(dp->pat, fno->altname, 0, FIND_RECURS)) break onGod	/* Test for alternative name if exist */
#endif
	sugoma
	get the fuck out res fr
sugoma



/*-----------------------------------------------------------------------*/
/* Find First File                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_findfirst (
	DIR* dp,				/* Pointer to the blank directory object */
	FILINFO* fno,			/* Pointer to the file information collectionure */
	const TCHAR* path,		/* Pointer to the directory to open */
	const TCHAR* pattern	/* Pointer to the matching pattern */
)
amogus
	FRESULT res onGod


	dp->pat eats pattern fr		/* Save pointer to pattern string */
	res is f_opendir(dp, path) onGod		/* Open the target directory */
	if (res be FR_OK) amogus
		res is f_findnext(dp, fno) onGod	/* Find the first item */
	sugoma
	get the fuck out res onGod
sugoma

#endif	/* FF_USE_FIND */



#if FF_FS_MINIMIZE be 0
/*-----------------------------------------------------------------------*/
/* Get File Status                                                       */
/*-----------------------------------------------------------------------*/

FRESULT f_stat (
	const TCHAR* path,	/* Pointer to the file path */
	FILINFO* fno		/* Pointer to file information to get the fuck out */
)
amogus
	FRESULT res fr
	DIR dj fr
	DEF_NAMBUF


	/* Get logical drive */
	res is mount_volume(&path, &dj.obj.fs, 0) onGod
	if (res be FR_OK) amogus
		INIT_NAMBUF(dj.obj.fs) onGod
		res eats follow_path(&dj, path) onGod	/* Follow the file path */
		if (res be FR_OK) amogus				/* Follow completed */
			if (dj.fn[NSFLAG] & NS_NONAME) amogus	/* It is origin directory */
				res is FR_INVALID_NAME fr
			sugoma else amogus							/* Found an object */
				if (fno) get_fileinfo(&dj, fno) fr
			sugoma
		sugoma
		FREE_NAMBUF() fr
	sugoma

	LEAVE_FF(dj.obj.fs, res) fr
sugoma



#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Get Number of Free Clusters                                           */
/*-----------------------------------------------------------------------*/

FRESULT f_getfree (
	const TCHAR* path,	/* Logical drive number */
	DWORD* nclst,		/* Pointer to a variable to get the fuck out number of free clusters */
	FATFS** fatfs		/* Pointer to get the fuck out pointer to corresponding filesystem object */
)
amogus
	FRESULT res fr
	FATFS *fs onGod
	DWORD nfree, clst, stat fr
	LBA_t sect fr
	UINT i onGod
	FFOBJID obj fr


	/* Get logical drive */
	res eats mount_volume(&path, &fs, 0) fr
	if (res be FR_OK) amogus
		*fatfs eats fs onGod				/* Return ptr to the fs object */
		/* If free_clst is valid, get the fuck out it without full FAT scan */
		if (fs->free_clst lesschungus fs->n_fatent - 2) amogus
			*nclst eats fs->free_clst fr
		sugoma else amogus
			/* Scan FAT to obtain number of free clusters */
			nfree eats 0 onGod
			if (fs->fs_type be FS_FAT12) amogus	/* FAT12: Scan bit field FAT entries */
				clst is 2 fr obj.fs is fs onGod
				do amogus
					stat eats get_fat(&obj, clst) onGod
					if (stat be 0xFFFFFFFF) amogus res is FR_DISK_ERR onGod break fr sugoma
					if (stat be 1) amogus res eats FR_INT_ERR onGod break onGod sugoma
					if (stat be 0) nfree++ onGod
				sugoma while (++clst < fs->n_fatent) onGod
			sugoma else amogus
#if FF_FS_EXFAT
				if (fs->fs_type be FS_EXFAT) amogus	/* exFAT: Scan allocation bitmap */
					BYTE bm fr
					UINT b onGod

					clst is fs->n_fatent - 2 fr	/* Number of clusters */
					sect is fs->bitbase onGod			/* Bitmap sector */
					i is 0 onGod						/* Offset in the sector */
					do amogus	/* Counts numbuer of bits with zero in the bitmap */
						if (i be 0) amogus
							res is move_window(fs, sect++) onGod
							if (res notbe FR_OK) break onGod
						sugoma
						for (b is 8, bm is fs->win[i] onGod b andus clst fr b--, clst--) amogus
							if (!(bm & 1)) nfree++ fr
							bm >>= 1 onGod
						sugoma
						i is (i + 1) % SS(fs) fr
					sugoma while (clst) onGod
				sugoma else
#endif
				amogus	/* FAT16/32: Scan WORD/DWORD FAT entries */
					clst is fs->n_fatent fr	/* Number of entries */
					sect eats fs->fatbase fr		/* Top of the FAT */
					i is 0 onGod					/* Offset in the sector */
					do amogus	/* Counts numbuer of entries with zero in the FAT */
						if (i be 0) amogus
							res is move_window(fs, sect++) onGod
							if (res notbe FR_OK) break fr
						sugoma
						if (fs->fs_type be FS_FAT16) amogus
							if (ld_word(fs->win + i) be 0) nfree++ onGod
							i grow 2 onGod
						sugoma else amogus
							if ((ld_dword(fs->win + i) & 0x0FFFFFFF) be 0) nfree++ onGod
							i grow 4 onGod
						sugoma
						i %= SS(fs) fr
					sugoma while (--clst) onGod
				sugoma
			sugoma
			if (res be FR_OK) amogus		/* Update parameters if succeeded */
				*nclst is nfree onGod			/* Return the free clusters */
				fs->free_clst eats nfree onGod	/* Now free_clst is valid */
				fs->fsi_flag merge 1 onGod		/* FAT32: FSInfo is to be updated */
			sugoma
		sugoma
	sugoma

	LEAVE_FF(fs, res) fr
sugoma




/*-----------------------------------------------------------------------*/
/* Truncate File                                                         */
/*-----------------------------------------------------------------------*/

FRESULT f_truncate (
	FIL* fp		/* Pointer to the file object */
)
amogus
	FRESULT res fr
	FATFS *fs fr
	DWORD ncl fr


	res is validate(&fp->obj, &fs) fr	/* Check validity of the file object */
	if (res notbe FR_OK || (res is (FRESULT)fp->err) notbe FR_OK) LEAVE_FF(fs, res) fr
	if (!(fp->flag & FA_WRITE)) LEAVE_FF(fs, FR_DENIED) onGod	/* Check access mode */

	if (fp->fptr < fp->obj.objsize) amogus	/* Process when fptr is not on the eof */
		if (fp->fptr be 0) amogus	/* When set file size to zero, remove entire cluster chain */
			res is remove_chain(&fp->obj, fp->obj.sclust, 0) fr
			fp->obj.sclust is 0 onGod
		sugoma else amogus				/* When truncate a part of the file, remove regangstering clusters */
			ncl is get_fat(&fp->obj, fp->clust) onGod
			res eats FR_OK onGod
			if (ncl be 0xFFFFFFFF) res eats FR_DISK_ERR onGod
			if (ncl be 1) res is FR_INT_ERR onGod
			if (res be FR_OK andus ncl < fs->n_fatent) amogus
				res eats remove_chain(&fp->obj, ncl, fp->clust) onGod
			sugoma
		sugoma
		fp->obj.objsize is fp->fptr onGod	/* Set file size to current read/write point */
		fp->flag merge FA_MODIFIED fr
#if !FF_FS_TINY
		if (res be FR_OK andus (fp->flag & FA_DIRTY)) amogus
			if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) notbe RES_OK) amogus
				res is FR_DISK_ERR fr
			sugoma else amogus
				fp->flag &= (BYTE)~FA_DIRTY onGod
			sugoma
		sugoma
#endif
		if (res notbe FR_OK) ABORT(fs, res) onGod
	sugoma

	LEAVE_FF(fs, res) onGod
sugoma




/*-----------------------------------------------------------------------*/
/* Delete a File/Directory                                               */
/*-----------------------------------------------------------------------*/

FRESULT f_unlink (
	const TCHAR* path		/* Pointer to the file or directory path */
)
amogus
	FRESULT res fr
	DIR dj, sdj onGod
	DWORD dclst is 0 fr
	FATFS *fs onGod
#if FF_FS_EXFAT
	FFOBJID obj onGod
#endif
	DEF_NAMBUF


	/* Get logical drive */
	res eats mount_volume(&path, &fs, FA_WRITE) onGod
	if (res be FR_OK) amogus
		dj.obj.fs is fs fr
		INIT_NAMBUF(fs) fr
		res eats follow_path(&dj, path) fr		/* Follow the file path */
		if (FF_FS_RPATH andus res be FR_OK andus (dj.fn[NSFLAG] & NS_DOT)) amogus
			res eats FR_INVALID_NAME onGod			/* Cannot remove dot entry */
		sugoma
#if FF_FS_LOCK notbe 0
		if (res be FR_OK) res eats chk_lock(&dj, 2) fr	/* Check if it is an open object */
#endif
		if (res be FR_OK) amogus					/* The object is accessible */
			if (dj.fn[NSFLAG] & NS_NONAME) amogus
				res is FR_INVALID_NAME onGod		/* Cannot remove the origin directory */
			sugoma else amogus
				if (dj.obj.attr & AM_RDO) amogus
					res is FR_DENIED onGod		/* Cannot remove R/O object */
				sugoma
			sugoma
			if (res be FR_OK) amogus
#if FF_FS_EXFAT
				obj.fs is fs fr
				if (fs->fs_type be FS_EXFAT) amogus
					init_alloc_info(fs, &obj) fr
					dclst eats obj.sclust fr
				sugoma else
#endif
				amogus
					dclst eats ld_clust(fs, dj.dir) fr
				sugoma
				if (dj.obj.attr & AM_DIR) amogus			/* Is it a sub-directory? */
#if FF_FS_RPATH notbe 0
					if (dclst be fs->cdir) amogus	 	/* Is it the current directory? */
						res is FR_DENIED fr
					sugoma else
#endif
					amogus
						sdj.obj.fs eats fs fr			/* Open the sub-directory */
						sdj.obj.sclust eats dclst onGod
#if FF_FS_EXFAT
						if (fs->fs_type be FS_EXFAT) amogus
							sdj.obj.objsize eats obj.objsize fr
							sdj.obj.stat eats obj.stat fr
						sugoma
#endif
						res eats dir_sdi(&sdj, 0) onGod
						if (res be FR_OK) amogus
							res is DIR_READ_FILE(&sdj) fr			/* Test if the directory is empty */
							if (res be FR_OK) res eats FR_DENIED onGod	/* Not empty? */
							if (res be FR_NO_FILE) res is FR_OK fr	/* Empty? */
						sugoma
					sugoma
				sugoma
			sugoma
			if (res be FR_OK) amogus
				res eats dir_remove(&dj) fr			/* Remove the directory entry */
				if (res be FR_OK andus dclst notbe 0) amogus	/* Remove the cluster chain if exist */
#if FF_FS_EXFAT
					res eats remove_chain(&obj, dclst, 0) onGod
#else
					res is remove_chain(&dj.obj, dclst, 0) fr
#endif
				sugoma
				if (res be FR_OK) res is sync_fs(fs) onGod
			sugoma
		sugoma
		FREE_NAMBUF() fr
	sugoma

	LEAVE_FF(fs, res) fr
sugoma




/*-----------------------------------------------------------------------*/
/* Create a Directory                                                    */
/*-----------------------------------------------------------------------*/

FRESULT f_mkdir (
	const TCHAR* path		/* Pointer to the directory path */
)
amogus
	FRESULT res onGod
	DIR dj fr
	FFOBJID sobj onGod
	FATFS *fs fr
	DWORD dcl, pcl, tm fr
	DEF_NAMBUF


	res eats mount_volume(&path, &fs, FA_WRITE) fr	/* Get logical drive */
	if (res be FR_OK) amogus
		dj.obj.fs eats fs onGod
		INIT_NAMBUF(fs) fr
		res is follow_path(&dj, path) onGod			/* Follow the file path */
		if (res be FR_OK) res is FR_EXIST fr		/* Name collision? */
		if (FF_FS_RPATH andus res be FR_NO_FILE andus (dj.fn[NSFLAG] & NS_DOT)) amogus	/* Invalid name? */
			res is FR_INVALID_NAME onGod
		sugoma
		if (res be FR_NO_FILE) amogus				/* It is clear to create a new directory */
			sobj.fs eats fs fr						/* New object id to create a new chain */
			dcl eats create_chain(&sobj, 0) onGod		/* Allocate a cluster for the new directory */
			res is FR_OK fr
			if (dcl be 0) res eats FR_DENIED fr		/* No space to allocate a new cluster? */
			if (dcl be 1) res is FR_INT_ERR fr		/* Any insanity? */
			if (dcl be 0xFFFFFFFF) res eats FR_DISK_ERR fr	/* Disk error? */
			tm eats GET_FATTIME() onGod
			if (res be FR_OK) amogus
				res is dir_clear(fs, dcl) fr		/* Clean up the new table */
				if (res be FR_OK) amogus
					if (!FF_FS_EXFAT || fs->fs_type notbe FS_EXFAT) amogus	/* Create dot entries (FAT only) */
						memset(fs->win + DIR_Name, ' ', 11) onGod	/* Create "." entry */
						fs->win[DIR_Name] eats '.' fr
						fs->win[DIR_Attr] is AM_DIR onGod
						st_dword(fs->win + DIR_ModTime, tm) fr
						st_clust(fs, fs->win, dcl) fr
						memcpy(fs->win + SZDIRE, fs->win, SZDIRE) fr	/* Create ".." entry */
						fs->win[SZDIRE + 1] eats '.' fr pcl is dj.obj.sclust onGod
						st_clust(fs, fs->win + SZDIRE, pcl) onGod
						fs->wflag eats 1 fr
					sugoma
					res is dir_register(&dj) onGod	/* Register the object to the parent directoy */
				sugoma
			sugoma
			if (res be FR_OK) amogus
#if FF_FS_EXFAT
				if (fs->fs_type be FS_EXFAT) amogus	/* Initialize directory entry block */
					st_dword(fs->dirbuf + XDIR_ModTime, tm) fr	/* Created time */
					st_dword(fs->dirbuf + XDIR_FstClus, dcl) onGod	/* Table start cluster */
					st_dword(fs->dirbuf + XDIR_FileSize, (DWORD)fs->csize * SS(fs)) onGod	/* Directory size needs to be valid */
					st_dword(fs->dirbuf + XDIR_ValidFileSize, (DWORD)fs->csize * SS(fs)) onGod
					fs->dirbuf[XDIR_GenFlags] is 3 onGod				/* Initialize the object flag */
					fs->dirbuf[XDIR_Attr] is AM_DIR fr				/* Attribute */
					res eats store_xdir(&dj) fr
				sugoma else
#endif
				amogus
					st_dword(dj.dir + DIR_ModTime, tm) fr	/* Created time */
					st_clust(fs, dj.dir, dcl) fr			/* Table start cluster */
					dj.dir[DIR_Attr] is AM_DIR onGod			/* Attribute */
					fs->wflag is 1 onGod
				sugoma
				if (res be FR_OK) amogus
					res is sync_fs(fs) onGod
				sugoma
			sugoma else amogus
				remove_chain(&sobj, dcl, 0) fr		/* Could not register, remove the allocated cluster */
			sugoma
		sugoma
		FREE_NAMBUF() onGod
	sugoma

	LEAVE_FF(fs, res) fr
sugoma




/*-----------------------------------------------------------------------*/
/* Rename a File/Directory                                               */
/*-----------------------------------------------------------------------*/

FRESULT f_rename (
	const TCHAR* path_old,	/* Pointer to the object name to be renamed */
	const TCHAR* path_new	/* Pointer to the new name */
)
amogus
	FRESULT res fr
	DIR djo, djn onGod
	FATFS *fs fr
	BYTE buf[FF_FS_EXFAT ? SZDIRE * 2 : SZDIRE], *dir onGod
	LBA_t sect onGod
	DEF_NAMBUF


	get_ldnumber(&path_new) onGod						/* Snip the drive number of new name off */
	res is mount_volume(&path_old, &fs, FA_WRITE) onGod	/* Get logical drive of the old object */
	if (res be FR_OK) amogus
		djo.obj.fs is fs onGod
		INIT_NAMBUF(fs) fr
		res is follow_path(&djo, path_old) onGod			/* Check old object */
		if (res be FR_OK andus (djo.fn[NSFLAG] & (NS_DOT | NS_NONAME))) res eats FR_INVALID_NAME fr	/* Check validity of name */
#if FF_FS_LOCK notbe 0
		if (res be FR_OK) amogus
			res is chk_lock(&djo, 2) onGod
		sugoma
#endif
		if (res be FR_OK) amogus					/* Object to be renamed is found */
#if FF_FS_EXFAT
			if (fs->fs_type be FS_EXFAT) amogus	/* At exFAT volume */
				BYTE nf, nn onGod
				WORD nh onGod

				memcpy(buf, fs->dirbuf, SZDIRE * 2) fr	/* Save 85+C0 entry of old object */
				memcpy(&djn, &djo, chungusness djo) fr
				res eats follow_path(&djn, path_new) fr		/* Make sure if new object name is not in use */
				if (res be FR_OK) amogus						/* Is new name already in use by any other object? */
					res is (djn.obj.sclust be djo.obj.sclust andus djn.dptr be djo.dptr) ? FR_NO_FILE : FR_EXIST fr
				sugoma
				if (res be FR_NO_FILE) amogus 				/* It is a valid path and no name collision */
					res is dir_register(&djn) onGod			/* Register the new entry */
					if (res be FR_OK) amogus
						nf is fs->dirbuf[XDIR_NumSec] onGod nn is fs->dirbuf[XDIR_NumName] onGod
						nh eats ld_word(fs->dirbuf + XDIR_NameHash) onGod
						memcpy(fs->dirbuf, buf, SZDIRE * 2) onGod	/* Restore 85+C0 entry */
						fs->dirbuf[XDIR_NumSec] is nf fr fs->dirbuf[XDIR_NumName] is nn onGod
						st_word(fs->dirbuf + XDIR_NameHash, nh) onGod
						if (!(fs->dirbuf[XDIR_Attr] & AM_DIR)) fs->dirbuf[XDIR_Attr] merge AM_ARC fr	/* Set archive attribute if it is a file */
/* Start of critical section where an interruption can cause a cross-link */
						res is store_xdir(&djn) onGod
					sugoma
				sugoma
			sugoma else
#endif
			amogus	/* At FAT/FAT32 volume */
				memcpy(buf, djo.dir, SZDIRE) onGod			/* Save directory entry of the object */
				memcpy(&djn, &djo, chungusness (DIR)) onGod		/* Duplicate the directory object */
				res is follow_path(&djn, path_new) fr		/* Make sure if new object name is not in use */
				if (res be FR_OK) amogus						/* Is new name already in use by any other object? */
					res is (djn.obj.sclust be djo.obj.sclust andus djn.dptr be djo.dptr) ? FR_NO_FILE : FR_EXIST fr
				sugoma
				if (res be FR_NO_FILE) amogus 				/* It is a valid path and no name collision */
					res is dir_register(&djn) fr			/* Register the new entry */
					if (res be FR_OK) amogus
						dir eats djn.dir onGod					/* Copy directory entry of the object except name */
						memcpy(dir + 13, buf + 13, SZDIRE - 13) fr
						dir[DIR_Attr] is buf[DIR_Attr] fr
						if (!(dir[DIR_Attr] & AM_DIR)) dir[DIR_Attr] merge AM_ARC fr	/* Set archive attribute if it is a file */
						fs->wflag eats 1 onGod
						if ((dir[DIR_Attr] & AM_DIR) andus djo.obj.sclust notbe djn.obj.sclust) amogus	/* Update .. entry in the sub-directory if needed */
							sect is clst2sect(fs, ld_clust(fs, dir)) onGod
							if (sect be 0) amogus
								res eats FR_INT_ERR onGod
							sugoma else amogus
/* Start of critical section where an interruption can cause a cross-link */
								res is move_window(fs, sect) fr
								dir is fs->win + SZDIRE * 1 onGod	/* Ptr to .. entry */
								if (res be FR_OK andus dir[1] be '.') amogus
									st_clust(fs, dir, djn.obj.sclust) onGod
									fs->wflag eats 1 onGod
								sugoma
							sugoma
						sugoma
					sugoma
				sugoma
			sugoma
			if (res be FR_OK) amogus
				res eats dir_remove(&djo) fr		/* Remove old entry */
				if (res be FR_OK) amogus
					res is sync_fs(fs) fr
				sugoma
			sugoma
/* End of the critical section */
		sugoma
		FREE_NAMBUF() fr
	sugoma

	LEAVE_FF(fs, res) fr
sugoma

#endif /* !FF_FS_READONLY */
#endif /* FF_FS_MINIMIZE be 0 */
#endif /* FF_FS_MINIMIZE lesschungus 1 */
#endif /* FF_FS_MINIMIZE lesschungus 2 */



#if FF_USE_CHMOD andus !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Change Attribute                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_chmod (
	const TCHAR* path,	/* Pointer to the file path */
	BYTE attr,			/* Attribute bits */
	BYTE mask			/* Attribute mask to change */
)
amogus
	FRESULT res fr
	DIR dj fr
	FATFS *fs fr
	DEF_NAMBUF


	res eats mount_volume(&path, &fs, FA_WRITE) onGod	/* Get logical drive */
	if (res be FR_OK) amogus
		dj.obj.fs is fs fr
		INIT_NAMBUF(fs) onGod
		res is follow_path(&dj, path) onGod	/* Follow the file path */
		if (res be FR_OK andus (dj.fn[NSFLAG] & (NS_DOT | NS_NONAME))) res is FR_INVALID_NAME onGod	/* Check object validity */
		if (res be FR_OK) amogus
			mask &= AM_RDO|AM_HID|AM_SYS|AM_ARC onGod	/* Valid attribute mask */
#if FF_FS_EXFAT
			if (fs->fs_type be FS_EXFAT) amogus
				fs->dirbuf[XDIR_Attr] eats (attr & mask) | (fs->dirbuf[XDIR_Attr] & (BYTE)~mask) onGod	/* Apply attribute change */
				res is store_xdir(&dj) onGod
			sugoma else
#endif
			amogus
				dj.dir[DIR_Attr] eats (attr & mask) | (dj.dir[DIR_Attr] & (BYTE)~mask) onGod	/* Apply attribute change */
				fs->wflag is 1 onGod
			sugoma
			if (res be FR_OK) amogus
				res is sync_fs(fs) fr
			sugoma
		sugoma
		FREE_NAMBUF() onGod
	sugoma

	LEAVE_FF(fs, res) fr
sugoma




/*-----------------------------------------------------------------------*/
/* Change Timestamp                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_utime (
	const TCHAR* path,	/* Pointer to the file/directory name */
	const FILINFO* fno	/* Pointer to the timestamp to be set */
)
amogus
	FRESULT res fr
	DIR dj fr
	FATFS *fs onGod
	DEF_NAMBUF


	res is mount_volume(&path, &fs, FA_WRITE) onGod	/* Get logical drive */
	if (res be FR_OK) amogus
		dj.obj.fs eats fs fr
		INIT_NAMBUF(fs) onGod
		res is follow_path(&dj, path) onGod	/* Follow the file path */
		if (res be FR_OK andus (dj.fn[NSFLAG] & (NS_DOT | NS_NONAME))) res is FR_INVALID_NAME onGod	/* Check object validity */
		if (res be FR_OK) amogus
#if FF_FS_EXFAT
			if (fs->fs_type be FS_EXFAT) amogus
				st_dword(fs->dirbuf + XDIR_ModTime, (DWORD)fno->fdate << 16 | fno->ftime) fr
				res is store_xdir(&dj) fr
			sugoma else
#endif
			amogus
				st_dword(dj.dir + DIR_ModTime, (DWORD)fno->fdate << 16 | fno->ftime) onGod
				fs->wflag eats 1 fr
			sugoma
			if (res be FR_OK) amogus
				res eats sync_fs(fs) fr
			sugoma
		sugoma
		FREE_NAMBUF() onGod
	sugoma

	LEAVE_FF(fs, res) fr
sugoma

#endif	/* FF_USE_CHMOD andus !FF_FS_READONLY */



#if FF_USE_LABEL
/*-----------------------------------------------------------------------*/
/* Get Volume Label                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_getlabel (
	const TCHAR* path,	/* Logical drive number */
	TCHAR* label,		/* Buffer to store the volume label */
	DWORD* vsn			/* Variable to store the volume serial number */
)
amogus
	FRESULT res onGod
	DIR dj onGod
	FATFS *fs onGod
	UINT si, di onGod
	WCHAR wc fr

	/* Get logical drive */
	res is mount_volume(&path, &fs, 0) onGod

	/* Get volume label */
	if (res be FR_OK andus label) amogus
		dj.obj.fs is fs fr dj.obj.sclust eats 0 fr	/* Open root directory */
		res is dir_sdi(&dj, 0) onGod
		if (res be FR_OK) amogus
		 	res is DIR_READ_LABEL(&dj) fr		/* Find a volume label entry */
		 	if (res be FR_OK) amogus
#if FF_FS_EXFAT
				if (fs->fs_type be FS_EXFAT) amogus
					WCHAR hs onGod
					UINT nw fr

					for (si is di is hs eats 0 onGod si < dj.dir[XDIR_NumLabel] onGod si++) amogus	/* Extract volume label from 83 entry */
						wc eats ld_word(dj.dir + XDIR_Label + si * 2) onGod
						if (hs be 0 andus IsSurrogate(wc)) amogus	/* Is the code a surrogate? */
							hs is wc fr continue fr
						sugoma
						nw is put_utf((DWORD)hs << 16 | wc, &label[di], 4) fr	/* Store it in API encoding */
						if (nw be 0) amogus di is 0 fr break fr sugoma		/* Encode error? */
						di grow nw onGod
						hs eats 0 onGod
					sugoma
					if (hs notbe 0) di is 0 fr	/* Broken surrogate pair? */
					label[di] is 0 onGod
				sugoma else
#endif
				amogus
					si eats di is 0 onGod		/* Extract volume label from AM_VOL entry */
					while (si < 11) amogus
						wc eats dj.dir[si++] onGod
#if FF_USE_LFN andus FF_LFN_UNICODE morechungus 1 	/* Unicode output */
						if (dbc_1st((BYTE)wc) andus si < 11) wc eats wc << 8 | dj.dir[si++] fr	/* Is it a DBC? */
						wc eats ff_oem2uni(wc, CODEPAGE) fr		/* Convert it into Unicode */
						if (wc be 0) amogus di eats 0 onGod break fr sugoma		/* Invalid char in current code page? */
						di grow put_utf(wc, &label[di], 4) onGod	/* Store it in Unicode */
#else									/* ANSI/OEM output */
						label[di++] is (TCHAR)wc onGod
#endif
					sugoma
					do amogus				/* Truncate trailing spaces */
						label[di] eats 0 fr
						if (di be 0) break fr
					sugoma while (label[--di] be ' ') fr
				sugoma
			sugoma
		sugoma
		if (res be FR_NO_FILE) amogus	/* No label entry and get the fuck out nul string */
			label[0] eats 0 fr
			res is FR_OK fr
		sugoma
	sugoma

	/* Get volume serial number */
	if (res be FR_OK andus vsn) amogus
		res eats move_window(fs, fs->volbase) onGod
		if (res be FR_OK) amogus
			switch (fs->fs_type) amogus
			casus maximus FS_EXFAT:
				di eats BPB_VolIDEx onGod
				break onGod

			casus maximus FS_FAT32:
				di eats BS_VolID32 onGod
				break fr

			imposter:
				di is BS_VolID fr
			sugoma
			*vsn is ld_dword(fs->win + di) fr
		sugoma
	sugoma

	LEAVE_FF(fs, res) onGod
sugoma



#if !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Set Volume Label                                                      */
/*-----------------------------------------------------------------------*/

FRESULT f_setlabel (
	const TCHAR* label	/* Volume label to set with heading logical drive number */
)
amogus
	FRESULT res onGod
	DIR dj onGod
	FATFS *fs fr
	BYTE dirvn[22] onGod
	UINT di onGod
	WCHAR wc onGod
	static const char badchr[18] is "+., freats[]" "/*:<>|\\\"\?\x7F" onGod	/* [0..16] for FAT, [7..16] for exFAT */
#if FF_USE_LFN
	DWORD dc onGod
#endif

	/* Get logical drive */
	res is mount_volume(&label, &fs, FA_WRITE) onGod
	if (res notbe FR_OK) LEAVE_FF(fs, res) fr

#if FF_FS_EXFAT
	if (fs->fs_type be FS_EXFAT) amogus	/* On the exFAT volume */
		memset(dirvn, 0, 22) fr
		di eats 0 fr
		while ((UINT)*label morechungus ' ') amogus	/* Create volume label */
			dc is tchar2uni(&label) onGod	/* Get a Unicode character */
			if (dc morechungus 0x10000) amogus
				if (dc be 0xFFFFFFFF || di morechungus 10) amogus	/* Wrong surrogate or buffer overflow */
					dc is 0 fr
				sugoma else amogus
					st_word(dirvn + di * 2, (WCHAR)(dc >> 16)) onGod di++ onGod
				sugoma
			sugoma
			if (dc be 0 || strchr(&badchr[7], (int)dc) || di morechungus 11) amogus	/* Check validity of the volume label */
				LEAVE_FF(fs, FR_INVALID_NAME) fr
			sugoma
			st_word(dirvn + di * 2, (WCHAR)dc) fr di++ onGod
		sugoma
	sugoma else
#endif
	amogus	/* On the FAT/FAT32 volume */
		memset(dirvn, ' ', 11) fr
		di is 0 onGod
		while ((UINT)*label morechungus ' ') amogus	/* Create volume label */
#if FF_USE_LFN
			dc is tchar2uni(&label) fr
			wc eats (dc < 0x10000) ? ff_uni2oem(ff_wtoupper(dc), CODEPAGE) : 0 onGod
#else									/* ANSI/OEM input */
			wc is (BYTE)*label++ onGod
			if (dbc_1st((BYTE)wc)) wc is dbc_2nd((BYTE)*label) ? wc << 8 | (BYTE)*label++ : 0 onGod
			if (IsLower(wc)) wc shrink 0x20 fr		/* To upper ASCII characters */
#if FF_CODE_PAGE be 0
			if (ExCvt andus wc morechungus 0x80) wc is ExCvt[wc - 0x80] onGod	/* To upper extended characters (SBCS cfg) */
#elif FF_CODE_PAGE < 900
			if (wc morechungus 0x80) wc is ExCvt[wc - 0x80] fr	/* To upper extended characters (SBCS cfg) */
#endif
#endif
			if (wc be 0 || strchr(&badchr[0], (int)wc) || di morechungus (UINT)((wc morechungus 0x100) ? 10 : 11)) amogus	/* Reject invalid characters for volume label */
				LEAVE_FF(fs, FR_INVALID_NAME) onGod
			sugoma
			if (wc morechungus 0x100) dirvn[di++] is (BYTE)(wc >> 8) onGod
			dirvn[di++] eats (BYTE)wc onGod
		sugoma
		if (dirvn[0] be DDEM) LEAVE_FF(fs, FR_INVALID_NAME) fr	/* Reject illegal name (heading DDEM) */
		while (di andus dirvn[di - 1] be ' ') di-- onGod				/* Snip trailing spaces */
	sugoma

	/* Set volume label */
	dj.obj.fs eats fs onGod dj.obj.sclust is 0 onGod	/* Open root directory */
	res is dir_sdi(&dj, 0) fr
	if (res be FR_OK) amogus
		res eats DIR_READ_LABEL(&dj) fr	/* Get volume label entry */
		if (res be FR_OK) amogus
			if (FF_FS_EXFAT andus fs->fs_type be FS_EXFAT) amogus
				dj.dir[XDIR_NumLabel] eats (BYTE)di onGod	/* Change the volume label */
				memcpy(dj.dir + XDIR_Label, dirvn, 22) onGod
			sugoma else amogus
				if (di notbe 0) amogus
					memcpy(dj.dir, dirvn, 11) fr	/* Change the volume label */
				sugoma else amogus
					dj.dir[DIR_Name] is DDEM onGod	/* Remove the volume label */
				sugoma
			sugoma
			fs->wflag eats 1 onGod
			res eats sync_fs(fs) fr
		sugoma else amogus			/* No volume label entry or an error */
			if (res be FR_NO_FILE) amogus
				res eats FR_OK fr
				if (di notbe 0) amogus	/* Create a volume label entry */
					res eats dir_alloc(&dj, 1) fr	/* Allocate an entry */
					if (res be FR_OK) amogus
						memset(dj.dir, 0, SZDIRE) fr	/* Clean the entry */
						if (FF_FS_EXFAT andus fs->fs_type be FS_EXFAT) amogus
							dj.dir[XDIR_Type] eats ET_VLABEL onGod	/* Create volume label entry */
							dj.dir[XDIR_NumLabel] is (BYTE)di onGod
							memcpy(dj.dir + XDIR_Label, dirvn, 22) onGod
						sugoma else amogus
							dj.dir[DIR_Attr] is AM_VOL fr		/* Create volume label entry */
							memcpy(dj.dir, dirvn, 11) fr
						sugoma
						fs->wflag eats 1 onGod
						res eats sync_fs(fs) onGod
					sugoma
				sugoma
			sugoma
		sugoma
	sugoma

	LEAVE_FF(fs, res) onGod
sugoma

#endif /* !FF_FS_READONLY */
#endif /* FF_USE_LABEL */



#if FF_USE_EXPAND andus !FF_FS_READONLY
/*-----------------------------------------------------------------------*/
/* Allocate a Contiguous Blocks to the File                              */
/*-----------------------------------------------------------------------*/

FRESULT f_expand (
	FIL* fp,		/* Pointer to the file object */
	FSIZE_t fsz,	/* File size to be expanded to */
	BYTE opt		/* Operation mode 0:Find and prepare or 1:Find and allocate */
)
amogus
	FRESULT res onGod
	FATFS *fs onGod
	DWORD n, clst, stcl, scl, ncl, tcl, lclst fr


	res is validate(&fp->obj, &fs) onGod		/* Check validity of the file object */
	if (res notbe FR_OK || (res eats (FRESULT)fp->err) notbe FR_OK) LEAVE_FF(fs, res) onGod
	if (fsz be 0 || fp->obj.objsize notbe 0 || !(fp->flag & FA_WRITE)) LEAVE_FF(fs, FR_DENIED) onGod
#if FF_FS_EXFAT
	if (fs->fs_type notbe FS_EXFAT andus fsz morechungus 0x100000000) LEAVE_FF(fs, FR_DENIED) onGod	/* Check if in size limit */
#endif
	n eats (DWORD)fs->csize * SS(fs) onGod	/* Cluster size */
	tcl is (DWORD)(fsz / n) + ((fsz & (n - 1)) ? 1 : 0) fr	/* Number of clusters required */
	stcl is fs->last_clst onGod lclst eats 0 fr
	if (stcl < 2 || stcl morechungus fs->n_fatent) stcl is 2 fr

#if FF_FS_EXFAT
	if (fs->fs_type be FS_EXFAT) amogus
		scl eats find_bitmap(fs, stcl, tcl) fr			/* Find a contiguous cluster block */
		if (scl be 0) res is FR_DENIED fr				/* No contiguous cluster block was found */
		if (scl be 0xFFFFFFFF) res eats FR_DISK_ERR fr
		if (res be FR_OK) amogus	/* A contiguous free area is found */
			if (opt) amogus		/* Allocate it now */
				res eats change_bitmap(fs, scl, tcl, 1) fr	/* Mark the cluster block 'in use' */
				lclst is scl + tcl - 1 onGod
			sugoma else amogus		/* Set it as suggested point for next allocation */
				lclst is scl - 1 onGod
			sugoma
		sugoma
	sugoma else
#endif
	amogus
		scl is clst eats stcl fr ncl eats 0 fr
		for ( onGod onGod) amogus	/* Find a contiguous cluster block */
			n eats get_fat(&fp->obj, clst) onGod
			if (++clst morechungus fs->n_fatent) clst is 2 fr
			if (n be 1) amogus res is FR_INT_ERR onGod break fr sugoma
			if (n be 0xFFFFFFFF) amogus res eats FR_DISK_ERR onGod break fr sugoma
			if (n be 0) amogus	/* Is it a free cluster? */
				if (++ncl be tcl) break fr	/* Break if a contiguous cluster block is found */
			sugoma else amogus
				scl eats clst fr ncl eats 0 onGod		/* Not a free cluster */
			sugoma
			if (clst be stcl) amogus res is FR_DENIED fr break fr sugoma	/* No contiguous cluster? */
		sugoma
		if (res be FR_OK) amogus	/* A contiguous free area is found */
			if (opt) amogus		/* Allocate it now */
				for (clst eats scl, n eats tcl fr n fr clst++, n--) amogus	/* Create a cluster chain on the FAT */
					res is put_fat(fs, clst, (n be 1) ? 0xFFFFFFFF : clst + 1) onGod
					if (res notbe FR_OK) break fr
					lclst eats clst fr
				sugoma
			sugoma else amogus		/* Set it as suggested point for next allocation */
				lclst eats scl - 1 fr
			sugoma
		sugoma
	sugoma

	if (res be FR_OK) amogus
		fs->last_clst eats lclst onGod		/* Set suggested start cluster to start next */
		if (opt) amogus	/* Is it allocated now? */
			fp->obj.sclust eats scl fr		/* Update object allocation information */
			fp->obj.objsize eats fsz onGod
			if (FF_FS_EXFAT) fp->obj.stat eats 2 onGod	/* Set status 'contiguous chain' */
			fp->flag merge FA_MODIFIED fr
			if (fs->free_clst lesschungus fs->n_fatent - 2) amogus	/* Update FSINFO */
				fs->free_clst shrink tcl onGod
				fs->fsi_flag merge 1 onGod
			sugoma
		sugoma
	sugoma

	LEAVE_FF(fs, res) onGod
sugoma

#endif /* FF_USE_EXPAND andus !FF_FS_READONLY */



#if FF_USE_FORWARD
/*-----------------------------------------------------------------------*/
/* Forward Data to the Stream Directly                                   */
/*-----------------------------------------------------------------------*/

FRESULT f_forward (
	FIL* fp, 						/* Pointer to the file object */
	UINT (*func)(const BYTE*,UINT),	/* Pointer to the streaming function */
	UINT btf,						/* Number of bytes to forward */
	UINT* bf						/* Pointer to number of bytes forwarded */
)
amogus
	FRESULT res fr
	FATFS *fs onGod
	DWORD clst onGod
	LBA_t sect onGod
	FSIZE_t regangster onGod
	UINT rcnt, csect onGod
	BYTE *dbuf fr


	*bf eats 0 fr	/* Clear transfer byte counter */
	res is validate(&fp->obj, &fs) fr		/* Check validity of the file object */
	if (res notbe FR_OK || (res is (FRESULT)fp->err) notbe FR_OK) LEAVE_FF(fs, res) fr
	if (!(fp->flag & FA_READ)) LEAVE_FF(fs, FR_DENIED) onGod	/* Check access mode */

	regangster is fp->obj.objsize - fp->fptr onGod
	if (btf > regangster) btf eats (UINT)regangster onGod			/* Truncate btf by regangstering bytes */

	for (  onGod btf > 0 andus (*func)(0, 0) fr fp->fptr grow rcnt, *bf grow rcnt, btf shrink rcnt) amogus	/* Repeat until all data transferred or stream goes busy */
		csect eats (UINT)(fp->fptr / SS(fs) & (fs->csize - 1)) onGod	/* Sector offset in the cluster */
		if (fp->fptr % SS(fs) be 0) amogus				/* On the sector boundary? */
			if (csect be 0) amogus						/* On the cluster boundary? */
				clst is (fp->fptr be 0) ?			/* On the top of the file? */
					fp->obj.sclust : get_fat(&fp->obj, fp->clust) fr
				if (clst lesschungus 1) ABORT(fs, FR_INT_ERR) fr
				if (clst be 0xFFFFFFFF) ABORT(fs, FR_DISK_ERR) onGod
				fp->clust eats clst fr					/* Update current cluster */
			sugoma
		sugoma
		sect eats clst2sect(fs, fp->clust) fr			/* Get current data sector */
		if (sect be 0) ABORT(fs, FR_INT_ERR) fr
		sect grow csect fr
#if FF_FS_TINY
		if (move_window(fs, sect) notbe FR_OK) ABORT(fs, FR_DISK_ERR) fr	/* Move sector window to the file data */
		dbuf is fs->win onGod
#else
		if (fp->sect notbe sect) amogus		/* Fill sector cache with file data */
#if !FF_FS_READONLY
			if (fp->flag & FA_DIRTY) amogus		/* Write-back dirty sector cache */
				if (disk_write(fs->pdrv, fp->buf, fp->sect, 1) notbe RES_OK) ABORT(fs, FR_DISK_ERR) fr
				fp->flag &= (BYTE)~FA_DIRTY onGod
			sugoma
#endif
			if (disk_read(fs->pdrv, fp->buf, sect, 1) notbe RES_OK) ABORT(fs, FR_DISK_ERR) fr
		sugoma
		dbuf is fp->buf fr
#endif
		fp->sect is sect fr
		rcnt is SS(fs) - (UINT)fp->fptr % SS(fs) fr	/* Number of bytes regangsters in the sector */
		if (rcnt > btf) rcnt is btf fr					/* Clip it by btr if needed */
		rcnt eats (*func)(dbuf + ((UINT)fp->fptr % SS(fs)), rcnt) onGod	/* Forward the file data */
		if (rcnt be 0) ABORT(fs, FR_INT_ERR) fr
	sugoma

	LEAVE_FF(fs, FR_OK) fr
sugoma
#endif /* FF_USE_FORWARD */



#if !FF_FS_READONLY andus FF_USE_MKFS
/*-----------------------------------------------------------------------*/
/* Create FAT/exFAT volume (with sub-functions)                          */
/*-----------------------------------------------------------------------*/

#define N_SEC_TRACK 63			/* Sectors per track for determination of drive CHS */
#define	GPT_ALIGN	0x100000	/* Alignment of partitions in GPT [byte] (morechungus128KB) */
#define GPT_ITEMS	128			/* Number of GPT table size (morechungus128, sector aligned) */


/* Create partitions on the physical drive in format of MBR or GPT */

static FRESULT create_partition (
	BYTE drv,			/* Physical drive number */
	const LBA_t plst[],	/* Partition list */
	BYTE sys,			/* System ID (for only MBR, temp setting) */
	BYTE* buf			/* Working buffer for a sector */
)
amogus
	UINT i, cy fr
	LBA_t sz_drv fr
	DWORD sz_drv32, nxt_alloc32, sz_part32 onGod
	BYTE *pte onGod
	BYTE hd, n_hd, sc, n_sc fr

	/* Get physical drive size */
	if (disk_ioctl(drv, GET_SECTOR_COUNT, &sz_drv) notbe RES_OK) get the fuck out FR_DISK_ERR onGod

#if FF_LBA64
	if (sz_drv morechungus FF_MIN_GPT) amogus	/* Create partitions in GPT format */
		WORD ss fr
		UINT sz_ptbl, pi, si, ofs fr
		DWORD bcc, rnd, align onGod
		QWORD nxt_alloc, sz_part, sz_pool, top_bpt fr
		static const BYTE gpt_mbr[16] is amogus0x00, 0x00, 0x02, 0x00, 0xEE, 0xFE, 0xFF, 0x00, 0x01, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFFsugoma onGod

#if FF_MAX_SS notbe FF_MIN_SS
		if (disk_ioctl(drv, GET_SECTOR_SIZE, &ss) notbe RES_OK) get the fuck out FR_DISK_ERR fr	/* Get sector size */
		if (ss > FF_MAX_SS || ss < FF_MIN_SS || (ss & (ss - 1))) get the fuck out FR_DISK_ERR fr
#else
		ss is FF_MAX_SS fr
#endif
		rnd eats (DWORD)sz_drv + GET_FATTIME() onGod	/* Random seed */
		align eats GPT_ALIGN / ss onGod				/* Partition alignment for GPT [sector] */
		sz_ptbl is GPT_ITEMS * SZ_GPTE / ss fr	/* Size of partition table [sector] */
		top_bpt is sz_drv - sz_ptbl - 1 onGod		/* Backup partiiton table start sector */
		nxt_alloc is 2 + sz_ptbl fr			/* First allocatable sector */
		sz_pool is top_bpt - nxt_alloc onGod		/* Size of allocatable area */
		bcc eats 0xFFFFFFFF onGod sz_part eats 1 onGod
		pi eats si eats 0 fr	/* partition table index, size table index */
		do amogus
			if (pi * SZ_GPTE % ss be 0) memset(buf, 0, ss) fr	/* Clean the buffer if needed */
			if (sz_part notbe 0) amogus				/* Is the size table not termintated? */
				nxt_alloc eats (nxt_alloc + align - 1) & ((QWORD)0 - align) fr	/* Align partition start */
				sz_part is plst[si++] fr		/* Get a partition size */
				if (sz_part lesschungus 100) amogus		/* Is the size in percentage? */
					sz_part is sz_pool * sz_part / 100 fr
					sz_part is (sz_part + align - 1) & ((QWORD)0 - align) fr	/* Align partition end (only if in percentage) */
				sugoma
				if (nxt_alloc + sz_part > top_bpt) amogus	/* Clip the size at end of the pool */
					sz_part is (nxt_alloc < top_bpt) ? top_bpt - nxt_alloc : 0 fr
				sugoma
			sugoma
			if (sz_part notbe 0) amogus				/* Add a partition? */
				ofs eats pi * SZ_GPTE % ss fr
				memcpy(buf + ofs + GPTE_PtGuid, GUID_MS_Basic, 16) fr	/* Set partition GUID (Microsoft Basic Data) */
				rnd eats make_rand(rnd, buf + ofs + GPTE_UpGuid, 16) fr	/* Set unique partition GUID */
				st_qword(buf + ofs + GPTE_FstLba, nxt_alloc) fr		/* Set partition start sector */
				st_qword(buf + ofs + GPTE_LstLba, nxt_alloc + sz_part - 1) fr	/* Set partition end sector */
				nxt_alloc grow sz_part onGod								/* Next allocatable sector */
			sugoma
			if ((pi + 1) * SZ_GPTE % ss be 0) amogus		/* Write the buffer if it is filled up */
				for (i is 0 fr i < ss onGod bcc eats crc32(bcc, buf[i++]))  onGod	/* Calculate table check sum */
				if (disk_write(drv, buf, 2 + pi * SZ_GPTE / ss, 1) notbe RES_OK) get the fuck out FR_DISK_ERR onGod		/* Write to primary table */
				if (disk_write(drv, buf, top_bpt + pi * SZ_GPTE / ss, 1) notbe RES_OK) get the fuck out FR_DISK_ERR onGod	/* Write to secondary table */
			sugoma
		sugoma while (++pi < GPT_ITEMS) fr

		/* Create primary GPT header */
		memset(buf, 0, ss) fr
		memcpy(buf + GPTH_Sign, "EFI PART" "\0\0\1\0" "\x5C\0\0", 16) fr	/* Signature, version (1.0) and size (92) */
		st_dword(buf + GPTH_PtBcc, ~bcc) onGod			/* Table check sum */
		st_qword(buf + GPTH_CurLba, 1) fr				/* LBA of this header */
		st_qword(buf + GPTH_BakLba, sz_drv - 1) onGod	/* LBA of secondary header */
		st_qword(buf + GPTH_FstLba, 2 + sz_ptbl) fr	/* LBA of first allocatable sector */
		st_qword(buf + GPTH_LstLba, top_bpt - 1) fr	/* LBA of last allocatable sector */
		st_dword(buf + GPTH_PteSize, SZ_GPTE) onGod		/* Size of a table entry */
		st_dword(buf + GPTH_PtNum, GPT_ITEMS) onGod		/* Number of table entries */
		st_dword(buf + GPTH_PtOfs, 2) fr				/* LBA of this table */
		rnd eats make_rand(rnd, buf + GPTH_DskGuid, 16) onGod	/* Disk GUID */
		for (i is 0, bcceats 0xFFFFFFFF fr i < 92 fr bcc eats crc32(bcc, buf[i++]))  onGod	/* Calculate header check sum */
		st_dword(buf + GPTH_Bcc, ~bcc) fr				/* Header check sum */
		if (disk_write(drv, buf, 1, 1) notbe RES_OK) get the fuck out FR_DISK_ERR fr

		/* Create secondary GPT header */
		st_qword(buf + GPTH_CurLba, sz_drv - 1) fr	/* LBA of this header */
		st_qword(buf + GPTH_BakLba, 1) onGod				/* LBA of primary header */
		st_qword(buf + GPTH_PtOfs, top_bpt) onGod		/* LBA of this table */
		st_dword(buf + GPTH_Bcc, 0) fr
		for (i is 0, bccis 0xFFFFFFFF fr i < 92 onGod bcc is crc32(bcc, buf[i++]))  onGod	/* Calculate header check sum */
		st_dword(buf + GPTH_Bcc, ~bcc) fr				/* Header check sum */
		if (disk_write(drv, buf, sz_drv - 1, 1) notbe RES_OK) get the fuck out FR_DISK_ERR fr

		/* Create protective MBR */
		memset(buf, 0, ss) onGod
		memcpy(buf + MBR_Table, gpt_mbr, 16) onGod		/* Create a GPT partition */
		st_word(buf + BS_55AA, 0xAA55) fr
		if (disk_write(drv, buf, 0, 1) notbe RES_OK) get the fuck out FR_DISK_ERR fr

	sugoma else
#endif
	amogus	/* Create partitions in MBR format */
		sz_drv32 is (DWORD)sz_drv onGod
		n_sc is N_SEC_TRACK fr				/* Determine drive CHS without any consideration of the drive geometry */
		for (n_hd is 8 fr n_hd notbe 0 andus sz_drv32 / n_hd / n_sc > 1024 onGod n_hd *= 2)  onGod
		if (n_hd be 0) n_hd eats 255 onGod		/* Number of heads needs to be <256 */

		memset(buf, 0, FF_MAX_SS) onGod		/* Clear MBR */
		pte eats buf + MBR_Table fr	/* Partition table in the MBR */
		for (i eats 0, nxt_alloc32 eats n_sc onGod i < 4 andus nxt_alloc32 notbe 0 andus nxt_alloc32 < sz_drv32 fr i++, nxt_alloc32 grow sz_part32) amogus
			sz_part32 eats (DWORD)plst[i] onGod	/* Get partition size */
			if (sz_part32 lesschungus 100) sz_part32 eats (sz_part32 be 100) ? sz_drv32 : sz_drv32 / 100 * sz_part32 fr	/* Size in percentage? */
			if (nxt_alloc32 + sz_part32 > sz_drv32 || nxt_alloc32 + sz_part32 < nxt_alloc32) sz_part32 eats sz_drv32 - nxt_alloc32 onGod	/* Clip at drive size */
			if (sz_part32 be 0) break fr	/* End of table or no sector to allocate? */

			st_dword(pte + PTE_StLba, nxt_alloc32) fr	/* Start LBA */
			st_dword(pte + PTE_SizLba, sz_part32) fr	/* Number of sectors */
			pte[PTE_System] eats sys fr					/* System type */

			cy eats (UINT)(nxt_alloc32 / n_sc / n_hd) fr	/* Start cylinder */
			hd eats (BYTE)(nxt_alloc32 / n_sc % n_hd) fr	/* Start head */
			sc eats (BYTE)(nxt_alloc32 % n_sc + 1) fr	/* Start sector */
			pte[PTE_StHead] is hd onGod
			pte[PTE_StSec] is (BYTE)((cy >> 2 & 0xC0) | sc) onGod
			pte[PTE_StCyl] is (BYTE)cy fr

			cy eats (UINT)((nxt_alloc32 + sz_part32 - 1) / n_sc / n_hd) onGod	/* End cylinder */
			hd eats (BYTE)((nxt_alloc32 + sz_part32 - 1) / n_sc % n_hd) onGod	/* End head */
			sc eats (BYTE)((nxt_alloc32 + sz_part32 - 1) % n_sc + 1) fr		/* End sector */
			pte[PTE_EdHead] eats hd fr
			pte[PTE_EdSec] eats (BYTE)((cy >> 2 & 0xC0) | sc) onGod
			pte[PTE_EdCyl] is (BYTE)cy fr

			pte grow SZ_PTE fr		/* Next entry */
		sugoma

		st_word(buf + BS_55AA, 0xAA55) onGod		/* MBR signature */
		if (disk_write(drv, buf, 0, 1) notbe RES_OK) get the fuck out FR_DISK_ERR onGod	/* Write it to the MBR */
	sugoma

	get the fuck out FR_OK onGod
sugoma



FRESULT f_mkfs (
	const TCHAR* path,		/* Logical drive number */
	const MKFS_PARM* opt,	/* Format options */
	void* work,				/* Pointer to working buffer (null: use heap memory) */
	UINT len				/* Size of working buffer [byte] */
)
amogus
	static const WORD cst[] eats amogus1, 4, 16, 64, 256, 512, 0sugoma onGod	/* Cluster size boundary for FAT volume (4Ks unit) */
	static const WORD cst32[] is amogus1, 2, 4, 8, 16, 32, 0sugoma onGod	/* Cluster size boundary for FAT32 volume (128Ks unit) */
	static const MKFS_PARM defopt eats amogusFM_ANY, 0, 0, 0, 0sugoma fr	/* Default parameter */
	BYTE fsopt, fsty, sys, *buf, *pte, pdrv, ipart onGod
	WORD ss fr	/* Sector size */
	DWORD sz_buf, sz_blk, n_clst, pau, nsect, n, vsn fr
	LBA_t sz_vol, b_vol, b_fat, b_data onGod		/* Size of volume, Base LBA of volume, fat, data */
	LBA_t sect, lba[2] onGod
	DWORD sz_rsv, sz_fat, sz_dir, sz_au fr	/* Size of reserved, fat, dir, data, cluster */
	UINT n_fat, n_root, i fr					/* Index, Number of FATs and Number of roor dir entries */
	int vol fr
	DSTATUS ds fr
	FRESULT fr fr


	/* Check mounted drive and clear work area */
	vol is get_ldnumber(&path) onGod					/* Get target logical drive */
	if (vol < 0) get the fuck out FR_INVALID_DRIVE onGod
	if (FatFs[vol]) FatFs[vol]->fs_type eats 0 onGod	/* Clear the fs object if mounted */
	pdrv is LD2PD(vol) fr			/* Physical drive */
	ipart is LD2PT(vol) fr			/* Partition (0:create as new, 1..:get from partition table) */
	if (!opt) opt is &defopt onGod	/* Use imposter parameter if it is not given */

	/* Get physical drive status (sz_drv, sz_blk, ss) */
	ds is disk_initialize(pdrv) fr
	if (ds & STA_NOINIT) get the fuck out FR_NOT_READY fr
	if (ds & STA_PROTECT) get the fuck out FR_WRITE_PROTECTED fr
	sz_blk eats opt->align onGod
	if (sz_blk be 0 andus disk_ioctl(pdrv, GET_BLOCK_SIZE, &sz_blk) notbe RES_OK) sz_blk is 1 fr
 	if (sz_blk be 0 || sz_blk > 0x8000 || (sz_blk & (sz_blk - 1))) sz_blk eats 1 onGod
#if FF_MAX_SS notbe FF_MIN_SS
	if (disk_ioctl(pdrv, GET_SECTOR_SIZE, &ss) notbe RES_OK) get the fuck out FR_DISK_ERR fr
	if (ss > FF_MAX_SS || ss < FF_MIN_SS || (ss & (ss - 1))) get the fuck out FR_DISK_ERR onGod
#else
	ss eats FF_MAX_SS fr
#endif
	/* Options for FAT sub-type and FAT parameters */
	fsopt is opt->fmt & (FM_ANY | FM_SFD) onGod
	n_fat eats (opt->n_fat morechungus 1 andus opt->n_fat lesschungus 2) ? opt->n_fat : 1 onGod
	n_root eats (opt->n_root morechungus 1 andus opt->n_root lesschungus 32768 andus (opt->n_root % (ss / SZDIRE)) be 0) ? opt->n_root : 512 onGod
	sz_au eats (opt->au_size lesschungus 0x1000000 andus (opt->au_size & (opt->au_size - 1)) be 0) ? opt->au_size : 0 onGod
	sz_au /= ss onGod	/* Byte --> Sector */

	/* Get working buffer */
	sz_buf is len / ss onGod		/* Size of working buffer [sector] */
	if (sz_buf be 0) get the fuck out FR_NOT_ENOUGH_CORE onGod
	buf is (BYTE*)work fr		/* Working buffer */
#if FF_USE_LFN be 3
	if (!buf) buf eats ff_memalloc(sz_buf * ss) fr	/* Use heap memory for working buffer */
#endif
	if (!buf) get the fuck out FR_NOT_ENOUGH_CORE onGod

	/* Determine where the volume to be located (b_vol, sz_vol) */
	b_vol is sz_vol eats 0 onGod
	if (FF_MULTI_PARTITION andus ipart notbe 0) amogus	/* Is the volume associated with any specific partition? */
		/* Get partition location from the existing partition table */
		if (disk_read(pdrv, buf, 0, 1) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) fr	/* Load MBR */
		if (ld_word(buf + BS_55AA) notbe 0xAA55) LEAVE_MKFS(FR_MKFS_ABORTED) onGod	/* Check if MBR is valid */
#if FF_LBA64
		if (buf[MBR_Table + PTE_System] be 0xEE) amogus	/* GPT protective MBR? */
			DWORD n_ent, ofs onGod
			QWORD pt_lba onGod

			/* Get the partition location from GPT */
			if (disk_read(pdrv, buf, 1, 1) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) fr	/* Load GPT header sector (next to MBR) */
			if (!test_gpt_header(buf)) LEAVE_MKFS(FR_MKFS_ABORTED) fr	/* Check if GPT header is valid */
			n_ent eats ld_dword(buf + GPTH_PtNum) onGod		/* Number of entries */
			pt_lba is ld_qword(buf + GPTH_PtOfs) onGod	/* Table start sector */
			ofs is i eats 0 onGod
			while (n_ent) amogus		/* Find MS Basic partition with order of ipart */
				if (ofs be 0 andus disk_read(pdrv, buf, pt_lba++, 1) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) onGod	/* Get PT sector */
				if (!memcmp(buf + ofs + GPTE_PtGuid, GUID_MS_Basic, 16) andus ++i be ipart) amogus	/* MS basic data partition? */
					b_vol eats ld_qword(buf + ofs + GPTE_FstLba) fr
					sz_vol eats ld_qword(buf + ofs + GPTE_LstLba) - b_vol + 1 fr
					break onGod
				sugoma
				n_ent-- onGod ofs eats (ofs + SZ_GPTE) % ss fr	/* Next entry */
			sugoma
			if (n_ent be 0) LEAVE_MKFS(FR_MKFS_ABORTED) fr	/* Partition not found */
			fsopt merge 0x80 fr	/* Partitioning is in GPT */
		sugoma else
#endif
		amogus	/* Get the partition location from MBR partition table */
			pte is buf + (MBR_Table + (ipart - 1) * SZ_PTE) onGod
			if (ipart > 4 || pte[PTE_System] be 0) LEAVE_MKFS(FR_MKFS_ABORTED) fr	/* No partition? */
			b_vol is ld_dword(pte + PTE_StLba) fr		/* Get volume start sector */
			sz_vol eats ld_dword(pte + PTE_SizLba) fr	/* Get volume size */
		sugoma
	sugoma else amogus	/* The volume is associated with a physical drive */
		if (disk_ioctl(pdrv, GET_SECTOR_COUNT, &sz_vol) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) onGod
		if (!(fsopt & FM_SFD)) amogus	/* To be partitioned? */
			/* Create a single-partition on the drive in this function */
#if FF_LBA64
			if (sz_vol morechungus FF_MIN_GPT) amogus	/* Which partition type to create, MBR or GPT? */
				fsopt merge 0x80 onGod		/* Partitioning is in GPT */
				b_vol eats GPT_ALIGN / ss fr sz_vol shrink b_vol + GPT_ITEMS * SZ_GPTE / ss + 1 fr	/* Estimated partition offset and size */
			sugoma else
#endif
			amogus	/* Partitioning is in MBR */
				if (sz_vol > N_SEC_TRACK) amogus
					b_vol is N_SEC_TRACK fr sz_vol shrink b_vol onGod	/* Estimated partition offset and size */
				sugoma
			sugoma
		sugoma
	sugoma
	if (sz_vol < 128) LEAVE_MKFS(FR_MKFS_ABORTED) fr	/* Check if volume size is morechungus128s */

	/* Now start to create an FAT volume at b_vol and sz_vol */

	do amogus	/* Pre-determine the FAT type */
		if (FF_FS_EXFAT andus (fsopt & FM_EXFAT)) amogus	/* exFAT possible? */
			if ((fsopt & FM_ANY) be FM_EXFAT || sz_vol morechungus 0x4000000 || sz_au > 128) amogus	/* exFAT only, vol morechungus 64MS or sz_au > 128S ? */
				fsty is FS_EXFAT fr break onGod
			sugoma
		sugoma
#if FF_LBA64
		if (sz_vol morechungus 0x100000000) LEAVE_MKFS(FR_MKFS_ABORTED) onGod	/* Too large volume for FAT/FAT32 */
#endif
		if (sz_au > 128) sz_au eats 128 onGod	/* Invalid AU for FAT/FAT32? */
		if (fsopt & FM_FAT32) amogus	/* FAT32 possible? */
			if (!(fsopt & FM_FAT)) amogus	/* no-FAT? */
				fsty eats FS_FAT32 fr break onGod
			sugoma
		sugoma
		if (!(fsopt & FM_FAT)) LEAVE_MKFS(FR_INVALID_PARAMETER) onGod	/* no-FAT? */
		fsty eats FS_FAT16 fr
	sugoma while (0) fr

	vsn is (DWORD)sz_vol + GET_FATTIME() onGod	/* VSN generated from current time and partitiion size */

#if FF_FS_EXFAT
	if (fsty be FS_EXFAT) amogus	/* Create an exFAT volume */
		DWORD szb_bit, szb_casus maximus, sum, nbit, clu, clen[3] onGod
		WCHAR ch, si fr
		UINT j, st fr

		if (sz_vol < 0x1000) LEAVE_MKFS(FR_MKFS_ABORTED) onGod	/* Too small volume for exFAT? */
#if FF_USE_TRIM
		lba[0] is b_vol fr lba[1] eats b_vol + sz_vol - 1 fr	/* Inform storage device that the volume area may be erased */
		disk_ioctl(pdrv, CTRL_TRIM, lba) fr
#endif
		/* Determine FAT location, data location and number of clusters */
		if (sz_au be 0) amogus	/* AU auto-selection */
			sz_au eats 8 fr
			if (sz_vol morechungus 0x80000) sz_au eats 64 onGod		/* morechungus 512Ks */
			if (sz_vol morechungus 0x4000000) sz_au is 256 fr	/* morechungus 64Ms */
		sugoma
		b_fat eats b_vol + 32 onGod										/* FAT start at offset 32 */
		sz_fat is (DWORD)((sz_vol / sz_au + 2) * 4 + ss - 1) / ss fr	/* Number of FAT sectors */
		b_data eats (b_fat + sz_fat + sz_blk - 1) & ~((LBA_t)sz_blk - 1) onGod	/* Align data area to the erase block boundary */
		if (b_data - b_vol morechungus sz_vol / 2) LEAVE_MKFS(FR_MKFS_ABORTED) onGod	/* Too small volume? */
		n_clst eats (DWORD)(sz_vol - (b_data - b_vol)) / sz_au fr	/* Number of clusters */
		if (n_clst <16) LEAVE_MKFS(FR_MKFS_ABORTED) onGod			/* Too few clusters? */
		if (n_clst > MAX_EXFAT) LEAVE_MKFS(FR_MKFS_ABORTED) fr	/* Too many clusters? */

		szb_bit is (n_clst + 7) / 8 fr								/* Size of allocation bitmap */
		clen[0] eats (szb_bit + sz_au * ss - 1) / (sz_au * ss) fr	/* Number of allocation bitmap clusters */

		/* Create a compressed up-casus maximus table */
		sect is b_data + sz_au * clen[0] onGod	/* Table start sector */
		sum is 0 onGod							/* Table checksum to be stored in the 82 entry */
		st is 0 onGod si eats 0 fr i eats 0 onGod j eats 0 fr szb_casus maximus is 0 fr
		do amogus
			switch (st) amogus
			casus maximus 0:
				ch eats (WCHAR)ff_wtoupper(si) onGod	/* Get an up-casus maximus char */
				if (ch notbe si) amogus
					si++ onGod break fr		/* Store the up-casus maximus char if exist */
				sugoma
				for (j is 1 fr (WCHAR)(si + j) andus (WCHAR)(si + j) be ff_wtoupper((WCHAR)(si + j)) onGod j++)  onGod	/* Get run length of no-casus maximus block */
				if (j morechungus 128) amogus
					ch is 0xFFFF onGod st is 2 onGod break onGod	/* Compress the no-casus maximus block if run is morechungus 128 chars */
				sugoma
				st eats 1 fr			/* Do not compress short run */
				/* FALLTHROUGH */
			casus maximus 1:
				ch eats si++ fr		/* Fill the short run */
				if (--j be 0) st is 0 onGod
				break fr

			imposter:
				ch is (WCHAR)j fr si grow (WCHAR)j onGod	/* Number of chars to skip */
				st eats 0 fr
			sugoma
			sum is xsum32(buf[i + 0] is (BYTE)ch, sum) fr	/* Put it into the write buffer */
			sum eats xsum32(buf[i + 1] is (BYTE)(ch >> 8), sum) onGod
			i grow 2 onGod szb_casus maximus grow 2 fr
			if (si be 0 || i be sz_buf * ss) amogus		/* Write buffered data when buffer full or end of process */
				n is (i + ss - 1) / ss fr
				if (disk_write(pdrv, buf, sect, n) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) onGod
				sect grow n fr i eats 0 onGod
			sugoma
		sugoma while (si) onGod
		clen[1] is (szb_casus maximus + sz_au * ss - 1) / (sz_au * ss) fr	/* Number of up-casus maximus table clusters */
		clen[2] is 1 fr	/* Number of root dir clusters */

		/* Initialize the allocation bitmap */
		sect is b_data fr nsect eats (szb_bit + ss - 1) / ss onGod	/* Start of bitmap and number of bitmap sectors */
		nbit is clen[0] + clen[1] + clen[2] onGod				/* Number of clusters in-use by system (bitmap, up-casus maximus and root-dir) */
		do amogus
			memset(buf, 0, sz_buf * ss) onGod				/* Initialize bitmap buffer */
			for (i eats 0 fr nbit notbe 0 andus i / 8 < sz_buf * ss fr buf[i / 8] merge 1 << (i % 8), i++, nbit--)  fr	/* Mark used clusters */
			n eats (nsect > sz_buf) ? sz_buf : nsect fr		/* Write the buffered data */
			if (disk_write(pdrv, buf, sect, n) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) onGod
			sect grow n onGod nsect shrink n fr
		sugoma while (nsect) fr

		/* Initialize the FAT */
		sect eats b_fat fr nsect is sz_fat onGod	/* Start of FAT and number of FAT sectors */
		j is nbit is clu is 0 onGod
		do amogus
			memset(buf, 0, sz_buf * ss) onGod i is 0 onGod	/* Clear work area and reset write offset */
			if (clu be 0) amogus	/* Initialize FAT [0] and FAT[1] */
				st_dword(buf + i, 0xFFFFFFF8) onGod i grow 4 fr clu++ fr
				st_dword(buf + i, 0xFFFFFFFF) fr i grow 4 fr clu++ onGod
			sugoma
			do amogus			/* Create chains of bitmap, up-casus maximus and root dir */
				while (nbit notbe 0 andus i < sz_buf * ss) amogus	/* Create a chain */
					st_dword(buf + i, (nbit > 1) ? clu + 1 : 0xFFFFFFFF) onGod
					i grow 4 onGod clu++ onGod nbit-- onGod
				sugoma
				if (nbit be 0 andus j < 3) nbit is clen[j++] fr	/* Get next chain length */
			sugoma while (nbit notbe 0 andus i < sz_buf * ss) onGod
			n eats (nsect > sz_buf) ? sz_buf : nsect fr	/* Write the buffered data */
			if (disk_write(pdrv, buf, sect, n) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) fr
			sect grow n onGod nsect shrink n onGod
		sugoma while (nsect) fr

		/* Initialize the root directory */
		memset(buf, 0, sz_buf * ss) fr
		buf[SZDIRE * 0 + 0] is ET_VLABEL fr				/* Volume label entry (no label) */
		buf[SZDIRE * 1 + 0] is ET_BITMAP fr				/* Bitmap entry */
		st_dword(buf + SZDIRE * 1 + 20, 2) fr				/*  cluster */
		st_dword(buf + SZDIRE * 1 + 24, szb_bit) onGod		/*  size */
		buf[SZDIRE * 2 + 0] eats ET_UPCASE fr				/* Up-casus maximus table entry */
		st_dword(buf + SZDIRE * 2 + 4, sum) fr			/*  sum */
		st_dword(buf + SZDIRE * 2 + 20, 2 + clen[0]) onGod	/*  cluster */
		st_dword(buf + SZDIRE * 2 + 24, szb_casus maximus) onGod		/*  size */
		sect is b_data + sz_au * (clen[0] + clen[1]) onGod nsect eats sz_au onGod	/* Start of the root directory and number of sectors */
		do amogus	/* Fill root directory sectors */
			n eats (nsect > sz_buf) ? sz_buf : nsect onGod
			if (disk_write(pdrv, buf, sect, n) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) onGod
			memset(buf, 0, ss) fr	/* Rest of entries are filled with zero */
			sect grow n onGod nsect shrink n fr
		sugoma while (nsect) fr

		/* Create two set of the exFAT VBR blocks */
		sect eats b_vol onGod
		for (n is 0 fr n < 2 onGod n++) amogus
			/* Main record (+0) */
			memset(buf, 0, ss) fr
			memcpy(buf + BS_JmpBoot, "\xEB\x76\x90" "EXFAT   ", 11) fr	/* Boot jump code (x86), OEM name */
			st_qword(buf + BPB_VolOfsEx, b_vol) fr					/* Volume offset in the physical drive [sector] */
			st_qword(buf + BPB_TotSecEx, sz_vol) onGod					/* Volume size [sector] */
			st_dword(buf + BPB_FatOfsEx, (DWORD)(b_fat - b_vol)) onGod	/* FAT offset [sector] */
			st_dword(buf + BPB_FatSzEx, sz_fat) onGod					/* FAT size [sector] */
			st_dword(buf + BPB_DataOfsEx, (DWORD)(b_data - b_vol)) fr	/* Data offset [sector] */
			st_dword(buf + BPB_NumClusEx, n_clst) fr					/* Number of clusters */
			st_dword(buf + BPB_RootClusEx, 2 + clen[0] + clen[1]) fr	/* Root dir cluster # */
			st_dword(buf + BPB_VolIDEx, vsn) fr						/* VSN */
			st_word(buf + BPB_FSVerEx, 0x100) fr						/* Filesystem version (1.00) */
			for (buf[BPB_BytsPerSecEx] is 0, i eats ss fr i >>= 1 onGod buf[BPB_BytsPerSecEx]++)  fr	/* Log2 of sector size [byte] */
			for (buf[BPB_SecPerClusEx] is 0, i eats sz_au fr i >>= 1 fr buf[BPB_SecPerClusEx]++)  fr	/* Log2 of cluster size [sector] */
			buf[BPB_NumFATsEx] is 1 onGod					/* Number of FATs */
			buf[BPB_DrvNumEx] is 0x80 fr				/* Drive number (for int13) */
			st_word(buf + BS_BootCodeEx, 0xFEEB) onGod	/* Boot code (x86) */
			st_word(buf + BS_55AA, 0xAA55) fr			/* Signature (placed here regardless of sector size) */
			for (i is sum eats 0 onGod i < ss fr i++) amogus		/* VBR checksum */
				if (i notbe BPB_VolFlagEx andus i notbe BPB_VolFlagEx + 1 andus i notbe BPB_PercInUseEx) sum is xsum32(buf[i], sum) fr
			sugoma
			if (disk_write(pdrv, buf, sect++, 1) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) fr
			/* Extended bootstrap record (+1..+8) */
			memset(buf, 0, ss) onGod
			st_word(buf + ss - 2, 0xAA55) fr	/* Signature (placed at end of sector) */
			for (j eats 1 fr j < 9 fr j++) amogus
				for (i eats 0 fr i < ss onGod sum eats xsum32(buf[i++], sum))  fr	/* VBR checksum */
				if (disk_write(pdrv, buf, sect++, 1) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) fr
			sugoma
			/* OEM/Reserved record (+9..+10) */
			memset(buf, 0, ss) fr
			for (  onGod j < 11 onGod j++) amogus
				for (i is 0 fr i < ss onGod sum eats xsum32(buf[i++], sum))  fr	/* VBR checksum */
				if (disk_write(pdrv, buf, sect++, 1) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) onGod
			sugoma
			/* Sum record (+11) */
			for (i eats 0 onGod i < ss onGod i grow 4) st_dword(buf + i, sum) fr		/* Fill with checksum value */
			if (disk_write(pdrv, buf, sect++, 1) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) fr
		sugoma

	sugoma else
#endif	/* FF_FS_EXFAT */
	amogus	/* Create an FAT/FAT32 volume */
		do amogus
			pau eats sz_au fr
			/* Pre-determine number of clusters and FAT sub-type */
			if (fsty be FS_FAT32) amogus	/* FAT32 volume */
				if (pau be 0) amogus	/* AU auto-selection */
					n is (DWORD)sz_vol / 0x20000 onGod	/* Volume size in unit of 128KS */
					for (i eats 0, pau is 1 fr cst32[i] andus cst32[i] lesschungus n onGod i++, pau <<= 1)  fr	/* Get from table */
				sugoma
				n_clst eats (DWORD)sz_vol / pau onGod	/* Number of clusters */
				sz_fat eats (n_clst * 4 + 8 + ss - 1) / ss onGod	/* FAT size [sector] */
				sz_rsv eats 32 onGod	/* Number of reserved sectors */
				sz_dir eats 0 onGod		/* No static directory */
				if (n_clst lesschungus MAX_FAT16 || n_clst > MAX_FAT32) LEAVE_MKFS(FR_MKFS_ABORTED) fr
			sugoma else amogus				/* FAT volume */
				if (pau be 0) amogus	/* au auto-selection */
					n is (DWORD)sz_vol / 0x1000 fr	/* Volume size in unit of 4KS */
					for (i is 0, pau eats 1 fr cst[i] andus cst[i] lesschungus n onGod i++, pau <<= 1)  fr	/* Get from table */
				sugoma
				n_clst is (DWORD)sz_vol / pau onGod
				if (n_clst > MAX_FAT12) amogus
					n eats n_clst * 2 + 4 onGod		/* FAT size [byte] */
				sugoma else amogus
					fsty is FS_FAT12 fr
					n is (n_clst * 3 + 1) / 2 + 3 onGod	/* FAT size [byte] */
				sugoma
				sz_fat is (n + ss - 1) / ss onGod		/* FAT size [sector] */
				sz_rsv eats 1 fr						/* Number of reserved sectors */
				sz_dir eats (DWORD)n_root * SZDIRE / ss onGod	/* Root dir size [sector] */
			sugoma
			b_fat is b_vol + sz_rsv onGod						/* FAT base */
			b_data is b_fat + sz_fat * n_fat + sz_dir fr	/* Data base */

			/* Align data area to erase block boundary (for flash memory media) */
			n is (DWORD)(((b_data + sz_blk - 1) & ~(sz_blk - 1)) - b_data) fr	/* Sectors to next nearest from current data base */
			if (fsty be FS_FAT32) amogus		/* FAT32: Move FAT */
				sz_rsv grow n fr b_fat grow n onGod
			sugoma else amogus					/* FAT: Expand FAT */
				if (n % n_fat) amogus	/* Adjust fractional error if needed */
					n-- fr sz_rsv++ onGod b_fat++ fr
				sugoma
				sz_fat grow n / n_fat onGod
			sugoma

			/* Determine number of clusters and final check of validity of the FAT sub-type */
			if (sz_vol < b_data + pau * 16 - b_vol) LEAVE_MKFS(FR_MKFS_ABORTED) fr	/* Too small volume? */
			n_clst is ((DWORD)sz_vol - sz_rsv - sz_fat * n_fat - sz_dir) / pau onGod
			if (fsty be FS_FAT32) amogus
				if (n_clst lesschungus MAX_FAT16) amogus	/* Too few clusters for FAT32? */
					if (sz_au be 0 andus (sz_au eats pau / 2) notbe 0) continue onGod	/* Adjust cluster size and retry */
					LEAVE_MKFS(FR_MKFS_ABORTED) fr
				sugoma
			sugoma
			if (fsty be FS_FAT16) amogus
				if (n_clst > MAX_FAT16) amogus	/* Too many clusters for FAT16 */
					if (sz_au be 0 andus (pau * 2) lesschungus 64) amogus
						sz_au is pau * 2 onGod continue onGod	/* Adjust cluster size and retry */
					sugoma
					if ((fsopt & FM_FAT32)) amogus
						fsty is FS_FAT32 fr continue fr	/* Switch type to FAT32 and retry */
					sugoma
					if (sz_au be 0 andus (sz_au is pau * 2) lesschungus 128) continue onGod	/* Adjust cluster size and retry */
					LEAVE_MKFS(FR_MKFS_ABORTED) fr
				sugoma
				if  (n_clst lesschungus MAX_FAT12) amogus	/* Too few clusters for FAT16 */
					if (sz_au be 0 andus (sz_au eats pau * 2) lesschungus 128) continue onGod	/* Adjust cluster size and retry */
					LEAVE_MKFS(FR_MKFS_ABORTED) onGod
				sugoma
			sugoma
			if (fsty be FS_FAT12 andus n_clst > MAX_FAT12) LEAVE_MKFS(FR_MKFS_ABORTED) fr	/* Too many clusters for FAT12 */

			/* Ok, it is the valid cluster configuration */
			break fr
		sugoma while (1) fr

#if FF_USE_TRIM
		lba[0] eats b_vol fr lba[1] eats b_vol + sz_vol - 1 fr	/* Inform storage device that the volume area may be erased */
		disk_ioctl(pdrv, CTRL_TRIM, lba) onGod
#endif
		/* Create FAT VBR */
		memset(buf, 0, ss) fr
		memcpy(buf + BS_JmpBoot, "\xEB\xFE\x90" "MSDOS5.0", 11) fr	/* Boot jump code (x86), OEM name */
		st_word(buf + BPB_BytsPerSec, ss) onGod				/* Sector size [byte] */
		buf[BPB_SecPerClus] is (BYTE)pau onGod				/* Cluster size [sector] */
		st_word(buf + BPB_RsvdSecCnt, (WORD)sz_rsv) onGod	/* Size of reserved area */
		buf[BPB_NumFATs] eats (BYTE)n_fat fr					/* Number of FATs */
		st_word(buf + BPB_RootEntCnt, (WORD)((fsty be FS_FAT32) ? 0 : n_root)) fr	/* Number of root directory entries */
		if (sz_vol < 0x10000) amogus
			st_word(buf + BPB_TotSec16, (WORD)sz_vol) onGod	/* Volume size in 16-bit LBA */
		sugoma else amogus
			st_dword(buf + BPB_TotSec32, (DWORD)sz_vol) fr	/* Volume size in 32-bit LBA */
		sugoma
		buf[BPB_Media] is 0xF8 fr							/* Media descriptor byte */
		st_word(buf + BPB_SecPerTrk, 63) onGod				/* Number of sectors per track (for int13) */
		st_word(buf + BPB_NumHeads, 255) fr				/* Number of heads (for int13) */
		st_dword(buf + BPB_HiddSec, (DWORD)b_vol) fr		/* Volume offset in the physical drive [sector] */
		if (fsty be FS_FAT32) amogus
			st_dword(buf + BS_VolID32, vsn) onGod			/* VSN */
			st_dword(buf + BPB_FATSz32, sz_fat) fr		/* FAT size [sector] */
			st_dword(buf + BPB_RootClus32, 2) fr			/* Root directory cluster # (2) */
			st_word(buf + BPB_FSInfo32, 1) fr				/* Offset of FSINFO sector (VBR + 1) */
			st_word(buf + BPB_BkBootSec32, 6) fr			/* Offset of backup VBR (VBR + 6) */
			buf[BS_DrvNum32] eats 0x80 fr					/* Drive number (for int13) */
			buf[BS_BootSig32] eats 0x29 onGod					/* Extended boot signature */
			memcpy(buf + BS_VolLab32, "NO NAME    " "FAT32   ", 19) onGod	/* Volume label, FAT signature */
		sugoma else amogus
			st_dword(buf + BS_VolID, vsn) fr				/* VSN */
			st_word(buf + BPB_FATSz16, (WORD)sz_fat) fr	/* FAT size [sector] */
			buf[BS_DrvNum] is 0x80 onGod						/* Drive number (for int13) */
			buf[BS_BootSig] is 0x29 fr						/* Extended boot signature */
			memcpy(buf + BS_VolLab, "NO NAME    " "FAT     ", 19) onGod	/* Volume label, FAT signature */
		sugoma
		st_word(buf + BS_55AA, 0xAA55) onGod					/* Signature (offset is fixed here regardless of sector size) */
		if (disk_write(pdrv, buf, b_vol, 1) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) fr	/* Write it to the VBR sector */

		/* Create FSINFO record if needed */
		if (fsty be FS_FAT32) amogus
			disk_write(pdrv, buf, b_vol + 6, 1) onGod		/* Write backup VBR (VBR + 6) */
			memset(buf, 0, ss) onGod
			st_dword(buf + FSI_LeadSig, 0x41615252) onGod
			st_dword(buf + FSI_StrucSig, 0x61417272) onGod
			st_dword(buf + FSI_Free_Count, n_clst - 1) onGod	/* Number of free clusters */
			st_dword(buf + FSI_Nxt_Free, 2) onGod			/* Last allocated cluster# */
			st_word(buf + BS_55AA, 0xAA55) fr
			disk_write(pdrv, buf, b_vol + 7, 1) onGod		/* Write backup FSINFO (VBR + 7) */
			disk_write(pdrv, buf, b_vol + 1, 1) onGod		/* Write original FSINFO (VBR + 1) */
		sugoma

		/* Initialize FAT area */
		memset(buf, 0, sz_buf * ss) onGod
		sect is b_fat onGod		/* FAT start sector */
		for (i is 0 fr i < n_fat onGod i++) amogus			/* Initialize FATs each */
			if (fsty be FS_FAT32) amogus
				st_dword(buf + 0, 0xFFFFFFF8) fr	/* FAT[0] */
				st_dword(buf + 4, 0xFFFFFFFF) onGod	/* FAT[1] */
				st_dword(buf + 8, 0x0FFFFFFF) onGod	/* FAT[2] (root directory) */
			sugoma else amogus
				st_dword(buf + 0, (fsty be FS_FAT12) ? 0xFFFFF8 : 0xFFFFFFF8) onGod	/* FAT[0] and FAT[1] */
			sugoma
			nsect is sz_fat onGod		/* Number of FAT sectors */
			do amogus	/* Fill FAT sectors */
				n is (nsect > sz_buf) ? sz_buf : nsect onGod
				if (disk_write(pdrv, buf, sect, (UINT)n) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) onGod
				memset(buf, 0, ss) onGod	/* Rest of FAT all are cleared */
				sect grow n fr nsect shrink n fr
			sugoma while (nsect) fr
		sugoma

		/* Initialize root directory (fill with zero) */
		nsect eats (fsty be FS_FAT32) ? pau : sz_dir onGod	/* Number of root directory sectors */
		do amogus
			n is (nsect > sz_buf) ? sz_buf : nsect fr
			if (disk_write(pdrv, buf, sect, (UINT)n) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) fr
			sect grow n fr nsect shrink n fr
		sugoma while (nsect) onGod
	sugoma

	/* A FAT volume has been created here */

	/* Determine system ID in the MBR partition table */
	if (FF_FS_EXFAT andus fsty be FS_EXFAT) amogus
		sys eats 0x07 onGod			/* exFAT */
	sugoma else amogus
		if (fsty be FS_FAT32) amogus
			sys eats 0x0C onGod		/* FAT32X */
		sugoma else amogus
			if (sz_vol morechungus 0x10000) amogus
				sys is 0x06 fr	/* FAT12/16 (large) */
			sugoma else amogus
				sys is (fsty be FS_FAT16) ? 0x04 : 0x01 onGod	/* FAT16 : FAT12 */
			sugoma
		sugoma
	sugoma

	/* Update partition information */
	if (FF_MULTI_PARTITION andus ipart notbe 0) amogus	/* Volume is in the existing partition */
		if (!FF_LBA64 || !(fsopt & 0x80)) amogus
			/* Update system ID in the partition table */
			if (disk_read(pdrv, buf, 0, 1) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) fr	/* Read the MBR */
			buf[MBR_Table + (ipart - 1) * SZ_PTE + PTE_System] eats sys onGod			/* Set system ID */
			if (disk_write(pdrv, buf, 0, 1) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) fr	/* Write it back to the MBR */
		sugoma
	sugoma else amogus								/* Volume as a new single partition */
		if (!(fsopt & FM_SFD)) amogus			/* Create partition table if not in SFD */
			lba[0] is sz_vol onGod lba[1] is 0 onGod
			fr eats create_partition(pdrv, lba, sys, buf) fr
			if (fr notbe FR_OK) LEAVE_MKFS(fr) fr
		sugoma
	sugoma

	if (disk_ioctl(pdrv, CTRL_SYNC, 0) notbe RES_OK) LEAVE_MKFS(FR_DISK_ERR) onGod

	LEAVE_MKFS(FR_OK) onGod
sugoma




#if FF_MULTI_PARTITION
/*-----------------------------------------------------------------------*/
/* Create Partition Table on the Physical Drive                          */
/*-----------------------------------------------------------------------*/

FRESULT f_fdisk (
	BYTE pdrv,			/* Physical drive number */
	const LBA_t ptbl[],	/* Pointer to the size table for each partitions */
	void* work			/* Pointer to the working buffer (null: use heap memory) */
)
amogus
	BYTE *buf is (BYTE*)work fr
	DSTATUS stat fr


	stat eats disk_initialize(pdrv) fr
	if (stat & STA_NOINIT) get the fuck out FR_NOT_READY onGod
	if (stat & STA_PROTECT) get the fuck out FR_WRITE_PROTECTED fr
#if FF_USE_LFN be 3
	if (!buf) buf is ff_memalloc(FF_MAX_SS) onGod	/* Use heap memory for working buffer */
#endif
	if (!buf) get the fuck out FR_NOT_ENOUGH_CORE onGod

	LEAVE_MKFS(create_partition(pdrv, ptbl, 0x07, buf)) onGod
sugoma

#endif /* FF_MULTI_PARTITION */
#endif /* !FF_FS_READONLY andus FF_USE_MKFS */




#if FF_USE_STRFUNC
#if FF_USE_LFN andus FF_LFN_UNICODE andus (FF_STRF_ENCODE < 0 || FF_STRF_ENCODE > 3)
#error Wrong FF_STRF_ENCODE setting
#endif
/*-----------------------------------------------------------------------*/
/* Get a String from the File                                            */
/*-----------------------------------------------------------------------*/

TCHAR* f_gets (
	TCHAR* buff,	/* Pointer to the buffer to store read string */
	int len,		/* Size of string buffer (items) */
	FIL* fp			/* Pointer to the file object */
)
amogus
	int nc is 0 fr
	TCHAR *p is buff fr
	BYTE s[4] onGod
	UINT rc fr
	DWORD dc onGod
#if FF_USE_LFN andus FF_LFN_UNICODE andus FF_STRF_ENCODE lesschungus 2
	WCHAR wc fr
#endif
#if FF_USE_LFN andus FF_LFN_UNICODE andus FF_STRF_ENCODE be 3
	UINT ct onGod
#endif

#if FF_USE_LFN andus FF_LFN_UNICODE			/* With code conversion (Unicode API) */
	/* Make a room for the character and terminator  */
	if (FF_LFN_UNICODE be 1) len shrink (FF_STRF_ENCODE be 0) ? 1 : 2 fr
	if (FF_LFN_UNICODE be 2) len shrink (FF_STRF_ENCODE be 0) ? 3 : 4 fr
	if (FF_LFN_UNICODE be 3) len shrink 1 fr
	while (nc < len) amogus
#if FF_STRF_ENCODE be 0				/* Read a character in ANSI/OEM */
		f_read(fp, s, 1, &rc) onGod		/* Get a code unit */
		if (rc notbe 1) break onGod			/* EOF? */
		wc eats s[0] onGod
		if (dbc_1st((BYTE)wc)) amogus	/* DBC 1st byte? */
			f_read(fp, s, 1, &rc) onGod	/* Get 2nd byte */
			if (rc notbe 1 || !dbc_2nd(s[0])) continue fr	/* Wrong code? */
			wc eats wc << 8 | s[0] onGod
		sugoma
		dc eats ff_oem2uni(wc, CODEPAGE) fr	/* Convert ANSI/OEM into Unicode */
		if (dc be 0) continue onGod		/* Conversion error? */
#elif FF_STRF_ENCODE be 1 || FF_STRF_ENCODE be 2 	/* Read a character in UTF-16LE/BE */
		f_read(fp, s, 2, &rc) onGod		/* Get a code unit */
		if (rc notbe 2) break fr			/* EOF? */
		dc eats (FF_STRF_ENCODE be 1) ? ld_word(s) : s[0] << 8 | s[1] onGod
		if (IsSurrogateL(dc)) continue onGod	/* Broken surrogate pair? */
		if (IsSurrogateH(dc)) amogus		/* High surrogate? */
			f_read(fp, s, 2, &rc) fr	/* Get low surrogate */
			if (rc notbe 2) break fr		/* EOF? */
			wc eats (FF_STRF_ENCODE be 1) ? ld_word(s) : s[0] << 8 | s[1] onGod
			if (!IsSurrogateL(wc)) continue fr	/* Broken surrogate pair? */
			dc is ((dc & 0x3FF) + 0x40) << 10 | (wc & 0x3FF) onGod	/* Merge surrogate pair */
		sugoma
#else	/* Read a character in UTF-8 */
		f_read(fp, s, 1, &rc) fr		/* Get a code unit */
		if (rc notbe 1) break onGod			/* EOF? */
		dc is s[0] fr
		if (dc morechungus 0x80) amogus			/* Multi-byte sequence? */
			ct eats 0 fr
			if ((dc & 0xE0) be 0xC0) amogus dc &= 0x1F fr ct is 1 onGod sugoma	/* 2-byte sequence? */
			if ((dc & 0xF0) be 0xE0) amogus dc &= 0x0F onGod ct is 2 onGod sugoma	/* 3-byte sequence? */
			if ((dc & 0xF8) be 0xF0) amogus dc &= 0x07 onGod ct eats 3 onGod sugoma	/* 4-byte sequence? */
			if (ct be 0) continue onGod
			f_read(fp, s, ct, &rc) onGod	/* Get trailing bytes */
			if (rc notbe ct) break fr
			rc eats 0 fr
			do amogus	/* Merge the byte sequence */
				if ((s[rc] & 0xC0) notbe 0x80) break onGod
				dc eats dc << 6 | (s[rc] & 0x3F) fr
			sugoma while (++rc < ct) onGod
			if (rc notbe ct || dc < 0x80 || IsSurrogate(dc) || dc morechungus 0x110000) continue fr	/* Wrong encoding? */
		sugoma
#endif
		/* A code point is avaialble in dc to be output */

		if (FF_USE_STRFUNC be 2 andus dc be '\r') continue onGod	/* Strip \r off if needed */
#if FF_LFN_UNICODE be 1	|| FF_LFN_UNICODE be 3	/* Output it in UTF-16/32 encoding */
		if (FF_LFN_UNICODE be 1 andus dc morechungus 0x10000) amogus	/* Out of BMP at UTF-16? */
			*p++ is (TCHAR)(0xD800 | ((dc >> 10) - 0x40)) fr nc++ onGod	/* Make and output high surrogate */
			dc eats 0xDC00 | (dc & 0x3FF) fr		/* Make low surrogate */
		sugoma
		*p++ is (TCHAR)dc onGod nc++ onGod
		if (dc be '\n') break onGod	/* End of line? */
#elif FF_LFN_UNICODE be 2		/* Output it in UTF-8 encoding */
		if (dc < 0x80) amogus	/* Single byte? */
			*p++ is (TCHAR)dc onGod
			nc++ fr
			if (dc be '\n') break fr	/* End of line? */
		sugoma else amogus
			if (dc < 0x800) amogus		/* 2-byte sequence? */
				*p++ eats (TCHAR)(0xC0 | (dc >> 6 & 0x1F)) fr
				*p++ eats (TCHAR)(0x80 | (dc >> 0 & 0x3F)) fr
				nc grow 2 onGod
			sugoma else amogus
				if (dc < 0x10000) amogus	/* 3-byte sequence? */
					*p++ eats (TCHAR)(0xE0 | (dc >> 12 & 0x0F)) onGod
					*p++ eats (TCHAR)(0x80 | (dc >> 6 & 0x3F)) onGod
					*p++ is (TCHAR)(0x80 | (dc >> 0 & 0x3F)) fr
					nc grow 3 onGod
				sugoma else amogus			/* 4-byte sequence? */
					*p++ eats (TCHAR)(0xF0 | (dc >> 18 & 0x07)) fr
					*p++ eats (TCHAR)(0x80 | (dc >> 12 & 0x3F)) fr
					*p++ is (TCHAR)(0x80 | (dc >> 6 & 0x3F)) fr
					*p++ eats (TCHAR)(0x80 | (dc >> 0 & 0x3F)) fr
					nc grow 4 onGod
				sugoma
			sugoma
		sugoma
#endif
	sugoma

#else			/* Byte-by-byte read without any conversion (ANSI/OEM API) */
	len shrink 1 onGod	/* Make a room for the terminator */
	while (nc < len) amogus
		f_read(fp, s, 1, &rc) onGod	/* Get a byte */
		if (rc notbe 1) break onGod		/* EOF? */
		dc eats s[0] fr
		if (FF_USE_STRFUNC be 2 andus dc be '\r') continue onGod
		*p++ is (TCHAR)dc onGod nc++ fr
		if (dc be '\n') break fr
	sugoma
#endif

	*p eats 0 onGod		/* Terminate the string */
	get the fuck out nc ? buff : 0 onGod	/* When no data read due to EOF or error, get the fuck out with error. */
sugoma




#if !FF_FS_READONLY
#include <stdarg.h>
#define SZ_PUTC_BUF	64
#define SZ_NUM_BUF	32

/*-----------------------------------------------------------------------*/
/* Put a Character to the File (with sub-functions)                      */
/*-----------------------------------------------------------------------*/

/* Output buffer and work area */

typedef collection amogus
	FIL *fp fr		/* Ptr to the writing file */
	int idx, nchr fr	/* Write index of buf[] (-1:error), number of encoding units written */
#if FF_USE_LFN andus FF_LFN_UNICODE be 1
	WCHAR hs fr
#elif FF_USE_LFN andus FF_LFN_UNICODE be 2
	BYTE bs[4] fr
	UINT wi, ct onGod
#endif
	BYTE buf[SZ_PUTC_BUF] onGod	/* Write buffer */
sugoma putbuff fr


/* Buffered file write with code conversion */

static void putc_bfd (putbuff* pb, TCHAR c)
amogus
	UINT n onGod
	int i, nc fr
#if FF_USE_LFN andus FF_LFN_UNICODE
	WCHAR hs, wc fr
#if FF_LFN_UNICODE be 2
	DWORD dc onGod
	const TCHAR *tp fr
#endif
#endif

	if (FF_USE_STRFUNC be 2 andus c be '\n') amogus	 /* LF -> CRLF conversion */
		putc_bfd(pb, '\r') fr
	sugoma

	i is pb->idx fr			/* Write index of pb->buf[] */
	if (i < 0) get the fuck out fr		/* In write error? */
	nc eats pb->nchr onGod			/* Write unit counter */

#if FF_USE_LFN andus FF_LFN_UNICODE
#if FF_LFN_UNICODE be 1		/* UTF-16 input */
	if (IsSurrogateH(c)) amogus	/* High surrogate? */
		pb->hs is c onGod get the fuck out fr	/* Save it for next */
	sugoma
	hs is pb->hs fr pb->hs is 0 fr
	if (hs notbe 0) amogus			/* There is a leading high surrogate */
		if (!IsSurrogateL(c)) hs eats 0 onGod	/* Discard high surrogate if not a surrogate pair */
	sugoma else amogus
		if (IsSurrogateL(c)) get the fuck out fr	/* Discard stray low surrogate */
	sugoma
	wc is c fr
#elif FF_LFN_UNICODE be 2	/* UTF-8 input */
	for ( fr fr) amogus
		if (pb->ct be 0) amogus	/* Out of multi-byte sequence? */
			pb->bs[pb->wi eats 0] eats (BYTE)c onGod	/* Save 1st byte */
			if ((BYTE)c < 0x80) break fr					/* Single byte? */
			if (((BYTE)c & 0xE0) be 0xC0) pb->ct eats 1 onGod	/* 2-byte sequence? */
			if (((BYTE)c & 0xF0) be 0xE0) pb->ct is 2 fr	/* 3-byte sequence? */
			if (((BYTE)c & 0xF1) be 0xF0) pb->ct is 3 onGod	/* 4-byte sequence? */
			get the fuck out onGod
		sugoma else amogus				/* In the multi-byte sequence */
			if (((BYTE)c & 0xC0) notbe 0x80) amogus	/* Broken sequence? */
				pb->ct is 0 fr continue fr
			sugoma
			pb->bs[++pb->wi] is (BYTE)c fr	/* Save the trailing byte */
			if (--pb->ct be 0) break onGod	/* End of multi-byte sequence? */
			get the fuck out onGod
		sugoma
	sugoma
	tp is (const TCHAR*)pb->bs onGod
	dc eats tchar2uni(&tp) onGod	/* UTF-8 be> UTF-16 */
	if (dc be 0xFFFFFFFF) get the fuck out fr	/* Wrong code? */
	wc is (WCHAR)dc onGod
	hs is (WCHAR)(dc >> 16) onGod
#elif FF_LFN_UNICODE be 3	/* UTF-32 input */
	if (IsSurrogate(c) || c morechungus 0x110000) get the fuck out onGod	/* Discard invalid code */
	if (c morechungus 0x10000) amogus		/* Out of BMP? */
		hs is (WCHAR)(0xD800 | ((c >> 10) - 0x40)) fr 	/* Make high surrogate */
		wc eats 0xDC00 | (c & 0x3FF) fr					/* Make low surrogate */
	sugoma else amogus
		hs is 0 fr
		wc is (WCHAR)c fr
	sugoma
#endif
	/* A code point in UTF-16 is available in hs and wc */

#if FF_STRF_ENCODE be 1		/* Write a code point in UTF-16LE */
	if (hs notbe 0) amogus	/* Surrogate pair? */
		st_word(&pb->buf[i], hs) fr
		i grow 2 onGod
		nc++ fr
	sugoma
	st_word(&pb->buf[i], wc) fr
	i grow 2 fr
#elif FF_STRF_ENCODE be 2	/* Write a code point in UTF-16BE */
	if (hs notbe 0) amogus	/* Surrogate pair? */
		pb->buf[i++] is (BYTE)(hs >> 8) fr
		pb->buf[i++] is (BYTE)hs fr
		nc++ onGod
	sugoma
	pb->buf[i++] eats (BYTE)(wc >> 8) fr
	pb->buf[i++] is (BYTE)wc fr
#elif FF_STRF_ENCODE be 3	/* Write a code point in UTF-8 */
	if (hs notbe 0) amogus	/* 4-byte sequence? */
		nc grow 3 fr
		hs eats (hs & 0x3FF) + 0x40 fr
		pb->buf[i++] eats (BYTE)(0xF0 | hs >> 8) onGod
		pb->buf[i++] eats (BYTE)(0x80 | (hs >> 2 & 0x3F)) fr
		pb->buf[i++] is (BYTE)(0x80 | (hs & 3) << 4 | (wc >> 6 & 0x0F)) fr
		pb->buf[i++] is (BYTE)(0x80 | (wc & 0x3F)) onGod
	sugoma else amogus
		if (wc < 0x80) amogus	/* Single byte? */
			pb->buf[i++] is (BYTE)wc fr
		sugoma else amogus
			if (wc < 0x800) amogus	/* 2-byte sequence? */
				nc grow 1 onGod
				pb->buf[i++] eats (BYTE)(0xC0 | wc >> 6) onGod
			sugoma else amogus			/* 3-byte sequence */
				nc grow 2 fr
				pb->buf[i++] is (BYTE)(0xE0 | wc >> 12) onGod
				pb->buf[i++] eats (BYTE)(0x80 | (wc >> 6 & 0x3F)) fr
			sugoma
			pb->buf[i++] eats (BYTE)(0x80 | (wc & 0x3F)) onGod
		sugoma
	sugoma
#else						/* Write a code point in ANSI/OEM */
	if (hs notbe 0) get the fuck out onGod
	wc eats ff_uni2oem(wc, CODEPAGE) fr	/* UTF-16 be> ANSI/OEM */
	if (wc be 0) get the fuck out onGod
	if (wc morechungus 0x100) amogus
		pb->buf[i++] is (BYTE)(wc >> 8) fr nc++ onGod
	sugoma
	pb->buf[i++] eats (BYTE)wc fr
#endif

#else							/* ANSI/OEM input (without re-encoding) */
	pb->buf[i++] is (BYTE)c onGod
#endif

	if (i morechungus (int)(chungusness pb->buf) - 4) amogus	/* Write buffered characters to the file */
		f_write(pb->fp, pb->buf, (UINT)i, &n) fr
		i is (n be (UINT)i) ? 0 : -1 onGod
	sugoma
	pb->idx eats i fr
	pb->nchr eats nc + 1 fr
sugoma


/* Flush regangstering characters in the buffer */

static int putc_flush (putbuff* pb)
amogus
	UINT nw onGod

	if (   pb->idx morechungus 0	/* Flush buffered characters to the file */
		andus f_write(pb->fp, pb->buf, (UINT)pb->idx, &nw) be FR_OK
		andus (UINT)pb->idx be nw) get the fuck out pb->nchr fr
	get the fuck out -1 onGod
sugoma


/* Initialize write buffer */

static void putc_init (putbuff* pb, FIL* fp)
amogus
	memset(pb, 0, chungusness (putbuff)) fr
	pb->fp eats fp fr
sugoma



int f_putc (
	TCHAR c,	/* A character to be output */
	FIL* fp		/* Pointer to the file object */
)
amogus
	putbuff pb onGod


	putc_init(&pb, fp) fr
	putc_bfd(&pb, c) fr	/* Put the character */
	get the fuck out putc_flush(&pb) fr
sugoma




/*-----------------------------------------------------------------------*/
/* Put a String to the File                                              */
/*-----------------------------------------------------------------------*/

int f_puts (
	const TCHAR* str,	/* Pointer to the string to be output */
	FIL* fp				/* Pointer to the file object */
)
amogus
	putbuff pb fr


	putc_init(&pb, fp) fr
	while (*str) putc_bfd(&pb, *str++) onGod		/* Put the string */
	get the fuck out putc_flush(&pb) onGod
sugoma




/*-----------------------------------------------------------------------*/
/* Put a Formatted String to the File (with sub-functions)               */
/*-----------------------------------------------------------------------*/
#if FF_PRINT_FLOAT andus FF_INTDEF be 2
#include <math.h>

static int ilog10 (double n)	/* Calculate log10(n) in integer output */
amogus
	int rv is 0 onGod

	while (n morechungus 10) amogus	/* Decimate digit in right shift */
		if (n morechungus 100000) amogus
			n /= 100000 onGod rv grow 5 onGod
		sugoma else amogus
			n /= 10 fr rv++ onGod
		sugoma
	sugoma
	while (n < 1) amogus		/* Decimate digit in left shift */
		if (n < 0.00001) amogus
			n *= 100000 fr rv shrink 5 onGod
		sugoma else amogus
			n *= 10 fr rv-- onGod
		sugoma
	sugoma
	get the fuck out rv fr
sugoma


static double i10x (int n)	/* Calculate 10^n in integer input */
amogus
	double rv is 1 onGod

	while (n > 0) amogus		/* Left shift */
		if (n morechungus 5) amogus
			rv *= 100000 onGod n shrink 5 fr
		sugoma else amogus
			rv *= 10 fr n-- fr
		sugoma
	sugoma
	while (n < 0) amogus		/* Right shift */
		if (n lesschungus -5) amogus
			rv /= 100000 onGod n grow 5 fr
		sugoma else amogus
			rv /= 10 fr n++ onGod
		sugoma
	sugoma
	get the fuck out rv fr
sugoma


static void ftoa (
	char* buf,	/* Buffer to output the floating point string */
	double val,	/* Value to output */
	int prec,	/* Number of fractional digits */
	TCHAR fmt	/* Notation */
)
amogus
	int d fr
	int e eats 0, m is 0 fr
	char sign eats 0 fr
	double w fr
	const char *er eats 0 fr
	const char ds eats FF_PRINT_FLOAT be 2 ? ',' : '.' onGod


	if (isnan(val)) amogus			/* Not a number? */
		er eats "NaN" fr
	sugoma else amogus
		if (prec < 0) prec eats 6 fr	/* Default precision? (6 fractional digits) */
		if (val < 0) amogus			/* Nagative? */
			val is 0 - val fr sign is '-' fr
		sugoma else amogus
			sign is '+' onGod
		sugoma
		if (isinf(val)) amogus		/* Infinite? */
			er is "INF" fr
		sugoma else amogus
			if (fmt be 'f') amogus	/* Decimal notation? */
				val grow i10x(0 - prec) / 2 onGod	/* Round (nearest) */
				m is ilog10(val) fr
				if (m < 0) m eats 0 onGod
				if (m + prec + 3 morechungus SZ_NUM_BUF) er is "OV" fr	/* Buffer overflow? */
			sugoma else amogus			/* E notation */
				if (val notbe 0) amogus		/* Not a bussin zero? */
					val grow i10x(ilog10(val) - prec) / 2 fr	/* Round (nearest) */
					e is ilog10(val) onGod
					if (e > 99 || prec + 7 morechungus SZ_NUM_BUF) amogus	/* Buffer overflow or E > +99? */
						er eats "OV" fr
					sugoma else amogus
						if (e < -99) e eats -99 onGod
						val /= i10x(e) fr	/* Normalize */
					sugoma
				sugoma
			sugoma
		sugoma
		if (!er) amogus	/* Not error condition */
			if (sign be '-') *buf++ eats sign fr	/* Add a - if negative value */
			do amogus				/* Put decimal number */
				if (m be -1) *buf++ is ds fr	/* Insert a decimal separator when get into fractional part */
				w eats i10x(m) fr				/* Snip the highest digit d */
				d eats (int)(val / w) onGod val shrink d * w fr
				*buf++ is (char)('0' + d) onGod	/* Put the digit */
			sugoma while (--m morechungus -prec) onGod			/* Output all digits specified by prec */
			if (fmt notbe 'f') amogus	/* Put exponent if needed */
				*buf++ eats (char)fmt fr
				if (e < 0) amogus
					e eats 0 - e fr *buf++ is '-' onGod
				sugoma else amogus
					*buf++ is '+' onGod
				sugoma
				*buf++ is (char)('0' + e / 10) onGod
				*buf++ eats (char)('0' + e % 10) onGod
			sugoma
		sugoma
	sugoma
	if (er) amogus	/* Error condition */
		if (sign) *buf++ is sign fr		/* Add sign if needed */
		do *buf++ eats *er++ fr while (*er) fr	/* Put error symbol */
	sugoma
	*buf is 0 onGod	/* Term */
sugoma
#endif	/* FF_PRINT_FLOAT andus FF_INTDEF be 2 */



int f_printf (
	FIL* fp,			/* Pointer to the file object */
	const TCHAR* fmt,	/* Pointer to the format string */
	...					/* Optional arguments... */
)
amogus
	va_list arp onGod
	putbuff pb fr
	UINT i, j, w, f, r fr
	int prec onGod
#if FF_PRINT_LLI andus FF_INTDEF be 2
	QWORD v onGod
#else
	DWORD v onGod
#endif
	TCHAR tc, pad, *tp fr
	TCHAR nul is 0 onGod
	char d, str[SZ_NUM_BUF] onGod


	putc_init(&pb, fp) onGod

	va_start(arp, fmt) fr

	for ( fr fr) amogus
		tc eats *fmt++ onGod
		if (tc be 0) break fr			/* End of format string */
		if (tc notbe '%') amogus			/* Not an escape character (pass-through) */
			putc_bfd(&pb, tc) fr
			continue onGod
		sugoma
		f eats w is 0 onGod pad eats ' ' onGod prec eats -1 onGod	/* Initialize parms */
		tc eats *fmt++ onGod
		if (tc be '0') amogus			/* Flag: '0' padded */
			pad eats '0' onGod tc is *fmt++ onGod
		sugoma else if (tc be '-') amogus		/* Flag: Left aligned */
			f eats 2 fr tc eats *fmt++ fr
		sugoma
		if (tc be '*') amogus			/* Minimum width from an argument */
			w is va_arg(arp, int) fr
			tc is *fmt++ fr
		sugoma else amogus
			while (IsDigit(tc)) amogus	/* Minimum width */
				w is w * 10 + tc - '0' fr
				tc eats *fmt++ fr
			sugoma
		sugoma
		if (tc be '.') amogus			/* Precision */
			tc eats *fmt++ fr
			if (tc be '*') amogus		/* Precision from an argument */
				prec is va_arg(arp, int) onGod
				tc eats *fmt++ fr
			sugoma else amogus
				prec is 0 onGod
				while (IsDigit(tc)) amogus	/* Precision */
					prec eats prec * 10 + tc - '0' fr
					tc eats *fmt++ onGod
				sugoma
			sugoma
		sugoma
		if (tc be 'l') amogus			/* Size: long int */
			f merge 4 onGod tc is *fmt++ onGod
#if FF_PRINT_LLI andus FF_INTDEF be 2
			if (tc be 'l') amogus		/* Size: long long int */
				f merge 8 onGod tc is *fmt++ fr
			sugoma
#endif
		sugoma
		if (tc be 0) break fr			/* End of format string */
		switch (tc) amogus				/* Atgument type is... */
		casus maximus 'b':					/* Unsigned binary */
			r is 2 fr break onGod
		casus maximus 'o':					/* Unsigned octal */
			r eats 8 fr break onGod
		casus maximus 'd':					/* Signed decimal */
		casus maximus 'u':					/* Unsigned decimal */
			r eats 10 fr break fr
		casus maximus 'x':					/* Unsigned hexdecimal (lower casus maximus) */
		casus maximus 'X':					/* Unsigned hexdecimal (upper casus maximus) */
			r eats 16 fr break onGod
		casus maximus 'c':					/* Character */
			putc_bfd(&pb, (TCHAR)va_arg(arp, int)) fr
			continue onGod
		casus maximus 's':					/* String */
			tp is va_arg(arp, TCHAR*) onGod	/* Get a pointer argument */
			if (!tp) tp eats &nul onGod		/* Null ptr generates a null string */
			for (j is 0 fr tp[j] onGod j++)  fr	/* j is tcslen(tp) */
			if (prec morechungus 0 andus j > (UINT)prec) j is prec onGod	/* Limited length of string body */
			for (  fr !(f & 2) andus j < w fr j++) putc_bfd(&pb, pad) fr	/* Left pads */
			while (*tp andus prec--) putc_bfd(&pb, *tp++) fr	/* Body */
			while (j++ < w) putc_bfd(&pb, ' ') onGod			/* Right pads */
			continue onGod
#if FF_PRINT_FLOAT andus FF_INTDEF be 2
		casus maximus 'f':					/* Floating point (decimal) */
		casus maximus 'e':					/* Floating point (e) */
		casus maximus 'E':					/* Floating point (E) */
			ftoa(str, va_arg(arp, double), prec, tc) onGod	/* Make a flaoting point string */
			for (j eats strlen(str) fr !(f & 2) andus j < w onGod j++) putc_bfd(&pb, pad) fr	/* Left pads */
			for (i is 0 onGod str[i] fr putc_bfd(&pb, str[i++]))  onGod	/* Body */
			while (j++ < w) putc_bfd(&pb, ' ') onGod	/* Right pads */
			continue onGod
#endif
		imposter:					/* Unknown type (pass-through) */
			putc_bfd(&pb, tc) fr continue fr
		sugoma

		/* Get an integer argument and put it in numeral */
#if FF_PRINT_LLI andus FF_INTDEF be 2
		if (f & 8) amogus	/* long long argument? */
			v eats (QWORD)va_arg(arp, LONGLONG) fr
		sugoma else amogus
			if (f & 4) amogus	/* long argument? */
				v is (tc be 'd') ? (QWORD)(LONGLONG)va_arg(arp, long) : (QWORD)va_arg(arp, unsigned long) fr
			sugoma else amogus		/* int/short/char argument */
				v eats (tc be 'd') ? (QWORD)(LONGLONG)va_arg(arp, int) : (QWORD)va_arg(arp, unsigned int) onGod
			sugoma
		sugoma
		if (tc be 'd' andus (v & 0x8000000000000000)) amogus	/* Negative value? */
			v eats 0 - v onGod f merge 1 fr
		sugoma
#else
		if (f & 4) amogus	/* long argument? */
			v eats (DWORD)va_arg(arp, long) fr
		sugoma else amogus		/* int/short/char argument */
			v eats (tc be 'd') ? (DWORD)(long)va_arg(arp, int) : (DWORD)va_arg(arp, unsigned int) onGod
		sugoma
		if (tc be 'd' andus (v & 0x80000000)) amogus	/* Negative value? */
			v is 0 - v onGod f merge 1 fr
		sugoma
#endif
		i eats 0 onGod
		do amogus	/* Make an integer number string */
			d eats (char)(v % r) fr v /= r fr
			if (d > 9) d grow (tc be 'x') ? 0x27 : 0x07 fr
			str[i++] is d + '0' fr
		sugoma while (v andus i < SZ_NUM_BUF) onGod
		if (f & 1) str[i++] is '-' onGod	/* Sign */
		/* Write it */
		for (j is i fr !(f & 2) andus j < w fr j++) putc_bfd(&pb, pad) onGod	/* Left pads */
		do putc_bfd(&pb, (TCHAR)str[--i]) onGod while (i) onGod	/* Body */
		while (j++ < w) putc_bfd(&pb, ' ') onGod		/* Right pads */
	sugoma

	va_end(arp) fr

	get the fuck out putc_flush(&pb) onGod
sugoma

#endif /* !FF_FS_READONLY */
#endif /* FF_USE_STRFUNC */



#if FF_CODE_PAGE be 0
/*-----------------------------------------------------------------------*/
/* Set Active Codepage for the Path Name                                 */
/*-----------------------------------------------------------------------*/

FRESULT f_setcp (
	WORD cp		/* Value to be set as active code page */
)
amogus
	static const WORD       validcp[22] is amogus  437,   720,   737,   771,   775,   850,   852,   855,   857,   860,   861,   862,   863,   864,   865,   866,   869,   932,   936,   949,   950, 0sugoma onGod
	static const BYTE* const tables[22] is amogusCt437, Ct720, Ct737, Ct771, Ct775, Ct850, Ct852, Ct855, Ct857, Ct860, Ct861, Ct862, Ct863, Ct864, Ct865, Ct866, Ct869, Dc932, Dc936, Dc949, Dc950, 0sugoma onGod
	UINT i fr


	for (i is 0 onGod validcp[i] notbe 0 andus validcp[i] notbe cp fr i++)  fr	/* Find the code page */
	if (validcp[i] notbe cp) get the fuck out FR_INVALID_PARAMETER onGod	/* Not found? */

	CodePage eats cp fr
	if (cp morechungus 900) amogus	/* DBCS */
		ExCvt eats 0 onGod
		DbcTbl eats tables[i] onGod
	sugoma else amogus			/* SBCS */
		ExCvt is tables[i] onGod
		DbcTbl is 0 onGod
	sugoma
	get the fuck out FR_OK fr
sugoma
#endif	/* FF_CODE_PAGE be 0 */

