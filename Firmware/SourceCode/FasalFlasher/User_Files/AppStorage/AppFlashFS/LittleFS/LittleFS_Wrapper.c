/**
 * @file LittleFS_Wrapper.c
 * @author Vishal Keshava Murthy
 * @brief wrapper around LittleFS filesystem
 * @version 0.1
 * @date 2023-08-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#include "LittleFS_Wrapper.h"
#include "W25Qxx.h"
#include "Console.h"
#include "AppConfiguration.h"

///////////////////////////////////////////////////////////////////////////////

#define LFS_READ_SIZE 			(128)
#define LFS_PROG_SIZE 			(LFS_READ_SIZE)
#define LFS_LOOKAHEAD_SIZE      (LFS_READ_SIZE / 8)
#define LFS_BLOCK_SIZE          (65536)
#define LFS_BLOCK_COUNT 		(128)
#define LFS_BLOCK_CYCLES 		(-1)
#define LFS_CACHE_SIZE          (64 % LFS_PROG_SIZE == 0 ? 64 : LFS_PROG_SIZE)
#define LFS_ERASE_VALUE 		(0xff)
#define LFS_ERASE_CYCLES 		(-1)
#define LFS_BADBLOCK_BEHAVIOR 	(LFS_TESTBD_BADBLOCK_PROGERROR)

///////////////////////////////////////////////////////////////////////////////

static uint8_t lfs_read_buf[LFS_READ_SIZE];
static uint8_t lfs_prog_buf[LFS_PROG_SIZE];
static __attribute__ ((aligned (32))) uint8_t lfs_lookahead_buf[LFS_LOOKAHEAD_SIZE];	// 128/8=16

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief read function provided to LitleFS
 * 
 * @param c 
 * @param block 
 * @param off 
 * @param buffer 
 * @param size 
 * @return int 
 */
static int lfs_device_read(const struct lfs_config *c, lfs_block_t block,
	lfs_off_t off, void *buffer, lfs_size_t size)
{    
   return W25qxx_ReadBlock((uint8_t*)buffer, block, off, size);
}

/**
 * @brief read function provided to LitleFS
 * 
 * @param c 
 * @param block 
 * @param off 
 * @param buffer 
 * @param size 
 * @return int 
 */
static int lfs_device_prog(const struct lfs_config *c, lfs_block_t block,
	lfs_off_t off, const void *buffer, lfs_size_t size)
{
    return W25qxx_WriteBlock((uint8_t*)buffer, block, off, size  );	
}

/**
 * @brief Erase function provided to LitleFS
 * 
 * @param c 
 * @param block 
 * @return int 
 */
static int lfs_device_erase(const struct lfs_config *c, lfs_block_t block)
{    
    return W25qxx_EraseBlock(block);
}

/**
 * @brief Erase function provided to LitleFS. Not supported by W25qxx
 * 
 * @param c 
 * @return int 
 */
static int lfs_device_sync(const struct lfs_config *c)
{
	return 0;
}

///////////////////////////////////////////////////////////////////////////////


/**
 * @brief configuration of the filesystem is provided by this struct
 * 
 */
static const struct lfs_config cfg = {
    // block device operations
    .read  = lfs_device_read,
    .prog  = lfs_device_prog,
    .erase = lfs_device_erase,
    .sync  = lfs_device_sync,

    // block device configuration
    .read_size = LFS_READ_SIZE,
    .prog_size = LFS_PROG_SIZE,
    .block_size = LFS_BLOCK_SIZE,
    .block_count = LFS_BLOCK_COUNT,
    .cache_size = LFS_CACHE_SIZE,
    .lookahead_size = LFS_LOOKAHEAD_SIZE,
    .block_cycles = LFS_BLOCK_CYCLES,
    
    .read_buffer = lfs_read_buf,
	.prog_buffer = lfs_prog_buf,
	.lookahead_buffer = lfs_lookahead_buf,
};

    
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Wrapper around littleFS initialization
 * 
 * @param plfs pointer to lfs structure 
 * @return int non zero if error 
 */
int lfsWrapper_Init(lfs_t* const plfs)
{
    int err = lfs_mount(plfs, &cfg);
    /**< reformat if we can't mount the filesystem */ 
    /**< this should only happen on the first boot */ 
    if (err) {
        err = lfs_format(plfs, &cfg);
        err |= lfs_mount(plfs, &cfg);
    }
    return err;
}

///////////////////////////////////////////////////////////////////////////////

#ifdef ENABLE_TESTS_DEFINITIONS

/**
 * @brief Low level Test function for littleFS
 * 
 * @return int 
 */
bool lfs_Test(void) 
{
    // variables used by the file-system
    lfs_t lfs;
    lfs_file_t file;

    W25qxx_Init();
    // mount the filesystem
    int err = lfs_mount(&lfs, &cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        err = lfs_format(&lfs, &cfg);
        err |= lfs_mount(&lfs, &cfg);
    }

    // read current count
    uint32_t boot_count = 0;
    
    if(0 == err)
    {
        lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
        lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

        Console_Print(eCONSOLE_PRINT_LVL0, "\r\n>> Boot count %u", boot_count);
        // update boot count
        boot_count += 1;
        lfs_file_rewind(&lfs, &file);
        lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

        // remember the storage is not updated until the file is closed successfully
        lfs_file_close(&lfs, &file);

    }

    // release any resources we were using
    lfs_unmount(&lfs);

    // print the boot count
    printf("boot_count: %lu\n", (unsigned long)boot_count);
    
    return (0 == err);
}

#endif


///////////////////////////////////////////////////////////////////////////////
