// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "parameters.h"

Parameters::Parameters() :
	limits(Limits()),
	size(DSI, DSI),
	maxIterations(DMI),
	damping(DDP),
	scaleDownFactor(DSC),
	scaleDown(false),
	multiThreaded(true),
	orbitMode(false),
	orbitStart(0, 0)
{
}

bool Parameters::paramsChanged(const Parameters &other) const
{
	// Check for same root count
	if (roots.size() != other.roots.size())
		return true;

	// Check for same roots
	for (quint8 i = 0; i < roots.size(); ++i) {
		if (roots[i] != other.roots[i]) {
			return true;
		}
	}

	// Check for remaining parameters
	return (
		limits != other.limits ||
		size != other.size ||
		maxIterations != other.maxIterations ||
		damping != other.damping ||
		scaleDownFactor != other.scaleDownFactor ||
		scaleDown != other.scaleDown ||
		multiThreaded != other.multiThreaded
	);
}

bool Parameters::orbitChanged(const Parameters &other) const
{
	// Check for orbit
	return (
		orbitStart != other.orbitStart ||
		orbitMode != other.orbitMode ||
		orbitMode && paramsChanged(other)
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
	// Equidistant points on a circle
	quint8 rootCount = roots.size();
	for (quint8 i = 0; i < rootCount; ++i) {
		double angle = 2 * PI * i / rootCount;
		roots[i].setValue(complex(cos(angle), sin(angle)));
	}

	// Reset limits and scaledown
	limits.reset(size);
	scaleDown = false;
}

complex string2complex(const QString &text)
{
	// Convert string to complex
	bool sign = false;
	double real = 0, imag = 0;
	QStringList parts = text.split('i');
	QString rstr = parts.first().simplified();

	// Get imag sign from first part
	int si = rstr.indexOf('-', 1);
	if (si > 0) {
		sign = true;
		rstr.replace(si, 1, ' ');
	} else rstr.replace('+', ' ');

	// Get real
	real = rstr.simplified().toDouble();

	// Get imag
	if (parts.length() >= 2) {
		imag = parts[1].simplified().toDouble();
		if (sign) imag = -imag;
	}

	// Return complex number
	return (complex(real, imag));
}

QString complex2string(complex z, quint8 precision)
{
	// Convert complex to string
	static QString complexFormat("%1 %2 i%3");
	QString real = QString::number(z.real(), 'f', precision);
	QString imag = QString::number(abs(z.imag()), 'f', precision);
	QString sign = z.imag() >= 0 ? "+" : "-";
	return complexFormat.arg(real, sign, imag);
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

complex distance2complex(QPointF d, const Parameters &params)
{
	// Convert distance to complex
	double real = d.x() * params.limits.width()  / (params.size.width() - 1);
	double imag = d.y() * -params.limits.height() / (params.size.height() - 1);
	return complex(real, imag);
}

bool rootContainsPoint(QPoint root, QPoint point)
{
	// Check if root contains point
	QPoint dist = root - point;
	return (abs(dist.x()) < RIR && abs(dist.y()) < RIR);
}
