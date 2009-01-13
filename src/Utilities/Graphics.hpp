// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2009 Elliot Glaysher
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

#ifndef SRC_UTILITIES_GRAPHICS_HPP_
#define SRC_UTILITIES_GRAPHICS_HPP_

#include <vector>

class Gameexe;
class Rect;
class RLMachine;
class Point;
class Size;

/**
 * Changes the coordinate types. All operations internally are done in
 * rec coordinates, (x, y, width, height). The GRP functions pass
 * parameters of the format (x1, y1, x2, y2).
 *
 * @param x1 X coordinate. Not changed by this function
 * @param y1 Y coordinate. Not changed by this function
 * @param x2 X2. In place changed to width.
 * @param y2 Y2. In place changed to height.
 */
inline void grpToRecCoordinates(int x1, int y1, int& x2, int& y2) {
  x2 = x2 - x1;
  y2 = y2 - y1;
}

// -----------------------------------------------------------------------

/**
 * Will search for a \#SEL.selNum (and translate from grp to rec
 * coordinates), or \#SELR.selNum if a #SEL version isn't found in the
 * gameexe.ini file.
 *
 * @return \#SEL in rec coordinates
 */
std::vector<int> getSELEffect(RLMachine& machine, int selNum);

// -----------------------------------------------------------------------

/**
 * Returns the source rect and destination point from a \#SEL or \#SELR id.
 */
void getSELPointAndRect(RLMachine& machine, int selNum, Rect& rect,
                        Point& point);

// -----------------------------------------------------------------------

/**
 * Gets the size of the screen and sets it in width/height.
 */
Size getScreenSize(Gameexe& gameexe);

// -----------------------------------------------------------------------

/**
 * Clamp var between [min, max].
 */
void clamp(float& var, float min, float max);

#endif  // SRC_UTILITIES_GRAPHICS_HPP_
