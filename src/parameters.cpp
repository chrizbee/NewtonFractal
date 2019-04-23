#include "parameters.h"

static const double PI = 3.141592653589793238463;

Parameters::Parameters(quint8 rootCount)
{
	// Initialize with rootCount
	maxIterations = DMI;
	resultSize = QSize(DSI, DSI);
	roots = equidistantRoots(rootCount);
}

QString complex2string(complex z)
{
	// Convert complex to string
	static QString complexFormat("%1 j %2");
	return complexFormat.arg(z.real()).arg(z.imag());
}

complex string2complex(QString s)
{
	// Convert string to complex
	complex z(0, 0);
	QStringList parts = s.split('j');
	z.real(parts.first().simplified().toDouble());
	if (parts.length() >= 2) {
		z.imag(parts[1].simplified().toDouble());
	}
	return z;
}

QPoint complex2point(complex z, QRect stretched, QRect limits)
{
	// Convert complex to point
	double xStretch = (double)stretched.width() / limits.width();
	double yStretch = (double)stretched.height() / limits.height();
	int x = xStretch * (z.real() - XA) * (limits.width() - 1) / (XB - XA);
	int y = yStretch * (z.imag() - YA) * (limits.height() - 1) / (YB - YA);
	return QPoint(x, y);
}

complex point2complex(QPoint p, QRect stretched, QRect limits)
{
	// Convert point to complex
	double xStretch = (double)stretched.width() / limits.width();
	double yStretch = (double)stretched.height() / limits.height();
	double real = p.x() * (XB - XA) / (xStretch * (limits.width() - 1)) + XA;
	double imag = p.y() * (YB - YA) / (yStretch * (limits.height() - 1)) + YA;
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
	return (abs(dist.x()) < RR && abs(dist.y()) < RR);
}
