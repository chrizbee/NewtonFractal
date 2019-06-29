// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <complex>
#include <QVector>
#include <QColor>

typedef std::complex<double> complex;

static const QColor colors[6] = { Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow };

static constexpr double  PI  = 3.141592653589793238463;	// Pi as a constexpr

static constexpr double  EPS = 1e-3;					// Max error allowed
static constexpr quint8  RIR = 5;						// Root indicator radius
static constexpr quint8  OIR = 3;						// Orbit point indicator radius
static constexpr double  MOD = 0.2;						// Root drag speed modifier
static constexpr double  ZMF = 0.05;					// Zoom factor

static constexpr quint8  DRC = 5;						// Default root count
static constexpr double  DSC = 0.5;						// Default zoom factor
static constexpr double  DDP = 1.0;						// Default damping factor
static constexpr quint16 DTI = 400;						// Default timer interval
static constexpr quint16 DMI = 160;						// Default max. iterations
static constexpr quint16 DSI = 700;						// Default size
static constexpr quint16 MSI = 128;						// Minimum size
static constexpr quint16 DZS = 2;						// Default complex size [-DZS -> +DZS]
static constexpr double  DSF = 0.5 * DZS / DSI;			// Resulting size factor


#endif // DEFAULTS_H
