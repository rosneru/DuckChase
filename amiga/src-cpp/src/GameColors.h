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



private:
  // It's useful to store the colors at a central place
  // This struct is a workaround for C++98 not allowing array 
  // initialization in th initializer list.
  //
  // It is defined to a size of 26 because:
  //   1..Header, 
  //   3x8=24..the 8 colors, 
  //   1..Termination
  //
  struct ViewColors
  { 
    ULONG elem[26];
  };

  // Declaring a static variable to hold the ColorArray data.
  // See top of the implementation file, there it is defined
  // and initialized
  static ViewColors m_sViewColors;

  // Declaring a member variable for the Color array. In the
  // constructor it is initialized with the static array data.
  ViewColors m_RGB32View;
};

#endif // GAME_COLORS_H
