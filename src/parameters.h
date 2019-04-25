#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QVector>
#include <QSize>
#include <QRect>
#include <complex>

#define HS		1e-6	// Step size for numerical derivative
#define EPS		1e-3	// Max error allowed
#define NR		6		// Number of roots
#define RR		5		// Root indicator radius
#define DRC		5		// Default root count
#define DMI		80		// Default max. iterations
#define DSI		300		// Default size
#define DZM		0.05	// Default zoom factor

typedef std::complex<double> complex;
typedef QVector<std::complex<double>> RootVector;

struct Limits {
	Limits();
	bool operator==(const Limits &other) const;
	void move(double dx, double dy);
	void zoom(bool in);
	double left;
	double right;
	double top;
	double bottom;
	double zoomFactor;
};

struct Parameters {
	Parameters(quint8 rootCount = DRC);
	bool operator==(const Parameters &other) const;
	RootVector roots;
	Limits limits;
	QSize resultSize;
	quint8 maxIterations;
	bool multiThreaded;
};

QString complex2string(complex z);
complex string2complex(QString s);
QPoint complex2point(complex z, const QRect &res, const QRect &stretched, const Limits &limits);
complex point2complex(QPoint p, const QRect &res, const QRect &stretched, const Limits &limits);
complex distance2complex(QPoint d, const QRect &res, const QRect &stretched, const Limits &limits);
RootVector equidistantRoots(quint8 rootCount);
bool rootContainsPoint(QPoint root, QPoint point);

#endif // PARAMETERS_H
