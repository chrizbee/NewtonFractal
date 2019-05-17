// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "renderthread.h"
#include <QtConcurrent>
#include <QImage>
#include <QPixmap>

static const QColor colors[NRT] = { Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow };

inline void func(complex z, complex &f, complex &df, const RootVector &roots)
{
	// Calculate f and derivative with given roots
	quint8 rootCount = roots.length();
	if (rootCount < 2) return;

	// TODO: algorithm documentation
	complex r = (z - roots[0]);
	complex l = (z - roots[1]);
	for (quint8 i = 1; i < rootCount - 1; ++i) {
		l = (z - roots[i + 1]) * (l + r);
		r *= (z - roots[i]);
	}
	df = l + r;
	f = r * (z - roots[rootCount - 1]);
}

inline void iterateX(ImageLine &il)
{
	// Iterate x-pixels
	const quint8 rootCount = il.params.roots.size();
	const Limits limits = il.params.limits;
	const double xFactor = limits.width() / (il.lineSize - 1);
	const double d = il.params.damping;

	for (int x = 0; x < il.lineSize; ++x) {

		// Create complex number from current pixel
		il.zx = x * xFactor + limits.left();
		complex z(il.zx, il.zy);

		// Newton iteration
		for (quint16 i = 0; i < il.params.maxIterations; ++i) {
			complex f, df;
			func(z, f, df, il.params.roots);
			complex z0 = z - d * f / df; // <- expensive division

			// If root has been found set color and break
			if (abs(z0 - z) < EPS) {
				for (quint8 r = 0; r < rootCount; ++r) {
					if (abs(z0 - il.params.roots[r]) < EPS) {
						il.scanLine[x] = colors[r].darker(50 + i * 8).rgb();
						goto POINT_DONE;
					}
				}
			}
			z = z0;
		}
		POINT_DONE:;
	}
}

ImageLine::ImageLine(QRgb *scanLine, int lineIndex, int lineSize, const Parameters &params) :
	scanLine(scanLine),
	lineIndex(lineIndex),
	lineSize(lineSize),
	params(params)
{
}

RenderThread::RenderThread(QObject *parent) :
	QThread(parent),
	first_(false)
{
}

RenderThread::~RenderThread()
{
	// Abort and wait for thread
	mutex_.lock();
	condition_.wakeOne();
	mutex_.unlock();
	wait();
}

void RenderThread::render(Parameters params)
{
	// Lock mutex
	QMutexLocker locker(&mutex_);
	nextParams_ = params;
	if (nextParams_.roots.length() > NRT) {
		nextParams_.roots.resize(NRT);
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
		timer_.start();

		// Get new parameters and return if same as before
		bool keepRunning = true;
		mutex_.lock();
		if (nextParams_ == curParams_) {
			keepRunning = false;
		} else curParams_ = nextParams_;
		mutex_.unlock();
		if (!keepRunning && first_) return;

		// Don't rerender if orbitMode
		curParams_.orbitMode ? renderOrbit() : renderPixmap();
	}
}

void RenderThread::renderPixmap()
{
	// Render whole pixmap
	QList<ImageLine> lineList;
	QSize size = curParams_.size * (curParams_.scaleDown ? curParams_.scaleDownFactor : 1);

	// Create image for fast pixel IO
	QImage image(size, QImage::Format_RGB32);
	image.fill(Qt::black);
	const qint32 height = size.height();
	const Limits limits = curParams_.limits;
	const double yFactor = -limits.height() / (height - 1);

	// Iterate y-pixels
	for (int y = 0; y < height; ++y) {
		ImageLine il((QRgb*)(image.scanLine(y)), y, image.width(), curParams_);
		il.zy = y * yFactor + limits.top();
		lineList.append(il);
	}

	// Iterate x-pixels with one threads
	if (!curParams_.multiThreaded) {
		for (ImageLine &il : lineList) {
			iterateX(il);
		}

	// Iterate x-pixels with multiple threads
	// TODO: use map to report progress with QFuture
	} else QtConcurrent::blockingMap(lineList, iterateX);

	// Return if aborted, else emit signal
	first_ = true;
	emit fractalRendered(QPixmap::fromImage(image), 1000.0 / timer_.elapsed());
}

void RenderThread::renderOrbit()
{
	// Create vector of points
	QVector<QPoint> orbit;
	const double d = curParams_.damping;

	// Create complex number from current pixel
	complex z = point2complex(curParams_.orbitStart, curParams_);
	orbit.append(complex2point(z, curParams_));

	// Newton iteration
	for (quint16 i = 0; i < curParams_.maxIterations; ++i) {
		complex f, df;
		func(z, f, df, curParams_.roots);
		complex z0 = z - d * f / df;

		// Append point to vector
		orbit.append(complex2point(z0, curParams_));

		// Break if root has been found
		if (abs(z0 - z) < EPS) break;
		z = z0;
	}

	// Emit signal
	emit orbitRendered(orbit, 1000.0 / timer_.elapsed());
}
