// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "defaults.h"
#include "limits.h"
#include <QSize>

struct Parameters {
	Parameters(quint8 rootCount = DRC);
	bool operator==(const Parameters &other) const;
	void resize(QSize newSize);
	void reset();
	RootVector roots;
	Limits limits;
	QSize size;
	quint16 maxIterations;
	double scaleDownFactor;
	bool scaleDown;
	bool multiThreaded;
	bool zoomToCursor;
};

QPoint  complex2point(complex z, const Parameters &params);
complex point2complex(QPoint p, const Parameters &params);
complex distance2complex(QPoint d, const Parameters &params);

RootVector equidistantRoots(quint8 rootCount);
bool rootContainsPoint(QPoint root, QPoint point);

#endif // PARAMETERS_H
