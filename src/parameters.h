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

enum Processor { CPU_SINGLE, CPU_MULTI, GPU_OPENGL };

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
	complex damping;
	double scaleDownFactor;
	bool scaleDown;
	Processor processor;
	bool orbitMode;
	QPoint orbitStart;
};

complex string2complex(const QString &text);
QString complex2string(complex z, quint8 precision = 2);
QVector2D complex2vec2(complex z);
complex point2complex(QPoint p, const Parameters &params);
QPoint  complex2point(complex z, const Parameters &params);
complex distance2complex(QPointF d, const Parameters &params);
bool rootContainsPoint(QPoint root, QPoint point);
QVector<QVector2D> rootsVec2(const Parameters &params);
QVector<QVector3D> colorsVec3(const Parameters &params);

#endif // PARAMETERS_H
