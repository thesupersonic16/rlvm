// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2008 Elliot Glaysher
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
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
// -----------------------------------------------------------------------

#ifndef __ScriptMachine_hpp__
#define __ScriptMachine_hpp__

#include "MachineBase/RLMachine.hpp"

#include <vector>
#include <luabind/luabind.hpp>

// -----------------------------------------------------------------------

/**
 * A special RLMachine used in testing, which automatically selects specific
 */
class ScriptMachine : public RLMachine {
 public:
  ScriptMachine(System& in_system, libReallive::Archive& in_archive);
  virtual ~ScriptMachine();

  /// Sets the decisions to take
  void setDecisionList(const std::vector<std::string>& decisions);

  void setHandlers(
    const std::map<std::pair<int, int>, luabind::object>& handlers);

  void saveOnDecisions(int slot) { save_on_decision_slot_ = slot; }

  // Overloaded from RLMachine:

  /**
   * ScriptMachine will run pieces of lua code at certain scene/line
   * combinations.
   */
  virtual void setLineNumber(const int i);

  /**
   * So we can effectively intercept requests to pause and x
   */
  virtual void pushLongOperation(LongOperation* long_operation);

  void incrementOnSave() { increment_on_save_ = true; }

  // Memory accessor. (Maybe just translate this in luabind_Machine?)
  int getInt(const std::string& bank, int position);

 private:
  typedef std::vector<std::string> Selections;
  Selections decisions_;

  typedef std::map<std::pair<int, int>, luabind::object> Handlers;
  Handlers handlers_;

  int current_decision_;

  /// Which game save slot to automatically save to when we automatically make
  /// a decision, or -1 if disabled.
  int save_on_decision_slot_;

  /// Whether we increment |save_on_decision_slot_| every time we save.
  bool increment_on_save_;
};  // end of class ScriptMachine

#endif
