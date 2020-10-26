# Add an option to auto-detect the frame width

Add an algorithm that:
- create a method countObjectsHorizontally() to count the number of
  objects in the current image. Method returns {vector<Rect>} with all
  the actual postion of all objects in the picture
- determine the {numObjects = countObjectsHorizontally().size}
- calculate the {frameWidth = loadedPictureWidth / numObjects}
frameWidthdivide the width of loaded picture by the number

# Add an option to create a new strip with the targetFrameWidth as parameter

When a picture containing anim frames is loaded, this function would:
- {numObjects = countObjectsHorizontally().size()} (see above)
- parse the maximumObjectWidth of counted objects {vector<Rect>}
- fail when maximumObjectWidth > targetFrameWidth
- calculate {newPictureWidth = targetFrameWidth * numObjects}
- create a new BitMap of calculated size
- blit all the {numObjects} centered into the target picture frame rects

# Support for AMOS abk

## Amos Sprite bank format

Offset  Length    Description
-----------------------------------------------------------------------
0       4 Bytes   ASCII identifier AmSp (sprites, load to bank 1) or 
                                   AmIc (icons, load to bank 2) 
4       2 Bytes   The number of sprites / icons to follow

6       ? Bytes   Sprite / icon data. Each sprite/icon is individually 
                  sized and has this format:

                  Offset  Length    Description
                  ----------------------------------------------------
                  0       2 Bytes   width of the sprite/icon, in 16-bit 
                                    words rather than pixels (w)

                  2       2 Bytes   height of the sprite/icon, in pixels (h)

                  4       2 Bytes   depth of the sprite/icon, in bitplanes (1-5) 

                  6       2 Bytes   hot-spot X co-ordinate

                  8       2 Bytes   hot-spot Y co-ordinate 

                 10 w*2*h*d Bytes   planar graphic data: plane 0 data 
                                    first, then planes 1, 2, 3, 4 if present

6 + ?   64      A 32-entry colour palette. Each entry has the Amiga 
                COLORx hardware register format, $0RGB

## Thoughts about abk support

An AMOS abk file could be parsed. Whenever the width of a sprite differs
from the previous one:
- a new 'sheet' will be created
- each sheet has its own BitMap with all the subsequent sprites with the
  same size from the abk blit on it
- In the gui the different sheets could be toggled by a ToggleGadget.
- Loaders and savers for abk must be created which reflect this sheets /
  BitMap concept.
  