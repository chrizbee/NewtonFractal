// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "parameters.h"

Parameters::Parameters(quint8 rootCount) :
	roots(equidistantRoots(rootCount)),
	limits(Limits()),
	size(DSI, DSI),
	maxIterations(DMI),
	damping(DDP),
	scaleDownFactor(DSC),
	scaleDown(false),
	multiThreaded(true)
{
}

bool Parameters::operator==(const Parameters &other) const
{
	// Check for same root count
	if (roots.size() != other.roots.size())
		return false;

	// Check for same roots
	for (quint8 i = 0; i < roots.size(); ++i) {
		if (roots[i] != other.roots[i]) {
			return false;
		}
	}

	// Check for remaining parameters
	return (
		limits == other.limits &&
		size == other.size &&
		maxIterations == other.maxIterations &&
		damping == other.damping &&
		scaleDownFactor == other.scaleDownFactor &&
		scaleDown == other.scaleDown &&
		multiThreaded == other.multiThreaded
	);
}

void Parameters::resize(QSize newSize)
{
	// Update limits and size
	limits.resize(newSize - size);
	size = newSize;
}

void Parameters::reset()
{
	// Reset roots and limits
	quint8 rootCount = roots.size();
	roots = equidistantRoots(rootCount);
	limits.reset(size);
	scaleDown = false;
}

QPoint complex2point(complex z, const Parameters &params)
{
	// Convert complex to point
	int x = (z.real() - params.limits.left()) * (params.size.width() - 1) / params.limits.width();
	int y = (z.imag() - params.limits.top()) * (params.size.height() - 1) / -params.limits.height();
	return QPoint(x, y);
}

complex point2complex(QPoint p, const Parameters &params)
{
	// Convert point to complex
	double real = p.x() * params.limits.width() / (params.size.width() - 1) + params.limits.left();
	double imag = p.y() * -params.limits.height() / (params.size.height() - 1) + params.limits.top();
	return complex(real, imag);
}

complex distance2complex(QPoint d, const Parameters &params)
{
	// Convert distance to complex
	double real = d.x() * params.limits.width()  / (params.size.width() - 1);
	double imag = d.y() * -params.limits.height() / (params.size.height() - 1);
	return complex(real, imag);
}

RootVector equidistantRoots(quint8 rootCount)
{
	// Return equidistant points on a circle
	RootVector roots;
	for (quint8 i = 0; i < rootCount; ++i) {
		double angle = 2 * PI * i / rootCount;
		roots.append(complex(cos(angle), sin(angle)));
	}
	return roots;
}

bool rootContainsPoint(QPoint root, QPoint point)
{
	// Check if root contains point
	QPoint dist = root - point;
	return (abs(dist.x()) < RAD && abs(dist.y()) < RAD);
}
