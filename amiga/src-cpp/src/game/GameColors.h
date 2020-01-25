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
  ULONG* GetViewColorsLoadRGB32();

  /**
   * Return the ColorArray for the arrow sprite.
   * 
   * Has a size of 9: 3 rows, and values r, g, b for in each row.
   * 
   * NOTE: This can not be used for LoadRGB32. It has no header longword
   * and no terminating \0 longword. Use it only with SetRGB32.
   * array
   */
  ULONG* GetArrowSpriteColors();


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

  /**
   * Type for storing the colors for the view. It is defined to a size
   * of 98 because: 
   *   1..Header, 
   *   3 x 32 = 96.. for the 32 colors, 
   *   1..Termination
   */
  struct ViewColors
  { 
    ULONG elem[98];
  };

  /**
   * Type for storing the collors for the arrow sprite.
   */
  struct ArrowSpriteColors
  {
    ULONG elem[9];
  };

  /**
   * Static variables to hold the view color data
   */
  static ViewColors m_sViewColors;

  /**
   * Static variable to hold the arrow sprite color data
   */
  static ArrowSpriteColors m_sArrowSpriteColors;
};

#endif // GAME_COLORS_H
