/**
 * @file AppStorageDataStructures.h
 * @author Vishal Keshava Murthy
 * @brief Common data structures used by Storage modules are defined here
 * @version 0.1
 * @date 2024-06-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#ifndef APPSTORAGE_APPSTORAGEDATASTRUCTURES_H_
#define APPSTORAGE_APPSTORAGEDATASTRUCTURES_H_

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Storage medium statuses defined here
 * 
 */
typedef enum
{
	eFS_SUCCESS,
	eFS_ERROR,
	eFS_NO_FILE,
	eFS_MAX_STATUS,
}eStorageFSStatus_t;

/**
 * @brief User enumeration RW modes for file system
 * 
 */
typedef enum
{
	eFS_READONLY,
	eFS_WRITEONLY,
	eFS_READ_WRITE,
	eFS_READ_CREATE,
	eFS_WRITE_CREATE,
	eFS_WRITE_APPEND,
	eFS_READ_WRITE_CREATE,
	eFS_MODE_MAX
}eStorageFSOperationModes_t;

/**
 * @brief Enums for all the files to be saved in Storage medium
 *
 */
typedef enum
{
	eFS_GOLDEN_IMAGE,
	eFS_FILE_2,
	eFS_MAX
}eStorageFileNamesEnums_t;

///////////////////////////////////////////////////////////////////////////////

#endif /* APPSTORAGE_APPSTORAGEDATASTRUCTURES_H_ */
