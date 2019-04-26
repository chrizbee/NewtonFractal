#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QVector>
#include <QSize>
#include <QRect>
#include <complex>

typedef std::complex<double> complex;
typedef QVector<std::complex<double>> RootVector;

static constexpr double PI = 3.141592653589793238463;	// Pi as a constexpr

static constexpr double HS = 1e-6;						// Step size for numerical derivative
static constexpr double EPS	= 1e-3;						// Max error allowed
static constexpr quint8 NR = 6;							// Number of roots
static constexpr quint8 RR = 5;							// Root indicator radius

static constexpr quint8 DRC = 5;						// Default root count
static constexpr double DZM = 0.05;						// Default zoom factor
static constexpr double DSC = 0.5;						// Default zoom factor
static constexpr quint16 DTI = 400;						// Default timer interval
static constexpr quint16 DMI = 160;						// Default max. iterations
static constexpr quint16 DSI = 700;						// Default size
static constexpr quint16 DZS = 2;						// Default complex size [-DZS -> +DZS]
static constexpr double DSF = 0.5 * DZS / DSI;			// Resulting size factor

static constexpr complex STEP(HS, HS);					// Step size for numerical derivative
static constexpr complex INV_STEP(0.5 / HS, -0.5 / HS);	// Inv step size for numerical derivative

struct Limits {
	Limits();
	bool operator==(const Limits &other) const;
	void move(double dx, double dy);
	void zoom(bool in, double xw, double yw);
	void reset(QSize size);
	double left;
	double right;
	double top;
	double bottom;
	double zoomFactor;
};

struct Parameters {
	Parameters(quint8 rootCount = DRC);
	bool operator==(const Parameters &other) const;
	void resize(QSize newSize, bool resizeLimits = false);
	RootVector roots;
	Limits limits;
	QSize size;
	double scaleDown;
	quint16 maxIterations;
	bool multiThreaded;
	bool zoomToCursor;
};

QString complex2string(complex z);
complex string2complex(QString s);
QPoint complex2point(complex z, const QRect &res, const QRect &stretched, const Limits &limits);
complex point2complex(QPoint p, const QRect &res, const QRect &stretched, const Limits &limits);
complex distance2complex(QPoint d, const QRect &res, const QRect &stretched, const Limits &limits);
RootVector equidistantRoots(quint8 rootCount);
bool rootContainsPoint(QPoint root, QPoint point);

#endif // PARAMETERS_H
