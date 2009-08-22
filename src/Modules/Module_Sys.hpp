// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2006 Elliot Glaysher
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
//
// -----------------------------------------------------------------------

#ifndef SRC_MODULES_MODULE_SYS_HPP_
#define SRC_MODULES_MODULE_SYS_HPP_

/**
 * @file   Module_Sys.hpp
 * @author Elliot Glaysher
 * @date   Tue Nov 14 21:04:24 2006
 *
 * @brief  A module that contains completely random and non-related functions.
 */

#include "MachineBase/RLModule.hpp"

#include "MachineBase/RLOperation.hpp"

class RLMachine;

/**
 * Contains functions for mod<1:4>, Sys.
 *
 * @ingroup ModuleSys
 */
class SysModule : public RLModule {
 public:
  SysModule();
};

// -----------------------------------------------------------------------

struct Sys_MenuReturn : public RLOp_Void_Void {
  /// Don't advance the instruction pointer when this returns
  virtual bool advanceInstructionPointer() { return false; }
  virtual void operator()(RLMachine& machine);
};
#endif  // SRC_MODULES_MODULE_SYS_HPP_
