#ifndef GAME_COLORS_H
#define GAME_COLORS_H

#include <exec/types.h>



/**
 * Class to store the colors of the game.
 *
 * @author Uwe Rosner
 * @date 24/01/2020
 */
class GameColors
{
public:

  GameColors();
  virtual ~GameColors();

  bool Load();

  /**
   * Return the ColorArray for the view as Amiga-LoadRGB32-compliant
   * array
   */
  ULONG* GetRGB32View();

  /**
   * Return the ColorArray for the arrow sprite.
   * 
   * Has a size of 9: 3 rows, and values r, g, b for in each row.
   * 
   * NOTE: This can not be used for LoadRGB32. It has no header longword
   * and no terminating \0 longword. Use it only with SetRGB32.
   * array
   */
  ULONG* GetRGB32ArrowSprite();


private:
  // It's useful to store the colors at a central place.
  // This struct is a workaround for C++98 not allowing array 
  // initialization in the initializer list.
  //
  // It is defined to a size of 98 because:
  //   1..Header, 
  //   3 x 32 = 96..the 32 colors, 
  //   1..Termination
  //
  struct ViewColors
  { 
    ULONG elem[98];
  };

  // Declaring a static variable to hold the ColorArray data.
  // See top of the implementation file, there it is defined
  // and initialized
  static ViewColors m_sViewColors;

  // Declaring a member variable for the Color array. In the
  // constructor it is initialized with the static array data.
  ViewColors m_RGB32View;

  struct ArrowSpriteColors
  {
    ULONG elem[9];
  };

  static ArrowSpriteColors m_sArrowSpriteColors;

  ArrowSpriteColors m_RGB32ArrowSprite;
};

#endif // GAME_COLORS_H
