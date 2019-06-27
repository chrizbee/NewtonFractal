// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include "defaults.h"
#include "limits.h"
#include "root.h"
#include <QSize>
#include <QVector>

struct Parameters {
	Parameters();
	bool paramsChanged(const Parameters &other) const;
	bool orbitChanged(const Parameters &other) const;
	void resize(QSize newSize);
	void reset();
	QVector<Root> roots;
	Limits limits;
	QSize size;
	quint16 maxIterations;
	double damping;
	double scaleDownFactor;
	bool scaleDown;
	bool multiThreaded;
	bool orbitMode;
	QPoint orbitStart;
};

QString complex2string(complex z);
QPoint  complex2point(complex z, const Parameters &params);
complex point2complex(QPoint p, const Parameters &params);
complex distance2complex(QPointF d, const Parameters &params);
bool rootContainsPoint(QPoint root, QPoint point);

#endif // PARAMETERS_H
