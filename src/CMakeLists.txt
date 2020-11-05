 
cmake_minimum_required (VERSION 2.8.11)


set(CROSS_COMPILE m68k-amigaos-)

SET(CMAKE_CXX_COMPILER m68k-amigaos-g++)

SET(CMAKE_CXX_FLAGS_DEBUG "-noixemul -fomit-frame-pointer -fno-rtti -Wall -O0 -g -Wl,--amiga-debug-hunk -ldebug")
SET(CMAKE_CXX_FLAGS_RELEASE "-noixemul -fomit-frame-pointer -fno-rtti -Wall -Os -s")


set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <OBJECTS>  -o <TARGET>")

add_executable (DuckChase_gcc
                amigautils/StopWatch.cpp
                anim/Animator.h
                anim/AnimSeqBase.cpp
                anim/AnimSeqBitmap.cpp
                anim/AnimSeqExtSprite.cpp
                anim/AnimSeqGels.cpp
                anim/AnimSeqGelsBob.cpp
                anim/AnimSeqGelsVSprite.cpp
                anim/ShadowMask.cpp
                entities/Arrow.cpp
                entities/ArrowGfxResources.cpp
                entities/Duck.cpp
                entities/DuckGfxResources.cpp
                entities/EntityBase.cpp
                entities/EntityCollection.cpp
                entities/Hunter.cpp
                entities/HunterGfxResources.cpp
                game/Game.cpp
                game/GameColors.cpp
                game/GameTile.cpp
                game/GameVars.cpp
                game/GameWorld.cpp
                game/InfoDisplay.cpp
                game/Level01.cpp
                iff/IffParse.cpp
                main.cpp
                picture/BitMapPictureBase.cpp
                picture/PictureBase.cpp
                picture/OpenIlbmPictureBitMap.cpp
                picture/OpenImageDataPicture.cpp
                picture/OpenRawPictureBitMap.cpp
                shapes/GelsLayer.cpp
                shapes/Rect.cpp
                shapes/ShapeBase.cpp
                shapes/ShapeBob.cpp
                shapes/ShapeExtSprite.cpp
                shapes/ShapeVSprite.cpp
                sound/Octave.cpp
                sound/Soundfile8SVX.cpp
                sound/SoundPlayer.cpp
                view/GameViewBase.cpp
                view/GameViewDBuf.cpp
                view/GameViewIntui.cpp
                view/GameViewLowLevel.cpp
                view/lowlevelview.cpp
                view/lowlevelviewport.cpp)


target_include_directories (DuckChase_gcc PUBLIC
                            amigautils
                            anim
                            entities
                            game
                            iff
                            picture
                            resources
                            shapes
                            sound
                            view
                            .)

add_custom_command(TARGET DuckChase_gcc
                   POST_BUILD
                   COMMAND cp DuckChase_gcc ${CMAKE_CURRENT_SOURCE_DIR}/..
                   DEPENDS DuckChase_gcc)



add_executable (AnimFrameTool_gcc
                anim/Animator.h
                anim/AnimSeqBase.cpp
                anim/AnimSeqBitmap.cpp
                anim/AnimSeqExtSprite.cpp
                anim/AnimSeqGels.cpp
                anim/AnimSeqGelsBob.cpp
                anim/ShadowMask.cpp
                iff/IffParse.cpp
                picture/BitMapPictureBase.cpp
                picture/PictureBase.cpp
                picture/OpenIlbmPictureBitMap.cpp
                picture/OpenImageDataPicture.cpp
                picture/OpenRawPictureBitMap.cpp
                picture/SaveBitMapPictureIlbm.cpp
                shapes/Rect.cpp
                _tool_anim_frame_adjust/AnimFrameTool.cpp
                _tool_anim_frame_adjust/AnimSheetContainer.cpp
                _tool_anim_frame_adjust/AslFileRequest.cpp
                _tool_anim_frame_adjust/BitMapTools.cpp
                _tool_anim_frame_adjust/ChunkyPixelArray.cpp
                _tool_anim_frame_adjust/MessageBox.cpp
                _tool_anim_frame_adjust/OpenAmosAbk.cpp
                _tool_anim_frame_adjust/SaveAmosAbk.cpp
                _tool_anim_frame_adjust/main.cpp)

target_include_directories (AnimFrameTool_gcc PUBLIC
                            _tool_anim_frame_adjust
                            anim
                            iff
                            picture
                            shapes
                            .)

add_custom_command(TARGET AnimFrameTool_gcc
                   POST_BUILD
                   COMMAND cp AnimFrameTool_gcc ${CMAKE_CURRENT_SOURCE_DIR}/..
                   DEPENDS AnimFrameTool_gcc)



add_executable (Collision_test_ui
                anim/Animator.h
                anim/AnimSeqBase.cpp
                anim/AnimSeqBitmap.cpp
                anim/AnimSeqExtSprite.cpp
                anim/AnimSeqGels.cpp
                anim/AnimSeqGelsBob.cpp
                anim/ShadowMask.cpp
                entities/ArrowGfxResources.cpp
                entities/DuckGfxResources.cpp
                iff/IffParse.cpp
                picture/BitMapPictureBase.cpp
                picture/PictureBase.cpp
                picture/OpenIlbmPictureBitMap.cpp
                picture/OpenImageDataPicture.cpp
                picture/OpenRawPictureBitMap.cpp
                shapes/GelsLayer.cpp
                shapes/Rect.cpp
                shapes/ShapeBase.cpp
                shapes/ShapeBob.cpp
                shapes/ShapeExtSprite.cpp
                _tests_amiga/Collision_test_ui.cpp)

target_include_directories (Collision_test_ui PUBLIC
                            anim
                            entities
                            iff
                            picture
                            resources
                            shapes
                            .)

add_custom_command(TARGET Collision_test_ui
                   POST_BUILD
                   COMMAND cp Collision_test_ui ${CMAKE_CURRENT_SOURCE_DIR}/..
                   DEPENDS Collision_test_ui)



add_executable (SoundPlayer_test_shell
                iff/IffParse.cpp
                sound/Octave.cpp
                sound/Soundfile8SVX.cpp
                sound/SoundPlayer.cpp
                _tests_amiga/SoundPlayer_test_shell.cpp)

target_include_directories (SoundPlayer_test_shell PUBLIC
                            iff
                            sound
                            _tests_amiga)

add_custom_command(TARGET SoundPlayer_test_shell
                   POST_BUILD
                   COMMAND cp SoundPlayer_test_shell ${CMAKE_CURRENT_SOURCE_DIR}/..
                   DEPENDS SoundPlayer_test_shell)
