#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QVector>
#include <QSize>
#include <QRect>
#include <complex>

#define XA	-1.0
#define XB   1.0
#define YA	 1.0
#define YB  -1.0
#define HS	 1e-6	// Step size for numerical derivative
#define EPS	 1e-3	// Max error allowed
#define NR	 6		// Number of roots
#define RR	 5		// Root indicator radius
#define DMI	 80		// Default max. iterations
#define DSI	 256	// Default size

typedef std::complex<double> complex;
typedef QVector<std::complex<double>> RootVector;

struct Parameters {
	Parameters(quint8 rootCount = NR);
	RootVector roots;
	QSize resultSize;
	quint8 maxIterations;
};

QString complex2string(complex z);
complex string2complex(QString s);
QPoint complex2point(complex z, QRect stretched, QRect limits);
complex point2complex(QPoint p, QRect stretched, QRect limits);
RootVector equidistantRoots(quint8 rootCount);
bool rootContainsPoint(QPoint root, QPoint point);

#endif // PARAMETERS_H
