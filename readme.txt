
                             * GEM Worm! *


Written originally as a demonstration for Retrochallenge 2009
Copyright 2009, 2020 Jeffrey Armstrong
Licensed under the GNU General Public License version 3

Introduction
============

GEM Worm is a simplistic game where you guide your worm towards food.  As
the worm eats more, he continues to grow.  You mustn't bump into yourself
or the walls, though, or your worm will die!  Simple!

The program is written as a GEM programming demonstration, and probably
contains numerous bugs, coding issues, etc.  It has been designed to 
compile on either the PC or Atari.  

Game Play
=========

To start the game, select "New Game" from the File menu.  The following
keys move your worm

          w = up
a = left  s = down  d = right

The arrangement is very close to modern first person shooters.

Every time your worm eats, you receive 100 points.  Every time you fail
to eat available food, your score drops 10 points.

Compiling
=========

If you have downloaded the source code, the game can be easily compiled
for either the PC or Atari.  

    - PC -
	
	The program has been designed to compile using Turbo C 2 from
	Borland.  To compile with GEM bindings, you'll need to install
	the GEM Programmer's Toolkit first.  A makefile is included to
	build the game.
	
    - Atari -
	
	The program is also designed to compile with Pure C from Pure
	Software or AHCC from whoever is maintaining that nowadays.  The
	appropriate project file (worm.prj) is included.
  
	Recently, a GNU makefile (makefile.gnu) was added for use with
	GCC on an actual Atari. 

Legalese
========

Although this game is shockingly simple, it is still copyrighted.  The
following applies:

GEM Worm
Copyright (C) 2009, 2020 Jeffrey Armstrong
http://jeff.rainbow-100.com

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

A full version of the license terms is available in LICENSE.txt.
