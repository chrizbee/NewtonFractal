#include "renderthread.h"
#include <QtConcurrent>
#include <QImage>
#include <QPixmap>
#include <QElapsedTimer>

static const QColor colors[NR] = { Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow };

ImageLine::ImageLine(QRgb *scanLine, int lineIndex, int lineSize, const Parameters &params) :
	scanLine(scanLine),
	lineIndex(lineIndex),
	lineSize(lineSize),
	params(params)
{
}

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
	if (nextParams_.roots.length() > NR) {
		nextParams_.roots.resize(NR);
	}

	// Start working
	if (!isRunning()) {
		start(QThread::NormalPriority);
	}
}

void RenderThread::run()
{
	// Run forever
	while (1) {

		// Start timer to measure fps
		QElapsedTimer timer;
		timer.start();

		// Get new parameters and return if same as before
		bool keepRunning = true;
		mutex_.lock();
		if (nextParams_ == currentParams_) {
			keepRunning = false;
		} else currentParams_ = nextParams_;
		mutex_.unlock();
		if (!keepRunning) return;

		// Create image for fast pixel IO
		QList<ImageLine> lineList;
		QImage image(currentParams_.size, QImage::Format_RGB32);
		qint32 height = currentParams_.size.height();
		Limits limits = currentParams_.limits;

		// Iterate y-pixels
		for (int y = 0; y < height; ++y) {
			ImageLine il((QRgb*)(image.scanLine(y)), y, image.width(), currentParams_);
			il.zy = y * (limits.bottom - limits.top) / (height - 1) + limits.top;
			lineList.append(il);
		}

		// Iterate x-pixels with one threads
		if (!currentParams_.multiThreaded) {
			for (ImageLine &il : lineList) {
				iterateX(il);
			}

		// Iterate x-pixels with multiple threads
		// TODO: use map to report progress with QFuture
		} else QtConcurrent::blockingMap(lineList, iterateX);

		// Return if aborted, else emit signal
		if (abort_) return;
		emit fractalRendered(QPixmap::fromImage(image), 1000.0 / timer.elapsed());
	}
}

void iterateX(ImageLine &il)
{
	// Iterate x-pixels
	quint8 rootCount = il.params.roots.size();
	Limits limits = il.params.limits;

	for (int x = 0; x < il.lineSize; ++x) {

		// Create complex number from current pixel
		il.zx = x * (limits.right - limits.left) / (il.lineSize - 1) + limits.left;
		complex z(il.zx, il.zy);
		QColor color(Qt::black);

		// Newton iteration
		for (quint16 i = 0; i < il.params.maxIterations; ++i) {
			complex dz = (func(z + STEP, il.params.roots) - func(z, il.params.roots)) * INV_STEP;
			complex z0 = z - func(z, il.params.roots) / dz;

			// If root has been found set color and break
			if (abs(z0 - z) < EPS) {
				for (quint8 r = 0; r < rootCount; ++r) {
					if (abs(z0 - il.params.roots[r]) < EPS) {
						color = colors[r].darker(50 + i * 8);
						break;
					}
				}
				break;

			// Else next iteration
			} else z = z0;
		}

		// Color the pixel
		il.scanLine[x] = color.rgb();
	}
}

complex func(complex z, const RootVector &roots)
{
	// Calculate function with given roots
	complex result(1, 0);
	quint8 rootCount = roots.length();
	for (quint8 i = 0; i < rootCount; ++i) {
		result = result * (z - roots[i]);
	}
	return result;
}
