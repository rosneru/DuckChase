# About

Imagine you have a wide picture that contains anim frames like a film
strip. This tool allows to move the content of each frame to left,
right, top or bottom without affecting the other frames. The content
can't be moved out of the frame border. So, movement in a direction is
only possible until the first non-background pixel hits its frame
border.

Then, the result picture can be saved to an IFF ILBM  file.

# Basic usage

## Load an IFF / ILBM file

The display of the tool is split: a control screen at the top, a
canvas screen at the bottom. 

After opening a picture:

* the picture is displayed in the canvas screen

* the default raster width is set to 16 (the smallest possible value)

* the number of frames is calculated according to the picture width 
and raster width {numFrames = loadedPictureWidth / rasterWidth}

* the raster is drawn on top of the picture, the first frame is
selected as the current one; its raster is drawn in another color than
the other frame rasters.

* the content of the current frame (after loadingthe this is the first
one) is displayed in the current frame box of the control screen.


## Adapt frame size

Set the frame width using slider until it fits the animation images.

## Flip through the frames

Use `<cursor-left>` or `<cursor-right>` to flip through the animation
images.

## Move frame content

Use any `<shift + cursor>` key combination to move the content in the
desired direction.

## Save the result

After moving some frame contents, the result can be saved by selecting
menu *Project*, *Save*. Note: Currently it's not possble to select the
output file name or path. The file name is generated automatically by
attaching an _out.ilbm to the input file name. It is saved into the same
directory as the input file resides.
