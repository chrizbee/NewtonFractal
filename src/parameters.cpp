// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "parameters.h"

Parameters::Parameters() :
	limits(Limits()),
	size(nf::DSI, nf::DSI),
	maxIterations(nf::DMI),
	damping(nf::DDP),
	scaleDownFactor(nf::DSC),
	scaleDown(false),
	processor(CPU_MULTI),
	orbitMode(false),
	orbitStart(0, 0),
	benchmark(false)
{
}

bool Parameters::paramsChanged(const Parameters &other) const
{
	// Check for same root count
	if (roots.count() != other.roots.count())
		return true;

	// Check for same roots
	for (quint8 i = 0; i < roots.count(); ++i) {
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
		processor != other.processor ||
		benchmark != other.benchmark ||
		scaleUpFactor != other.scaleUpFactor
	);
}

bool Parameters::orbitChanged(const Parameters &other) const
{
	// Check for orbit
	return (
		orbitStart != other.orbitStart ||
		orbitMode != other.orbitMode ||
		(orbitMode && paramsChanged(other))
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
	quint8 rootCount = roots.count();
	for (quint8 i = 0; i < rootCount; ++i) {
		double angle = 2 * nf::PI * i / rootCount;
		roots[i].setValue(complex(cos(angle), sin(angle)));
	}

	// Reset limits and scaledown
	limits.reset(size);
	scaleDown = false;
}

QPoint Parameters::complex2point(complex z)
{
	// Convert complex to point
	int x = (z.real() - limits.left()) * (size.width() - 1) / limits.width();
	int y = (z.imag() - limits.top()) * (size.height() - 1) / -limits.height();
	return QPoint(x, y);
}

complex Parameters::point2complex(QPoint p)
{
	// Convert point to complex
	double real = p.x() * limits.width() / (size.width() - 1) + limits.left();
	double imag = p.y() * -limits.height() / (size.height() - 1) + limits.top();
	return complex(real, imag);
}

complex Parameters::distance2complex(QPointF d)
{
	// Convert distance to complex
	double real = d.x() * limits.width() / (size.width() - 1);
	double imag = d.y() * -limits.height() / (size.height() - 1);
	return complex(real, imag);
}

int Parameters::rootContainsPoint(QPoint point)
{
	// Check if root contains point
	quint8 rootCount = roots.count();
	for (quint8 i = 0; i < rootCount; ++i) {
		QPoint dist = complex2point(roots[i].value()) - point;
		if (abs(dist.x()) < nf::RIR && abs(dist.y()) < nf::RIR) {
			return i;
		}
	}
	return -1;
}

QVector<QVector2D> Parameters::rootsVec2()
{
	// Return vector of root values only
	QVector<QVector2D> vec2;
	quint8 rootCount = roots.count();
	for (quint8 i = 0; i < rootCount; ++i) {
		vec2.append(roots[i].valueVec2());
	}
	return vec2;
}

QVector<QVector3D> Parameters::colorsVec3()
{
	// Return vector of root colors only
	QVector<QVector3D> vec3;
	quint8 rootCount = roots.count();
	for (quint8 i = 0; i < rootCount; ++i) {
		vec3.append(roots[i].colorVec3());
	}
	return vec3;
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

QVector2D complex2vec2(complex z)
{
	// Convert complex to vec2
	return QVector2D(z.real(), z.imag());
}
