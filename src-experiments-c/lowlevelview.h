#ifndef LOWLEVELVIEW_H
#define LOWLEVELVIEW_H


#include <exec/types.h>
#include <graphics/view.h>
#include <graphics/videocontrol.h>
#include <graphics/monitor.h>
#include <graphics/modeid.h>
#include <graphics/displayinfo.h>

struct View* CreateAView(APTR pMemoryPool, ULONG modeid);
void DeleteAView(struct View *pView);

struct ViewPort* CreateAViewPort(APTR pMemoryPool,
                                 ULONG sizex,
                                 ULONG sizey,
                                 ULONG depth,
                                 ULONG modeid,
                                 ULONG colors);

void DeleteAViewPort(struct ViewPort *pViewPort);


struct View* CreateBView(APTR pMemoryPool, ULONG modeid);
void DeleteBView(struct View *pView);

struct ViewPort* CreateBViewPort(APTR pMemoryPool,
                                 ULONG sizex,
                                 ULONG sizey,
                                 ULONG depth,
                                 ULONG modeid,
                                 ULONG colors);

void DeleteBViewPort(struct ViewPort *pViewPort);

#endif
