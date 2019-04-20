#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QVector>
#include <QSize>
#include <complex>

#define XA	-1.0
#define XB   1.0
#define YA	 1.0
#define YB  -1.0
#define H	 1e-6	// Step size for numerical derivative
#define EPS	 1e-3	// Max error allowed
#define N	 6		// Number of roots

typedef std::complex<double> complex;

struct Parameters {
	Parameters(quint8 rootCount = N);
	QVector<complex> roots;
	QSize resultSize;
	quint8 maxIterations;
};

QString complex2string(complex z);
complex string2complex(QString s);

#endif // PARAMETERS_H
