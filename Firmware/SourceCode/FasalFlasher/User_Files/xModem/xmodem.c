/**
 * @file xmodem.c
 * @author Vishal keshava Murthy 
 * @brief Modified Xmodem module which transfers file received over serial to Storage medium
 * @version 0.1
 * @date 2024-02-22
 * 
 * @copyright Copyright (c) 2024
 * @note : Original module source https://github.com/ferenc-nemeth
 * 
 */

///////////////////////////////////////////////////////////////////////////////

#include <string.h>

#include "xmodem.h"
#include "Console.h"
#include "AppFlash_API.h"

///////////////////////////////////////////////////////////////////////////////

/* Global variables. */
static uint8_t gxModemPacketNumber = 1u;          /**< Packet number counter. */
static uint8_t gxModemIsFirstPacket = false;      /**< First packet or not. */

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief   Calculates the CRC-16 for the input package.
 * @param   *data:  Array of the data which we want to calculate.
 * @param   length: Size of the data, either 128 or 1024 bytes.
 * @return  status: The calculated CRC.
 */
static uint16_t xmodem_computeCRC(uint8_t *data, uint16_t length)
{
    uint16_t crc = 0u;
    while (length)
    {
        length--;
        crc = crc ^ ((uint16_t)*data++ << 8u);
        for (uint8_t i = 0u; i < 8u; i++)
        {
            if (crc & 0x8000u)
            {
                crc = (crc << 1u) ^ 0x1021u;
            }
            else
            {
                crc = crc << 1u;
            }
        }
    }
    return crc;
}

/**
 * @brief   This function handles the data packet we get from the xmodem protocol.
 * @param   header: SOH or STX.
 * @return  status: Report about the packet.
 */
static xmodem_status xmodem_handlePacket(uint8_t header)
{
  xmodem_status status = X_OK;
  uint16_t size = 0u;

  /* 2 bytes for packet number, 1024 for data, 2 for CRC*/
  uint8_t received_packet_number[X_PACKET_NUMBER_SIZE];

  uint8_t received_packet_data[X_PACKET_1024_SIZE];

  uint8_t received_packet_crc[X_PACKET_CRC_SIZE];

  /* Get the size of the data. */
  if (X_SOH == header)
  {
    size = X_PACKET_128_SIZE;
  }
  else if (X_STX == header)
  {
    size = X_PACKET_1024_SIZE;
  }
  else
  {
    /* Wrong header type. This shoudn't be possible... */
    status |= X_ERROR;
  }

  eConsolePrintStatus_t comm_status = eCONSOLE_SUCCESS;
  /* Get the packet number, data and CRC from UART. */
  comm_status |= Console_receive(&received_packet_number[0u], X_PACKET_NUMBER_SIZE);
  comm_status |= Console_receive(&received_packet_data[0u], size);
  comm_status |= Console_receive(&received_packet_crc[0u], X_PACKET_CRC_SIZE);
  /* Merge the two bytes of CRC. */
  uint16_t crc_received = ((uint16_t)received_packet_crc[X_PACKET_CRC_HIGH_INDEX] << 8u) | ((uint16_t)received_packet_crc[X_PACKET_CRC_LOW_INDEX]);
  /* We calculate it too. */
  uint16_t crc_calculated = xmodem_computeCRC(&received_packet_data[0u], size);

  /* Communication error. */
  if (eCONSOLE_SUCCESS != comm_status)
  {
    status |= X_ERROR_UART;
  }

  /* If it is the first packet, then erase the memory. */
  if ((X_OK == status) && (false == gxModemIsFirstPacket))
  {
	if (eFS_SUCCESS == FlashFs_API_OpenGoldenImageFile())
	{
	  gxModemIsFirstPacket = true;
	}
    else
    {
      status |= X_ERROR_FLASH;
    }
  }

  /* Error handling and flashing. */
  if (X_OK == status)
  {
    if (gxModemPacketNumber != received_packet_number[0u])
    {
      /* Packet number counter mismatch. */
      status |= X_ERROR_NUMBER;
    }
    if (255u != (received_packet_number[X_PACKET_NUMBER_INDEX] + received_packet_number[X_PACKET_NUMBER_COMPLEMENT_INDEX]))
    {
      /* The sum of the packet number and packet number complement aren't 255. */
      /* The sum always has to be 255. */
      status |= X_ERROR_NUMBER;
    }
    if (crc_calculated != crc_received)
    {
      /* The calculated and received CRC are different. */
      status |= X_ERROR_CRC;
    }
  }

  if ((X_OK == status) && (eFS_SUCCESS != FlashFs_API_WriteToGoldenImageFile((char*)&received_packet_data[0u], (uint32_t)size)))
  {
    /* Flashing error. */
    status |= X_ERROR_FLASH;
  }


  /* Raise the packet number and the address counters (if there weren't any errors). */
  if (X_OK == status)
  {
    gxModemPacketNumber++;
  }

  return status;
}

/**
 * @brief   Handles the xmodem error.
 *          Raises the error counter, then if the number of the errors reached critical, do a graceful abort, otherwise send a NAK.
 * @param   *error_number:    Number of current errors (passed as a pointer).
 * @param   max_error_number: Maximal allowed number of errors.
 * @return  status: X_ERROR in case of too many errors, X_OK otherwise.
 */
static xmodem_status xmodem_errorHandler(uint8_t *error_number, uint8_t max_error_number)
{
  xmodem_status status = X_OK;
  /* Raise the error counter. */
  (*error_number)++;
  /* If the counter reached the max value, then abort. */
  if ((*error_number) >= max_error_number)
  {
    /* Graceful abort. */
    (void)Console_TransmitChar(X_CAN);
    (void)Console_TransmitChar(X_CAN);
    FlashFs_API_DeleteGoldenImageFile();
    status = X_ERROR;
  }
  /* Otherwise send a NAK for a repeat. */
  else
  {
    (void)Console_TransmitChar(X_NAK);
    status = X_OK;
  }
  return status;
}

///////////////////////////////////////////////////////////////////////////////

/**
 * @brief   This function is the base of the Xmodem protocol.
 *          When we receive a header from UART, it decides what action it shall take.
 * @param   void
 * @return  xmodem_status
 */
xmodem_status xmodem_API_receive(void)
{
  volatile xmodem_status status = X_OK;
  uint8_t error_number = 0u;

  gxModemIsFirstPacket = false;
  gxModemPacketNumber = 1u;

  /* Loop until there isn't any error (or until we jump to the user application). */
  while (X_OK == status)
  {
    uint8_t header = 0x00u;

    /* Get the header from UART. */
    eConsolePrintStatus_t comm_status = Console_receive(&header, 1u);

    /* Spam the host (until we receive something) with ACSII "C", to notify it, we want to use CRC-16. */
    if ((eCONSOLE_SUCCESS != comm_status) && (false == gxModemIsFirstPacket))
    {
      (void)Console_TransmitChar(X_C);
    }
    /* Uart timeout or any other errors. */
    else if ((eCONSOLE_SUCCESS != comm_status) && (true == gxModemIsFirstPacket))
    {
      status = xmodem_errorHandler(&error_number, X_MAX_ERRORS);
    }
    else
    {
      /* Do nothing. */
    }

    xmodem_status packet_status = X_ERROR;
    /* The header can be: SOH, STX, EOT and CAN. */
    switch(header)
    {
      /* 128 or 1024 bytes of data. */
      case X_SOH:
      case X_STX:
        /* If the handling was successful, then send an ACK. */
        packet_status = xmodem_handlePacket(header);
        if (X_OK == packet_status)
        {
          (void)Console_TransmitChar(X_ACK);
        }
        /* If the error was flash related, then immediately set the error counter to max (graceful abort). */
        else if (X_ERROR_FLASH == packet_status)
        {
          error_number = X_MAX_ERRORS;
          status = xmodem_errorHandler(&error_number, X_MAX_ERRORS);
        }
        /* Error while processing the packet, either send a NAK or do graceful abort. */
        else
        {
          status = xmodem_errorHandler(&error_number, X_MAX_ERRORS);
        }
        break;
      /* End of Transmission. */
      case X_EOT:
        /* ACK, feedback to user (as a text), then jump to user application. */
        (void)Console_TransmitChar(X_ACK);
        status = X_COMPLETE;
        FlashFs_API_CloseGoldenImageFile();
        break;
      /* Abort from host. */
      case X_CAN:
        status = X_ERROR;
        break;
      default:
        /* Wrong header. */
        if (eCONSOLE_SUCCESS == comm_status)
        {
          status = xmodem_errorHandler(&error_number, X_MAX_ERRORS);
        }
        break;
    }
  }

  return status;
}

///////////////////////////////////////////////////////////////////////////////
