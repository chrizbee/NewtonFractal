#include "parameters.h"

static const double PI = 3.141592653589793238463;

Parameters::Parameters(quint8 rootCount)
{
	// Initialize with rootCount
	maxIterations = 30;
	resultSize = QSize(128, 128);
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

QVector<complex> equidistantRoots(quint8 rootCount)
{
	// Return equidistant points on a circle
	QVector<complex> roots;
	for (quint8 i = 0; i < rootCount; ++i) {
		double angle = 2 * PI * i / rootCount;
		roots.append(complex(cos(angle), sin(angle)));
	}
	return roots;
}
