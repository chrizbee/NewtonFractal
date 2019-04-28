// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Föehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "parameters.h"

Limits::Limits() :
	left(-1.0),
	right(1.0),
	top(1.0),
	bottom(-1.0),
	zoomFactor(DZM)
{
}

bool Limits::operator==(const Limits &other) const
{
	// Check if limits are the same
	return (
		left == other.left &&
		right == other.right &&
		top == other.top &&
		bottom == other.bottom
	);
}

void Limits::move(QPoint distance, const QSize &ref)
{
	// Move limits by distance
	double dx = distance.x() * (right - left) / (ref.width() - 1);
	double dy = distance.y() * (bottom - top) / (ref.height() - 1);
	left += dx;
	right += dx;
	top += dy;
	bottom += dy;
}

void Limits::zoom(bool in, double xw, double yw)
{
	// Zoom limits in / out
	double zoom = in ? -zoomFactor : zoomFactor;
	double wZoom = (right - left) * zoom;
	double hZoom = (top - bottom) * zoom;
	left -= xw * wZoom;
	right += (1.0 - xw) * wZoom;
	top += yw * hZoom;
	bottom -= (1.0 - yw) * hZoom;
}

void Limits::reset(QSize size)
{
	// Reset limits
	right = DSF * size.width();
	left = -DSF * size.width();
	top = DSF * size.height();
	bottom = -DSF * size.height();
}

Parameters::Parameters(quint8 rootCount) :
	roots(equidistantRoots(rootCount)),
	limits(Limits()),
	size(DSI, DSI),
	maxIterations(DMI),
	scaleDownFactor(DSC),
	scaleDown(false),
	multiThreaded(true),
	zoomToCursor(true)
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
		scaleDownFactor == other.scaleDownFactor &&
		scaleDown == other.scaleDown &&
		multiThreaded == other.multiThreaded &&
		zoomToCursor == other.zoomToCursor
	);
}

void Parameters::resize(QSize newSize, bool resizeLimits)
{
	// Update limits if necessary
	if (resizeLimits) {
		int dx = newSize.width() - size.width();
		int dy = newSize.height() - size.height();
		limits.right += DSF * dx;
		limits.left -= DSF * dx;
		limits.top += DSF * dy;
		limits.bottom -= DSF * dy;
	}

	// Update size
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
	int x = (z.real() - params.limits.left) * (params.size.width() - 1) / (params.limits.right - params.limits.left);
	int y = (z.imag() - params.limits.top) * (params.size.height() - 1) / (params.limits.bottom - params.limits.top);
	return QPoint(x, y);
}

complex point2complex(QPoint p, const Parameters &params)
{
	// Convert point to complex
	double real = p.x() * (params.limits.right - params.limits.left) / (params.size.width() - 1) + params.limits.left;
	double imag = p.y() * (params.limits.bottom - params.limits.top) / (params.size.height() - 1) + params.limits.top;
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
