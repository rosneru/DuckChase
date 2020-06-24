#ifndef SOUNDFILE_8SVX_H
#define SOUNDFILE_8SVX_H

/**
 * Class for loading an iff 8svx sound file by using the
 * iffparse.library. Code of newiff from Commodore is heavily used.
 *
 * @author Uwe Rosner
 * @date 24/06/2020
 */
class Soundfile8SVX
{
public:
  /**
   * Opens and reads an iff 8svx sound file with given name.
   */
  Soundfile8SVX(const char* pFileName);

  virtual ~Soundfile8SVX();
};

#endif
