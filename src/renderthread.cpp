#include "renderthread.h"
#include <QImage>
#include <QPixmap>

#define XA	-1.0
#define XB   1.0
#define YA	-1.0
#define YB   1.0
#define H	 1e-6	// Step size for numerical derivative
#define EPS	 1e-3	// Max error allowed
#define N	 6		// Number of roots

static complex step(H, H);
static QColor colors[N] = { Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow };

RenderThread::RenderThread(QObject *parent) :
	QThread(parent),
	abort_(false)
{
}

RenderThread::~RenderThread()
{
	// Abort and wait for thread
	mutex_.lock();
	abort_ = true;
	condition_.wakeOne();
	mutex_.unlock();
	wait();
}

void RenderThread::render(Parameters params)
{
	// Lock mutex
	QMutexLocker locker(&mutex_);
	nextParams_ = params;
	if (nextParams_.roots.length() > N) {
		nextParams_.roots.resize(N);
	}

	// Start working
	if (!isRunning()) {
		start(QThread::NormalPriority);
	}
}

void RenderThread::run()
{
	// Run foooooorrreeeeevvvvvveeeeer
	while (1) {

		// Get new parameters
		mutex_.lock();
		currentParams_ = nextParams_;
		mutex_.unlock();

		// Create image for fast pixel IO
		QImage image(currentParams_.resultSize, QImage::Format_RGB32);
		qint32 width = currentParams_.resultSize.width();
		qint32 height = currentParams_.resultSize.height();
		quint8 rootCount = currentParams_.roots.length();
		double darkerFactor = 300.0 / currentParams_.maxIterations;

		// Iterate y-pixels
		for (int y = 0; y < height; ++y) {
			double zy = y * (YB - YA) / (height - 1) + YA;

			// Iterate x-pixels
			for (int x = 0; x < width; ++x) {
				double zx = x * (XB - XA) / (width - 1) + XA;

				// Create complex number from current pixel
				complex z(zx, zy);

				// Newton iteration
				for (quint8 i = 0; i < currentParams_.maxIterations; ++i) {
					complex dz = (f(z + step) - f(z)) / step;
					complex z0 = z - f(z) / dz;

					// If root has been found check which
					if (abs(z0 - z) < EPS) {
						for (quint8 r = 0; r < rootCount; ++r) {
							if (abs(z0 - currentParams_.roots[r]) < EPS) {
								QColor c = colors[r].darker(50 + i * darkerFactor);
								image.setPixel(x, y, c.rgb());
								break;
							}
						}
						break;

					// Else next iteration
					} else z = z0;
				}
			}
		}

		// Return if aborted, else emit signal
		if (abort_) return;
		emit fractalRendered(QPixmap::fromImage(image));
	}
}

complex RenderThread::f(complex z)
{
	// Calculate function with given roots <-- TODO: super slow and super wrong
//	complex result;
//	quint8 rootCount = currentParams_.roots.length();
//	for (quint8 i = 0; i < rootCount; ++i) {
//		result = result * (z - currentParams_.roots[i]);
//	}

//	return result;
	return z * z * z - 1.0;
}
