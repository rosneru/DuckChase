 
cmake_minimum_required (VERSION 2.8.11)


#project (DokeyKong-Amiga-C)

set(CROSS_COMPILE m68k-amigaos-)

SET(CMAKE_CXX_COMPILER m68k-amigaos-g++)

SET(CMAKE_CXX_FLAGS_DEBUG "-noixemul -fomit-frame-pointer -fno-rtti -Wall -O0 -g -Wl,--amiga-debug-hunk -ldebug")
SET(CMAKE_CXX_FLAGS_RELEASE "-noixemul -fomit-frame-pointer -fno-rtti -Wall -Os -s")


set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <OBJECTS>  -o <TARGET>")

add_executable (DonkeyKong
                amigautils/StopWatch.cpp
                anim/Animator.h
                anim/AnimSeqBase.cpp
                anim/AnimSeqExtSprite.cpp
                anim/AnimSeqGels.cpp
                anim/AnimSeqGelsBob.cpp
                anim/AnimSeqGelsVSprite.cpp
                entities/BrownBarrel.cpp
                entities/EntityBase.cpp
                entities/EntityCollection.cpp
                entities/Jumpman.cpp
                game/Game.cpp
                game/GameColors.cpp
                game/GameTile.cpp
                game/GameWorld.cpp
                game/InfoDisplay.cpp
                main.cpp
                picture/BitMapPictureBase.cpp
                picture/PictureBase.cpp
                picture/IlbmBitMap.cpp
                picture/ImageDataPicture.cpp
                picture/RawBitMap.cpp
                resources/BrownBarrelResources.cpp
                resources/JumpmanResources.cpp
                shapes/GelsLayer.cpp
                shapes/ShapeBase.cpp
                shapes/ShapeBob.cpp
                shapes/ShapeExtSprite.cpp
                shapes/ShapeVSprite.cpp
                view/GameViewBase.cpp
                view/GameViewIntui.cpp
                view/GameViewLowLevel.cpp
                view/lowlevelview.cpp
                view/lowlevelviewport.cpp)


target_include_directories (DonkeyKong PUBLIC
                            amigautils
                            anim
                            entities
                            game
                            picture
                            resources
                            shapes
                            view
                            .)

add_custom_command(TARGET DonkeyKong
                   POST_BUILD
                   COMMAND cp DonkeyKong ${CMAKE_CURRENT_SOURCE_DIR}/..
                   DEPENDS DonkeyKong)


#add_executable (Picture_test_ui
#                amigautils/Picture.cpp
#                testing_tools/Picture_test_ui.cpp)
#
#target_include_directories (Picture_test_ui PUBLIC
#                            amigautils
#                            testing_tools)
#
#add_custom_command(TARGET Picture_test_ui
#                   POST_BUILD
#                   COMMAND cp Picture_test_ui ${CMAKE_CURRENT_SOURCE_DIR}/..
#                   DEPENDS Picture_test_ui)
#
#
#add_executable (Picture_test_shell
#                Picture/BitMapPicture.cpp
#                testing_tools/Picture_test_shell.cpp)
#
#target_include_directories (Picture_test_shell PUBLIC
#                            Picture
#                            testing_tools)
#
#add_custom_command(TARGET Picture_test_shell
#                   POST_BUILD
#                   COMMAND cp Picture_test_shell ${CMAKE_CURRENT_SOURCE_DIR}/..
#                   DEPENDS Picture_test_shell)
