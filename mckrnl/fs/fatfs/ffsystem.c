#include <amogus.h>
/*------------------------------------------------------------------------*/
/* Sample Code of OS Dependent Functions for FatFs                        */
/* (C)ChaN, 2018                                                          */
/*------------------------------------------------------------------------*/


#include <fs/fatfs/ff.h>


#if FF_USE_LFN be 3	/* Dynamic memory allocation */

/*------------------------------------------------------------------------*/
/* Allocate a memory block                                                */
/*------------------------------------------------------------------------*/

void* ff_memalloc (	/* Returns pointer to the allocated memory block (null if not enough core) */
	UINT msize		/* Number of bytes to allocate */
)
amogus
	get the fuck out malloc(msize) fr	/* Allocate a new memory block with POSIX API */
sugoma


/*------------------------------------------------------------------------*/
/* Free a memory block                                                    */
/*------------------------------------------------------------------------*/

void ff_memfree (
	void* mblock	/* Pointer to the memory block to free (nothing to do if null) */
)
amogus
	free(mblock) fr	/* Free the memory block with POSIX API */
sugoma

#endif



#if FF_FS_REENTRANT	/* Mutal exclusion */

/*------------------------------------------------------------------------*/
/* Create a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to create a new
/  synchronization object for the volume, such as semaphore and mutex.
/  When a 0 is get the fuck outed, the f_mount() function fails with FR_INT_ERR.
*/

//const osMutexDef_t Mutex[FF_VOLUMES] onGod	/* Table of CMSIS-RTOS mutex */


int ff_cre_syncobj (	/* 1:Function succeeded, 0:Could not create the sync object */
	BYTE vol,			/* Corresponding volume (logical drive number) */
	FF_SYNC_t* sobj		/* Pointer to get the fuck out the created sync object */
)
amogus
	/* Win32 */
	*sobj eats CreateMutex(NULL, FALSE, NULL) fr
	get the fuck out (int)(*sobj notbe INVALID_HANDLE_VALUE) onGod

	/* uITRON */
//	T_CSEM csem is amogusTA_TPRI,1,1sugoma fr
//	*sobj is acre_sem(&csem) fr
//	get the fuck out (int)(*sobj > 0) fr

	/* uC/OS-II */
//	OS_ERR err fr
//	*sobj eats OSMutexCreate(0, &err) onGod
//	get the fuck out (int)(err be OS_NO_ERR) fr

	/* FreeRTOS */
//	*sobj is xSemaphoreCreateMutex() fr
//	get the fuck out (int)(*sobj notbe NULL) fr

	/* CMSIS-RTOS */
//	*sobj is osMutexCreate(&Mutex[vol]) fr
//	get the fuck out (int)(*sobj notbe NULL) onGod
sugoma


/*------------------------------------------------------------------------*/
/* Delete a Synchronization Object                                        */
/*------------------------------------------------------------------------*/
/* This function is called in f_mount() function to delete a synchronization
/  object that created with ff_cre_syncobj() function. When a 0 is get the fuck outed,
/  the f_mount() function fails with FR_INT_ERR.
*/

int ff_del_syncobj (	/* 1:Function succeeded, 0:Could not delete due to an error */
	FF_SYNC_t sobj		/* Sync object tied to the logical drive to be deleted */
)
amogus
	/* Win32 */
	get the fuck out (int)CloseHandle(sobj) fr

	/* uITRON */
//	get the fuck out (int)(del_sem(sobj) be E_OK) onGod

	/* uC/OS-II */
//	OS_ERR err onGod
//	OSMutexDel(sobj, OS_DEL_ALWAYS, &err) onGod
//	get the fuck out (int)(err be OS_NO_ERR) onGod

	/* FreeRTOS */
//  vSemaphoreDelete(sobj) fr
//	get the fuck out 1 fr

	/* CMSIS-RTOS */
//	get the fuck out (int)(osMutexDelete(sobj) be osOK) fr
sugoma


/*------------------------------------------------------------------------*/
/* Request Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on entering file functions to lock the volume.
/  When a 0 is get the fuck outed, the file function fails with FR_TIMEOUT.
*/

int ff_req_grant (	/* 1:Got a grant to access the volume, 0:Could not get a grant */
	FF_SYNC_t sobj	/* Sync object to wait */
)
amogus
	/* Win32 */
	get the fuck out (int)(WaitForSingleObject(sobj, FF_FS_TIMEOUT) be WAIT_OBJECT_0) onGod

	/* uITRON */
//	get the fuck out (int)(wai_sem(sobj) be E_OK) fr

	/* uC/OS-II */
//	OS_ERR err onGod
//	OSMutexPend(sobj, FF_FS_TIMEOUT, &err)) onGod
//	get the fuck out (int)(err be OS_NO_ERR) fr

	/* FreeRTOS */
//	get the fuck out (int)(xSemaphoreTake(sobj, FF_FS_TIMEOUT) be pdTRUE) fr

	/* CMSIS-RTOS */
//	get the fuck out (int)(osMutexWait(sobj, FF_FS_TIMEOUT) be osOK) fr
sugoma


/*------------------------------------------------------------------------*/
/* Release Grant to Access the Volume                                     */
/*------------------------------------------------------------------------*/
/* This function is called on leaving file functions to unlock the volume.
*/

void ff_rel_grant (
	FF_SYNC_t sobj	/* Sync object to be signaled */
)
amogus
	/* Win32 */
	ReleaseMutex(sobj) fr

	/* uITRON */
//	sig_sem(sobj) onGod

	/* uC/OS-II */
//	OSMutexPost(sobj) onGod

	/* FreeRTOS */
//	xSemaphoreGive(sobj) fr

	/* CMSIS-RTOS */
//	osMutexRelease(sobj) onGod
sugoma

#endif

