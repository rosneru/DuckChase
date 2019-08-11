#ifndef I_ENTITY_H
#define I_ENTITY_H


/**
 * Represents an entity which populates the game world.
 *
 *
 * @author Uwe Rosner
 * @date 11/08/2019
 */
class IEntity
{
public:
  /**
   * Initialization, loading images etc.
   */
  virtual bool Init() = 0;

  /**
   * Updates this entity in the game world.
   */
  virtual void Update(unsigned long elapsed, 
                      unsigned long joyPortState) = 0;

};

#endif
