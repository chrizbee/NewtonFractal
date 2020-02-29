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
#include <QVector2D>
#include <QVector3D>

enum Processor {
	CPU_SINGLE,
	CPU_MULTI,
	GPU_OPENGL
};

struct Parameters {
	Parameters();
	bool paramsChanged(const Parameters &other) const;
	bool orbitChanged(const Parameters &other) const;
	void resize(QSize newSize);
	void reset();

	complex point2complex(QPoint p);
	QPoint  complex2point(complex z);
	complex distance2complex(QPointF d);
	int rootContainsPoint(QPoint point);
	QVector<QVector2D> rootsVec2();
	QVector<QVector3D> colorsVec3();

	QVector<Root> roots;
	Limits limits;
	QSize size;
	quint16 maxIterations;
	complex damping;
	double scaleDownFactor;
	bool scaleDown;
	Processor processor;
	bool orbitMode;
	QPoint orbitStart;
	bool benchmark;
	uint scaleUpFactor;
};

// Does not really belong here, but I don't care
complex string2complex(const QString &text);
QString complex2string(complex z, quint8 precision = 2);
QVector2D complex2vec2(complex z);
QString dynamicFileName(const Parameters &params, const QString &ext);

#endif // PARAMETERS_H
