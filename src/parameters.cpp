#include "parameters.h"

static const double PI = 3.141592653589793238463;

Limits::Limits(double left, double right, double top, double bottom) :
	left(left),
	right(right),
	top(top),
	bottom(bottom)
{
}

bool Limits::operator==(const Limits &other) const
{
	// Check if limits are the same
	return (
		left == other.left &&
		right == other.right &&
		top == other.top &&
		bottom == other.bottom
	);
}

Parameters::Parameters(quint8 rootCount) :
	roots(equidistantRoots(rootCount)),
	limits(-1, 1, 1, -1),
	resultSize(QSize(DSI, DSI)),
	maxIterations(DMI),
	multiThreaded(true)
{
}

bool Parameters::operator==(const Parameters &other) const
{
	// Check for same root count
	if (roots.size() != other.roots.size())
		return false;

	// Check for same roots
	for (quint8 i = 0; i < roots.size(); ++i) {
		if (roots[i] != other.roots[i]) {
			return false;
		}
	}

	// Check for remaining parameters
	return (
		limits == other.limits &&
		resultSize == other.resultSize &&
		maxIterations == other.maxIterations &&
		multiThreaded == other.multiThreaded
	);
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

QPoint complex2point(complex z, const QRect &res, const QRect &stretched, const Limits &limits)
{
	// Convert complex to point
	double xStretch = (double)stretched.width() / res.width();
	double yStretch = (double)stretched.height() / res.height();
	int x = xStretch * (z.real() - limits.left) * (res.width() - 1) / (limits.right - limits.left);
	int y = yStretch * (z.imag() - limits.top) * (res.height() - 1) / (limits.bottom - limits.top);
	return QPoint(x, y);
}

complex point2complex(QPoint p, const QRect &res, const QRect &stretched, const Limits &limits)
{
	// Convert point to complex
	double xStretch = (double)stretched.width() / res.width();
	double yStretch = (double)stretched.height() / res.height();
	double real = p.x() * (limits.right - limits.left) / (xStretch * (res.width() - 1)) + limits.left;
	double imag = p.y() * (limits.bottom - limits.top) / (yStretch * (res.height() - 1)) + limits.top;
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
