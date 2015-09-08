/*******************************************************************************
* SimpleMAC sample application: transmit.h
*
* transmit.c handles packet transmission for the SimpleMAC sample application.
*
* COPYRIGHT 2010 STMicroelectronics. All rights reserved. 
*******************************************************************************/

/**
 * @brief This is a debugging macro for printing transmit details.
 * \b WARNING: THIS MACRO WILL CAUSE PRINTS FROM ISR CONTEXT!
 */
#define TX_DETAILS(x)           

/**
 * @brief Define the size of the direct transmit packet queue.
 */
#define TX_DIRECT_QUEUE_SIZE   (5)

/**
 * @brief The total number of transmit attempts when a MAC ACK should
 * have been received or when CCA fails.  This count includes the very
 * first transmission.
 */
#define TX_ATTEMPTS_COUNT      (4)


/**
 * @brief Indicates if a packet is currently in flight.
 */
extern boolean txPacketInFlight;

/**
 * @brief Indicates that the frame pending bit was set and there is a
 * new packet enroute to this node..
 */
extern boolean framePendingPktEnroute;

/**
 * @brief The system time, in milliseconds, when the set frame pending
 * bit was seen by ST_RadioTransmitCompleteIsrCallback.
 */
extern u16 framePendingMsTick;


/**
 * @brief The tick function for driving the transmit queues.  This
 * function should be called as often as possible.
 */
void txTick(void);

/**
 * @brief Copy a packet onto the transmit queue.
 *
 * @param direct  If TRUE the packet is placed on the direct queue.  If
 * FALSE the packet is placed on the indirect queue.
 *
 * @param dstShortAddr  The short address of the destination.  If long
 * addressing is used, this parameter is ignored.
 * 
 * @param packet  A pointer to the packet being sent.  The packet is
 * copied into the transmit queue's local packet buffer.
 * 
 * @param sfdPtrOffset  A byte offset into the packet where the transmit
 * SFD time should be injected.  If this parameter is 0, the SFD time is
 * not injected into the packet.
 */
void enqueueTxPacket(boolean direct,
                     u16 dstShortAddr,
                     u8 * packet,
                     u8 sfdPtrOffset);

/**
 * @brief Tells the indirect queue to mark the next packet as active and
 * to send it now.
 */
void transmitIndirectPacketNow(void);

/**
 * @brief Looks into the indirect queue and determines if there is data
 * pending for the given short address.
 *
 * @param shortId  The short address of the node polling for data.
 *
 * @return TRUE if there is data pending, FALSE if not.
 */
boolean isDataPendingForShortId(u16 shortId);

/**
 * @brief Clears the direct queue of all packets/data.
 */
void clearDirectQueue(void);

/**
 * @brief Clears the indirect queue of all packets/data.
 */
void clearIndirectQueue(void);

