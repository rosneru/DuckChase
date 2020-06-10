 
cmake_minimum_required (VERSION 2.8.11)


#project (DokeyKong-Amiga-C)

set(CROSS_COMPILE m68k-amigaos-)

SET(CMAKE_CXX_COMPILER m68k-amigaos-g++)

SET(CMAKE_CXX_FLAGS_DEBUG "-noixemul -fomit-frame-pointer -fno-rtti -Wall -O0 -g -Wl,--amiga-debug-hunk -ldebug")
SET(CMAKE_CXX_FLAGS_RELEASE "-noixemul -fomit-frame-pointer -fno-rtti -Wall -Os -s")


set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <OBJECTS>  -o <TARGET>")

add_executable (DuckChase
                amigautils/StopWatch.cpp
                anim/Animator.h
                anim/AnimSeqBase.cpp
                anim/AnimSeqBitmap.cpp
                anim/AnimSeqExtSprite.cpp
                anim/AnimSeqGels.cpp
                anim/AnimSeqGelsBob.cpp
                anim/AnimSeqGelsVSprite.cpp
                entities/Arrow.cpp
                entities/Duck.cpp
                entities/EntityBase.cpp
                entities/EntityCollection.cpp
                entities/Hunter.cpp
                game/Game.cpp
                game/GameColors.cpp
                game/GameTile.cpp
                game/GameWorld.cpp
                game/InfoDisplay.cpp
                game/Level01.cpp
                main.cpp
                picture/BitmapPictureBase.cpp
                picture/PictureBase.cpp
                picture/IlbmBitmap.cpp
                picture/ImageDataPicture.cpp
                picture/RawBitmap.cpp
                resources/ArrowResources.cpp
                resources/DuckResources.cpp
                resources/HunterResources.cpp
                shapes/GelsLayer.cpp
                shapes/Rect.cpp
                shapes/ShapeBase.cpp
                shapes/ShapeBob.cpp
                shapes/ShapeExtSprite.cpp
                shapes/ShapeVSprite.cpp
                view/GameViewBase.cpp
                view/GameViewIntui.cpp
                view/GameViewLowLevel.cpp
                view/lowlevelview.cpp
                view/lowlevelviewport.cpp)


target_include_directories (DuckChase PUBLIC
                            amigautils
                            anim
                            entities
                            game
                            picture
                            resources
                            shapes
                            view
                            .)

add_custom_command(TARGET DuckChase
                   POST_BUILD
                   COMMAND cp DuckChase ${CMAKE_CURRENT_SOURCE_DIR}/..
                   DEPENDS DuckChase)
