// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <complex>
#include <QVector>

typedef std::complex<double> complex;
typedef QVector<std::complex<double>> RootVector;

static constexpr double  PI  = 3.141592653589793238463;	// Pi as a constexpr

static constexpr double  EPS = 1e-3;					// Max error allowed
static constexpr quint8  NRT = 6;						// Number of roots
static constexpr quint8  RAD = 5;						// Root indicator radius
static constexpr double  MOD = 0.2;						// Root drag speed modifier
static constexpr double  ZMF = 0.05;					// Zoom factor

static constexpr quint8  DRC = 5;						// Default root count
static constexpr double  DSC = 0.5;						// Default zoom factor
static constexpr double  DDP = 1.0;						// Default damping factor
static constexpr quint16 DTI = 400;						// Default timer interval
static constexpr quint16 DMI = 160;						// Default max. iterations
static constexpr quint16 DSI = 660;						// Default size
static constexpr quint16 DZS = 2;						// Default complex size [-DZS -> +DZS]
static constexpr double  DSF = 0.5 * DZS / DSI;			// Resulting size factor

#endif // DEFAULTS_H
