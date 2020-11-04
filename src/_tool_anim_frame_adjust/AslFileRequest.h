#ifndef ASL_FILE_REQUEST_H
#define ASL_FILE_REQUEST_H

#include <string>
#include <exec/types.h>
#include <intuition/intuition.h>

/**
 * Provides an easy to use ASL file requester.
 *
 * @author Uwe Rosner
 * @date 18/10/2018
 */
class AslFileRequest
{
public:
  AslFileRequest(struct Window*& pIntuiWindow);

  AslFileRequest(struct Window*& pIntuiWindow, 
                 ULONG left, 
                 ULONG top,
                 ULONG width,
                 ULONG height);

  virtual ~AslFileRequest();

  /**
   * Open a file requester and let the user select a file
   *
   * @param title Text to be displayed in the file requesters title bar
   *
   * @param initialFileFullPath A string with a full path and file name
   * to pre-select this file at request opening time. If empty or
   * invalid, no file is pre- selected.
   *
   * @param isOnlyPathPreSelected When set to true, only the path of
   * initialFileFullPath will be pre-selected but not the file itself.
   *
  * @param isSaveRequester When set to true the requester will be opened
   * as save file selector.
   *
   * @returns
   * Full path to the selected file or an empty string if dialog was
   * canceled by the user.
   */
  std::string SelectFile(const std::string& title,
                         const std::string& initialFileFullPath,
                         bool isOnlyPathPreSelected = false,
                         bool isSaveRequester = false);

private:
  struct Window*& m_pIntuiWindow;

  ULONG m_LeftTag;
  ULONG m_LeftVal;

  ULONG m_TopTag;
  ULONG m_TopVal;

  ULONG m_WidthTag;
  ULONG m_WidthVal;

  ULONG m_HeightTag;
  ULONG m_HeightVal;
};

#endif

