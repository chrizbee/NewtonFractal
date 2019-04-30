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

static constexpr double  DSF = 0.5 * DZS / DSI;			// Resulting size factor
static constexpr complex STEP(HST, HST);				// Step size for numerical derivative
static constexpr complex INV_STEP(0.5/HST, -0.5/HST);	// Inv step size to avoid division in loop

#endif // DEFAULTS_H
