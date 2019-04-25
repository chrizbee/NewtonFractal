#include "parameters.h"

static const double PI = 3.141592653589793238463;

Limits::Limits() :
	left(-1.0),
	right(1.0),
	top(1.0),
	bottom(-1.0),
	zoomFactor(DZM)
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

void Limits::move(double dx, double dy)
{
	// Move limits
	left += dx;
	right += dx;
	top += dy;
	bottom += dy;
}

void Limits::zoom(bool in, double xw, double yw)
{
	// Zoom limits in / out
	double zoom = in ? -zoomFactor : zoomFactor;
	double wZoom = (right - left) * zoom;
	double hZoom = (top - bottom) * zoom;
	left -= xw * wZoom;
	right += (1.0 - xw) * wZoom;
	top += yw * hZoom;
	bottom -= (1.0 - yw) * hZoom;
}

Parameters::Parameters(quint8 rootCount) :
	roots(equidistantRoots(rootCount)),
	limits(Limits()),
	resultSize(QSize(DSI, DSI)),
	maxIterations(DMI),
	multiThreaded(true),
	zoomToCursor(true)
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
		multiThreaded == other.multiThreaded &&
		zoomToCursor == other.zoomToCursor
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

complex distance2complex(QPoint d, const QRect &res, const QRect &stretched, const Limits &limits)
{
	// Convert distance to complex
	double xStretch = (double)stretched.width() / res.width();
	double yStretch = (double)stretched.height() / res.height();
	double real = d.x() * (limits.right - limits.left) / (xStretch * (res.width() - 1));
	double imag = d.y() * (limits.bottom - limits.top) / (yStretch * (res.height() - 1));
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
