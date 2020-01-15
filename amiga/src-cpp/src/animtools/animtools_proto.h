/* animtools_proto.h */
#ifndef GELTOOLS_PROTO_H
#define GELTOOLS_PROTO_H


#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>

#include "animtools.h"

struct GelsInfo* setupGelSys(struct RastPort *rPort, BYTE reserved);
VOID cleanupGelSys(struct GelsInfo *gInfo, struct RastPort *rPort);

struct VSprite* makeVSprite(NEWVSPRITE *nVSprite);
struct Bob* makeBob(NEWBOB *nBob);
struct AnimComp* makeComp(NEWBOB *nBob, NEWANIMCOMP *nAnimComp);
struct AnimComp* makeSeq(NEWBOB *nBob, NEWANIMSEQ *nAnimSeq);

VOID freeVSprite(struct VSprite *vsprite);
VOID freeBob(struct Bob *bob, LONG rasdepth);
VOID freeComp(struct AnimComp *myComp, LONG rasdepth);
VOID freeSeq(struct AnimComp *headComp, LONG rasdepth);
VOID freeOb(struct AnimOb *headOb, LONG rasdepth);

#endif
