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
   * Return the ColorArray for the arrow sprite.
   * 
   * Has a size of 9: 3 rows, and values r, g, b for in each row.
   * 
   * NOTE: This can not be used for LoadRGB32. It has no header longword
   * and no terminating \0 longword. Use it only with SetRGB32.
   * array
   */
  WORD* GetBarrelBrownSpriteColors();

  ULONG* GetStrainSpreadColors();


private:
  /**
   * Type for storing the colors for the arrow sprite.
   * Defined to an ULONG array of size 9.
   */
  struct BarrelBrownSpriteColors
  {
    WORD elem[3];
  };

  /**
   * Type for storing the color gradient of the strain display.
   * Defined to an ULONG array of size 192.
   */
  struct StrainSpreadColors
  {
    ULONG elem[192];
  };

  /**
   * Static variable to hold the arrow sprite color data
   */
  static BarrelBrownSpriteColors m_sArrowSpriteColors;

  /**
   * Static variable to hold the arrow sprite color data
   */
  static StrainSpreadColors m_sStrainSpreadColors;
};

#endif // GAME_COLORS_H
