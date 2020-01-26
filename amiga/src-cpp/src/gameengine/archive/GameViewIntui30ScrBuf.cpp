#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <graphics/modeid.h>
#include <graphics/videocontrol.h>

#include "GameViewIntui30ScrBuf.h"

extern struct GfxBase* GfxBase;


GameViewIntui30ScrBuf::GameViewIntui30ScrBuf(short viewWidth,
                                 short viewHeight,
                                 short viewDepth)
  : m_ViewWidth(viewWidth),
    m_ViewHeight(viewHeight),
    m_ViewDepth(viewDepth),
    m_IsDoubleBuffered(true),
    scbuf(),
    dbufport(NULL),
    sigs(0),
    buf_current(0),
    buf_nextdraw(1),
    buf_nextswap(1),
    m_pScreen(NULL),
    m_InitError(IE_None)
{
  m_ViewNumColors = 1L << viewDepth;
}


GameViewIntui30ScrBuf::~GameViewIntui30ScrBuf()
{

}

bool GameViewIntui30ScrBuf::Open(GameColors& colors)
{
  if(m_pScreen != NULL)
  {
    m_InitError = IE_AlreadyInitialized;
    return false;
  }

  m_InitError = IE_None;

  dbufport = CreateMsgPort();
  if(dbufport == NULL)
  {
    m_InitError = IE_CreateMsgPort;
    return false;
  }

  struct TagItem vctags[] =
{
    VTAG_BORDERSPRITE_SET, TRUE,
    TAG_DONE, 0,
};

  m_pScreen = OpenScreenTags(NULL,
    SA_DisplayID, PAL_MONITOR_ID|HIRES_KEY,
    // SA_Overscan, OSCAN_TEXT,
    SA_Depth, m_ViewDepth,
    SA_Top, 0,
    SA_Left, 0,
    SA_Width, m_ViewWidth,
    SA_Height, m_ViewHeight,
    SA_Quiet, TRUE,
    SA_Exclusive, TRUE,
    // SA_BitMap, m_pBitMapArray[0],
    TAG_END);

  if(m_pScreen == NULL)
  {
    m_InitError = IE_OpeningScreen;
    Close();
    return false;
  }

  scbuf[0] = AllocScreenBuffer(m_pScreen, NULL, SB_SCREEN_BITMAP);
  if(scbuf[0] == NULL)
  {
    m_InitError = IE_AllocScreenBuf1;
    return false;
  }

  scbuf[1] = AllocScreenBuffer(m_pScreen, NULL, SB_COPY_BITMAP);
  if(scbuf[1] == NULL)
  {
    m_InitError = IE_AllocScreenBuf2;
    return false;
  }

  scbuf[0]->sb_DBufInfo->dbi_UserData1 = (APTR)(0);
  scbuf[1]->sb_DBufInfo->dbi_UserData1 = (APTR)(1);
  status[0] = OK_REDRAW;
  status[1] = OK_REDRAW;

  InitRastPort(&rport[0]);
  InitRastPort(&rport[1]);
  rport[0].BitMap = scbuf[0]->sb_BitMap;
  rport[1].BitMap = scbuf[1]->sb_BitMap;

  return true;
}

void GameViewIntui30ScrBuf::Close()
{
  if(m_pScreen != NULL)
  {
    if(scbuf[1] != NULL)
    {
      FreeScreenBuffer(m_pScreen, scbuf[1]);
      scbuf[1]  = NULL;
    }

    if(scbuf[0] != NULL)
    {
      FreeScreenBuffer(m_pScreen, scbuf[0]);
      scbuf[0]  = NULL;
    }

    CloseScreen(m_pScreen);
    m_pScreen = NULL;
  }

  if(dbufport != NULL)
  {
    DeleteMsgPort(dbufport);
    dbufport = NULL;
  }
}

void GameViewIntui30ScrBuf::DisableDoubleBuf()
{
  m_IsDoubleBuffered = false;
}

short GameViewIntui30ScrBuf::Width()
{
  return m_ViewWidth;
}


short GameViewIntui30ScrBuf::Height()
{
  return m_ViewHeight;
}


short GameViewIntui30ScrBuf::Depth()
{
  return m_ViewDepth;
}


struct RastPort* GameViewIntui30ScrBuf::RastPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &(m_pScreen->RastPort);
}

struct RastPort* GameViewIntui30ScrBuf::RastPort1()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &rport[0];
}

struct RastPort* GameViewIntui30ScrBuf::RastPort2()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &rport[1];
}


struct ViewPort* GameViewIntui30ScrBuf::ViewPort()
{
  if(m_pScreen == NULL)
  {
    return NULL;
  }

  return &(m_pScreen->ViewPort);
}


void GameViewIntui30ScrBuf::Render()
{
  // //
  // // START OF Current rendering
  // // (or move it to handle bufferswap before WaitBlit..?)
  // SortGList(&(m_pScreen->RastPort));
  // DrawGList(&(m_pScreen->RastPort), &(m_pScreen->ViewPort));

  // // WaitTOF();

  // // MrgCop(ViewAddress());    // TODO Avoid multiple calls
  // // LoadView(ViewAddress());

  // //
  // // END OF Current rendering
  // //


  // Check for and handle any double-buffering messages.
  //
  // Note that double-buffering messages are "replied" to us, so we
  // don't want to reply them to anyone.
  if (sigs & (1 << dbufport->mp_SigBit))
  {
    struct Message *dbmsg;
    while (dbmsg = GetMsg(dbufport))
    {
      handleDBufMessage(dbmsg);
    }
  }


  ULONG held_off = 0;
  held_off = handleBufferSwap();

  if (held_off)
  {
    // If were held-off at ChangeScreenBuffer() time, then we need to
    // try ChangeScreenBuffer() again, without awaiting a signal.
    // We WaitTOF() to avoid busy-looping.
    WaitTOF();
  }
  else
  {
    // If we were not held-off, then we're all done with what we have
    // to do. We'll have no work to do until some kind of signal
    // arrives. This will normally be the arrival of the
    // dbi_SafeMessage from the ROM double-buffering routines, but in
    // another example it might also be an IntuiMessage.
    sigs = Wait(1 << dbufport->mp_SigBit);
  }

}

const char* GameViewIntui30ScrBuf::LastError() const
{
  switch(m_InitError)
  {
    case IE_None:
      return "No error: init done successfully.";
      break;

    case IE_AlreadyInitialized:
      return "GameView already initialized.";
      break;

    case IE_OpeningScreen:
      return "Could not open the screen.\n";
      break;

    case IE_CreateMsgPort:
      return "Could not create the msg port for double buffering.\n";
      break;

    case IE_AllocScreenBuf1:
      return "Could not allocate the screen buffer 1.\n";
      break;

    case IE_AllocScreenBuf2:
      return "Could not allocate the screen buffer 2.\n";
      break;

    default:
      return "Unknown state";
      break;
  }
}

const char* GameViewIntui30ScrBuf::ViewName() const
{
  return "Intuition view (OS3.0+)";
}


ULONG GameViewIntui30ScrBuf::handleBufferSwap()
{
  ULONG held_off = 0;

  // 'buf_nextdraw' is the next buffer to draw into.
  // The buffer is ready for drawing when we've received the
  // dbi_SafeMessage for that buffer.  Our routine to handle
  // messaging from the double-buffering functions sets the
  // OK_REDRAW flag when this message has appeared.
  //
  // Here, we set the OK_SWAPIN flag after we've redrawn
  // the imagery, since the buffer is ready to be swapped in.
  // We clear the OK_REDRAW flag, since we're done with redrawing
  //
  if (status[buf_nextdraw] == OK_REDRAW)
  {
    SortGList(&(m_pScreen->RastPort));
    DrawGList(&(m_pScreen->RastPort), &(m_pScreen->ViewPort));

    WaitTOF();

    MrgCop(ViewAddress());    // TODO Avoid multiple calls
    LoadView(ViewAddress());

    //WaitBlit(); // Gots to let the BBMRP finish

    status[buf_nextdraw] = OK_SWAPIN;

    // Toggle which the next buffer to draw is.
    // If you're using multiple ( >2 ) buffering, you
    // would use
    //
    //    buf_nextdraw = ( buf_nextdraw+1 ) % NUMBUFFERS;
    //
    //
    buf_nextdraw ^= 1;
  }

  // Let's make sure that the next frame is rendered before we swap..
  if (status[buf_nextswap] == OK_SWAPIN)
  {
    scbuf[buf_nextswap]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = dbufport;

    if (ChangeScreenBuffer(m_pScreen, scbuf[buf_nextswap]))
    {
      status[buf_nextswap] = 0;

      buf_current = buf_nextswap;

      // Toggle which the next buffer to swap in is.
      // If you're using multiple ( >2 ) buffering, you
      // would use
      //
      //    buf_nextswap = ( buf_nextswap+1 ) % NUMBUFFERS;
      //
      //
      buf_nextswap ^= 1;

    }
    else
    {
      held_off = 1;
    }
  }

  return(held_off);
}

  void GameViewIntui30ScrBuf::handleDBufMessage(struct Message *dbmsg)
  {
    ULONG buffer;

    // dbi_SafeMessage is followed by an APTR dbi_UserData1, which
    // contains the buffer number. This is an easy way to extract
    // it. The dbi_SafeMessage tells us that it's OK to redraw the
    // in the previous buffer.
    buffer = (ULONG) *((APTR **)(dbmsg + 1));

    // Mark the previous buffer as OK to redraw into.
    // If you're using multiple ( >2 ) buffering, you
    // would use
    //
    //    ( buffer + NUMBUFFERS - 1 ) % NUMBUFFERS
    //
    //
    status[buffer ^ 1] = OK_REDRAW;
  }