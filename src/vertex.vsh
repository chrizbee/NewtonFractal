// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

precision mediump float;
attribute vec2 vPosition;

void main()
{
    gl_Position = vec4(vPosition, 0.0, 1.0);
}
