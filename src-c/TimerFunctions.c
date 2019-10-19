#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include "TimerFunctions.h"

/// Timer global variables

ULONG m_ClocksPerSecond;
BOOL m_bInitialized;
struct EClockVal m_StartClock;
struct EClockVal m_StopClock;
struct IORequest* m_pIORequest;
struct MsgPort* m_pMsgPort;

struct Device* TimerBase = NULL;

///

/// Timer functions

BOOL InitializeTimer()
{
  m_bInitialized = FALSE;

  // Create a message port
  m_pMsgPort = CreateMsgPort();
  if(m_pMsgPort == NULL)
  {
    return FALSE;
  }

  // Create an IORequest
  m_pIORequest = (struct IORequest*)CreateIORequest(
    m_pMsgPort, sizeof(struct timerequest));

  if(m_pIORequest == NULL)
  {
    DeleteMsgPort(m_pMsgPort);
    m_pMsgPort = NULL;

    return FALSE;
  }

  // Opening the timer.device
  BYTE res = OpenDevice(TIMERNAME, UNIT_ECLOCK, m_pIORequest,
                        TR_GETSYSTIME);
  if(res != 0)
  {
    DeleteIORequest(m_pIORequest);
    m_pIORequest = NULL;

    DeleteMsgPort(m_pMsgPort);
    m_pMsgPort = NULL;

    return FALSE;
  }

  TimerBase = m_pIORequest->io_Device;
  m_bInitialized = TRUE;

  return TRUE;
}


void FreeTimer()
{
  if((TimerBase != NULL) && (m_pIORequest != NULL))
  {
    CloseDevice(m_pIORequest);
    TimerBase = NULL;

    DeleteIORequest(m_pIORequest);
    m_pIORequest = NULL;
  }

  if(m_pMsgPort != NULL)
  {
    DeleteMsgPort(m_pMsgPort);
  }
}


void StartTimer()
{
  if(m_bInitialized == FALSE)
  {
    return;
  }

  m_ClocksPerSecond = ReadEClock(&m_StartClock);
}


double PickTime(BOOL bKeepStartPoint)
{
  if(m_bInitialized == FALSE)
  {
    return -1.0;
  }

  if(m_ClocksPerSecond == 0)
  {
    return -2.0;
  }

    // Reading the eclock value again
  ReadEClock(&m_StopClock);

    // Calculating elapsed time in seconds
  double seconds = m_StopClock.ev_lo - m_StartClock.ev_lo;
  seconds /= (double)m_ClocksPerSecond;

  if(bKeepStartPoint == FALSE)
  {
    ReadEClock(&m_StartClock);
  }

    // Returning the elapsed time in milliseconds
  return seconds * 1000.0;
}

///

