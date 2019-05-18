// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef IMAGELINE_H
#define IMAGELINE_H

#include "parameters.h"
#include <QRgb>

struct ImageLine {
	ImageLine(QRgb *scanLine, int lineIndex, int lineSize, const Parameters &params);
	ImageLine(const ImageLine &other);
	ImageLine &operator=(const ImageLine &other);
	QRgb *scanLine;
	int lineIndex;
	int lineSize;
	double zx;
	double zy;
	const Parameters &params;
};

#endif // IMAGELINE_H
