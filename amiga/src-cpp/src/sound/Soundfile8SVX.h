#ifndef SOUNDFILE_8SVX_H
#define SOUNDFILE_8SVX_H

#include "Voice8Header.h"

/**
 * Class for loading an iff 8svx sound file by using the
 * iffparse.library. Code of newiff from Commodore is heavily used.
 *
 * @author Uwe Rosner
 * @date 24/06/2020
 */
class Soundfile8SVX
{
public:
  /**
   * Opens and reads an iff 8svx sound file with given name.
   */
  Soundfile8SVX(const char* pFileName);

  virtual ~Soundfile8SVX();

private:
  struct Voice8Header* m_pVoice8Header;

  bool decode8SVXBody();
};


// /* Used to keep track of allocated IFFHandle, and whether file is
//  * clipboard or not, filename, copied chunks, etc.
//  * This structure is included in the beginning of every
//  * form-specific info structure used by the example modules.
//  */
// struct ParseInfo {
// 	/* general parse.c related */
// 	struct  IFFHandle *iff;		/* to be alloc'd with AllocIFF */
// 	UBYTE	*filename;		/* current filename of this ui */
// 	LONG	*propchks;		/* properties to get */
// 	LONG	*collectchks;		/* properties to collect */
// 	LONG	*stopchks;		/* stop on these (like BODY) */
// 	BOOL    opened;			/* this iff has been opened */
// 	BOOL	clipboard;		/* file is clipboard */
// 	BOOL	hunt;			/* we are parsing a complex file */
// 	BOOL	Reserved1;		/* must be zero for now */		

// 	/* for copychunks.c - for read/modify/write programs
// 	 * and programs that need to keep parsed chunk info
// 	 * around after closing file.
// 	 * Deallocated by freechunklist();
// 	 */
// 	struct Chunk *copiedchunks;

// 	/* application may hang its own list of new chunks here
// 	 * just to keep it with the frame.
// 	 */
// 	struct Chunk *newchunks;

// 	ULONG	Reserved[8];
// 	};


// struct EightSVXInfo {
// 	/* general parse.c related */
// 	struct  ParseInfo ParseInfo;

// 	/* For convenient access to VHDR, Name, and sample.
// 	 * Other chunks will be accessible through FindProp()
//          *  (or findchunk() if the chunks have been copied)
// 	 */
// 	/* 8SVX */
// 	Voice8Header	Vhdr;		

// 	BYTE		*sample;
// 	ULONG		samplebytes;

// 	BYTE		*osamps[MAXOCT];
// 	ULONG		osizes[MAXOCT];
// 	BYTE		*rsamps[MAXOCT];
// 	ULONG		rsizes[MAXOCT];
// 	ULONG		spcycs[MAXOCT];

// 	UBYTE		name[80];

// 	ULONG		Reserved[8];	/* must be 0 for now */

// 	/* Applications may add variables here */
// 	};

#endif
