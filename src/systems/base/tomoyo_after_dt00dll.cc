// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2013 Elliot Glaysher
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

#include "systems/base/tomoyo_after_dt00dll.h"

#include <dlfcn.h>
#include <iostream>

#include "systems/base/system.h"
#include "libreallive/gameexe.h"
#include "machine/rlmachine.h"
#include "machine/memory.h"

static void* dt00;
TomoyoAfterDT00DLL::TomoyoAfterDT00DLL(RLMachine& machine) {
  auto gameexe = machine.system().gameexe();
  boost::filesystem::path path(gameexe("__GAMEPATH"));

  // Load Dungeons & Takafumis library
  dt00 = dlopen((path / "libdt00.so").string().c_str(), RTLD_LAZY);
  if (dt00) {
    *reinterpret_cast<int**>(dlsym(dt00, "intA")) =
        machine.memory().local().intA;
    *reinterpret_cast<int**>(dlsym(dt00, "intB")) =
        machine.memory().local().intB;
    *reinterpret_cast<int**>(dlsym(dt00, "intC")) =
        machine.memory().local().intC;
    *reinterpret_cast<int**>(dlsym(dt00, "intD")) =
        machine.memory().local().intD;
    *reinterpret_cast<int**>(dlsym(dt00, "intE")) =
        machine.memory().local().intE;
    *reinterpret_cast<int**>(dlsym(dt00, "intF")) =
        machine.memory().local().intF;
    ((int (*)(int, int))dlsym(dt00, "reallive_dll_func_load"))(0, 0);
  } else {
    std::cerr << "WARNING: Tomoyo After: Dungeons & Takafumis is implemented in"
              << " a DLL and hasn't been reverse engineered yet." << std::endl;
  }
}

TomoyoAfterDT00DLL::~TomoyoAfterDT00DLL() {
  if (dt00)
    dlclose(dt00);
}

int TomoyoAfterDT00DLL::CallDLL(RLMachine& machine,
                                int func,
                                int arg1,
                                int arg2,
                                int arg3,
                                int arg4) {
    if (dt00) {
      ((int (*)(int, int, int, int, int))dlsym(dt00, "reallive_dll_func_call"))(
        func, arg1, arg2, arg3, arg4);
    }
    return 1;
  }

const std::string& TomoyoAfterDT00DLL::GetDLLName() const {
  static std::string n("DT00");
  return n;
}
