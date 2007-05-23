// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006 Elliot Glaysher
//  
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//  
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//  
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
//  
// -----------------------------------------------------------------------

#ifndef __TextSystem_hpp__
#define __TextSystem_hpp__

#include <string>

#include <boost/ptr_container/ptr_vector.hpp>
#include "SDL_ttf.h"

class Gameexe;
class RLMachine;
class TextWindow;
class TextPage;
class TextKeyCursor;

class TextSystem
{
protected:
  /// TextPage will call our internals since it actually does most of
  /// the work while we hold state.
  friend class TextPage;

  /**
   * @name Auto mode (variables)
   * 
   * @{
   */
  /// Whether Auto mode is enabled
  bool m_autoMode;

  int m_autoModeBaseTime;
  int m_autoModeCharTime;

  /// @}

  /// Whether holding down the control key will skip text.
  bool m_ctrlKeySkip;

  /// Fast text mode
  bool m_fastTextMode;

  /// Internal 'no wait' flag
  bool m_messageNoWait;

  /// Message speed; range from 0 to 255
  char m_messageSpeed;

  /// Sets which window is the current active window.
  int m_activeWindow;

  /**
   * @name Backlog Management
   * 
   * @{
   */

  /// Whether we are reading the backlog
  bool m_isReadingBacklog;

  /// Internal structure used to keep track of the state of 
  typedef boost::ptr_map<int, TextPage> PageSet;

  /// The current page set. Represents what is on the screen right now.
  std::auto_ptr<PageSet> m_currentPageset;

  /// Previous Text Pages. The TextSystem owns the list of previous
  /// pages because multiple windows can be displayed in one text page.
  boost::ptr_vector<PageSet> m_previousPageSets;

  /// When m_previousPageIt == m_previousPages.end(), m_activePage is
  /// currently being rendered to the screen. When it is any valid
  /// iterator pointing into m_previousPages, that is the current page
  /// being rendered.
  boost::ptr_vector<PageSet>::iterator m_previousPageIt;

  /// Whether we are in a state where the interpreter is pause()d.
  bool m_inPauseState;

  /// @}

  boost::shared_ptr<TextKeyCursor> m_textKeyCursor;

  /// The default \#WINDOW_ATTR. This is what is changed by the 
  std::vector<int> m_windowAttr;

  /** 
   * @name Font storage
   * 
   * @{
   */

  typedef std::map< int , boost::shared_ptr<TTF_Font> > FontSizeMap;
  FontSizeMap m_map;

  /// @}

  /**
   * @name Global Window Button Toggles
   * 
   * @{
   */
  bool m_moveUse, m_clearUse, m_readJumpUse, m_automodeUse, m_msgbkUse,
    m_msgbkleftUse, m_msgbkrightUse, m_exbtnUse;

  void checkAndSetBool(Gameexe& gexe, const std::string& key, bool& out);
  /// @}

public:
  TextSystem(Gameexe& gexe);
  virtual ~TextSystem();

  /**
   * @name Implementation detail interface
   * 
   * @{
   */

  virtual void executeTextSystem(RLMachine& machine) = 0;

  virtual void render(RLMachine& machine) = 0;
  virtual void hideTextWindow(int winNumber) = 0;
  virtual void hideAllTextWindows() = 0;
  virtual void clearAllTextWindows() = 0;
  virtual TextWindow& textWindow(RLMachine&, int textWindowNumber) = 0;
  TextWindow& currentWindow(RLMachine& machine);

  /// @}

  void setInPauseState(bool in) { m_inPauseState = in; }

  int activeWindow() const { return m_activeWindow; }
  void setActiveWindow(int window) { m_activeWindow = window; }

  std::vector<int> activeWindows();

  /** 
   * Take a snapshot of the current window state, with their
   * respective TextPages, and add it to the backlog.
   */
  void snapshot(RLMachine& machine);

  /** 
   * Resets the text page in the currentSet
   */
  void newPageOnWindow(RLMachine& machine, int window);

  /** 
   * Get the active page. This function will return
   * m_windows[m_activeWindow].page().
   */
  TextPage& currentPage(RLMachine& machine);

  /**
   * @name Backlog management
   * 
   * @{
   */

  /**
   * Cleares the screen, moves back one page and renders it.
   */
  void backPage(RLMachine& machine);
  void forwardPage(RLMachine& machine);

  void replayPageSet(PageSet& set, bool isCurrentPage);

  bool isReadingBacklog() const;
  void stopReadingBacklog();

  /// @}

  /**
   * @name Auto mode
   * 
   * It is possible to set the interpreter up to advance text
   * automatically instead of waiting for player input after each
   * screen is displayed; the `auto mode' controls permit this
   * behaviour to be customised.
   * 
   * @{
   */
  void setAutoMode(int i) { m_autoMode = i; }
  int autoMode() const { return m_autoMode; }

  void setAutoBaseTime(int i) { m_autoModeBaseTime = i; }
  int autoBaseTime() const { return m_autoModeBaseTime; }

  void setAutoCharTime(int i) { m_autoModeCharTime = i; }
  int autoCharTime() const { return m_autoModeCharTime; }

  int getAutoTime(int numChars);
  /// @}

  void setKeyCursor(RLMachine& machine, int newCursor);

  void setCtrlKeySkip(int i) { m_ctrlKeySkip = i; }
  int ctrlKeySkip() const { return m_ctrlKeySkip; }

  void setFastTextMode(int i) { m_fastTextMode = i; }
  int fastTextMode() const { return m_fastTextMode; }

  void setMessageNoWait(int i) { m_messageNoWait = i; }
  int messageNoWait() const { return m_messageNoWait; }

  void setMessageSpeed(int i) { m_messageSpeed = i; }
  int messageSpeed() const { return m_messageSpeed; }

  /**
   * @name Window Attr Related functions
   *
   * @note Any class deriving from TextSystem is responsible for
   *       overriding all the virtual functions in this section, so as
   *       to alert any TextWindow derived objects that it owns that
   *       the default window attr has changed.
   * @{
   */
  virtual void setDefaultWindowAttr(const std::vector<int>& attr);
  std::vector<int> windowAttr() const { return m_windowAttr; }

  int windowAttrR() const { return m_windowAttr.at(0); }
  int windowAttrG() const { return m_windowAttr.at(1); }
  int windowAttrB() const { return m_windowAttr.at(2); }
  int windowAttrA() const { return m_windowAttr.at(3); }
  int windowAttrF() const { return m_windowAttr.at(4); }

  virtual void setWindowAttrR(int i) { m_windowAttr.at(0) = i; }
  virtual void setWindowAttrG(int i) { m_windowAttr.at(1) = i; }
  virtual void setWindowAttrB(int i) { m_windowAttr.at(2) = i; }
  virtual void setWindowAttrA(int i) { m_windowAttr.at(3) = i; }
  virtual void setWindowAttrF(int i) { m_windowAttr.at(4) = i; }
  /// @}

  /**
   * @name Window button state
   * 
   * @{
   */
  bool windowMoveUse() const { return m_moveUse; }
  bool windowClearUse() const { return m_clearUse; }
  bool windowReadJumpUse() const { return m_readJumpUse; }
  bool windowAutomodeUse() const { return m_automodeUse; }
  bool windowMsgbkUse() const { return m_msgbkUse; }
  bool windowMsgbkleftUse() const { return m_msgbkleftUse; }
  bool windowMsgbkrightUse() const { return m_msgbkrightUse; }
  bool windowExbtnUse() const { return m_exbtnUse; }

  /// Update the mouse cursor.
  virtual void setMousePosition(RLMachine& machine, int x, int y) = 0;
  virtual bool handleMouseClick(RLMachine& machine, int x, int y,
                                bool pressed) = 0;
  /// @}

  boost::shared_ptr<TTF_Font> getFontOfSize(int size);
};

#endif



