#ifndef GAME_VIEW_H
#define GAME_VIEW_H


/**
 * Represents a graphics.library constructed, double buffered view
 * to be used for gaming purposes etc.
 *
 * @author Uwe Rosner
 * @date 29/07/2019
 */
class GameView
{
public:
  GameView();
  ~GameView();

private:


  void clear();
};

#endif
