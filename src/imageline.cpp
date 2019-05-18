// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "imageline.h"

ImageLine::ImageLine(QRgb *scanLine, int lineIndex, int lineSize, const Parameters &params) :
	scanLine(scanLine),
	lineIndex(lineIndex),
	lineSize(lineSize),
	zx(0),
	zy(0),
	params(params)
{
}

ImageLine::ImageLine(const ImageLine &other) :
	scanLine(other.scanLine),
	lineIndex(other.lineIndex),
	lineSize(other.lineSize),
	zx(other.zx),
	zy(other.zy),
	params(other.params)
{
}

ImageLine &ImageLine::operator=(const ImageLine &other)
{
	// Assign other values to this
	// Except params. They couldn't have changed since they're const
	scanLine = other.scanLine;
	lineIndex = other.lineIndex;
	lineSize = other.lineSize;
	zx = other.zx;
	zy = other.zy;
	return *this;
}
