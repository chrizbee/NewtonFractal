// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Föehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QVector>
#include <QSize>
#include <QRect>
#include <complex>

typedef std::complex<double> complex;
typedef QVector<std::complex<double>> RootVector;

static constexpr double  PI  = 3.141592653589793238463;	// Pi as a constexpr

static constexpr double  HST = 1e-6;					// Step size for numerical derivative
static constexpr double  EPS = 1e-3;					// Max error allowed
static constexpr quint8  NRT = 6;						// Number of roots
static constexpr quint8  RAD = 5;						// Root indicator radius
static constexpr double  MOD = 0.2;						// Root drag speed modifier

static constexpr quint8  DRC = 5;						// Default root count
static constexpr double  DZM = 0.05;					// Default zoom factor
static constexpr double  DSC = 0.5;						// Default zoom factor
static constexpr quint16 DTI = 400;						// Default timer interval
static constexpr quint16 DMI = 160;						// Default max. iterations
static constexpr quint16 DSI = 600;						// Default size
static constexpr quint16 DZS = 2;						// Default complex size [-DZS -> +DZS]

static constexpr double DSF = 0.5 * DZS / DSI;			// Resulting size factor
static constexpr complex STEP(HST, HST);				// Step size for numerical derivative
static constexpr complex INV_STEP(0.5/HST, -0.5/HST);	// Inv step size to avoid division in loop

struct Limits {
	Limits();
	bool operator==(const Limits &other) const;
	void move(QPoint distance, const QSize &ref);
	void zoom(bool in, double xw, double yw);
	void reset(QSize size);
	double left;
	double right;
	double top;
	double bottom;
	double zoomFactor;
};

struct Parameters {
	Parameters(quint8 rootCount = DRC);
	bool operator==(const Parameters &other) const;
	void resize(QSize newSize, bool resizeLimits = false);
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
