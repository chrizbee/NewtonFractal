// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "renderthread.h"
#include <QtConcurrent>
#include <QImage>
#include <QPixmap>
#include <QElapsedTimer>

static const QColor colors[NRT] = { Qt::red, Qt::green, Qt::blue, Qt::cyan, Qt::magenta, Qt::yellow };

inline complex func(complex z, const RootVector &roots)
{
	// Calculate function with given roots
	quint8 rootCount = roots.length();
	complex result = z - roots[0];
	for (quint8 i = 1; i < rootCount; ++i) {
			result = result * (z - roots[i]);
	}
	return result;
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
	first_(false),
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
		QElapsedTimer timer;
		timer.start();

		// Get new parameters and return if same as before
		bool keepRunning = true;
		mutex_.lock();
		if (nextParams_ == curParams_) {
			keepRunning = false;
		} else curParams_ = nextParams_;
		mutex_.unlock();
		if (!keepRunning && first_) return;

		// Create image for fast pixel IO
		QList<ImageLine> lineList;
		QSize size = curParams_.size * (curParams_.scaleDown ? curParams_.scaleDownFactor : 1);
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
		if (abort_) return;
		emit fractalRendered(QPixmap::fromImage(image), 1000.0 / timer.elapsed());
	}
}

void iterateX(ImageLine &il)
{
	// Iterate x-pixels
	const quint8 rootCount = il.params.roots.size();
	const Limits limits = il.params.limits;
	const double xFactor = limits.width() / (il.lineSize - 1);

	for (int x = 0; x < il.lineSize; ++x) {

		// Create complex number from current pixel
		il.zx = x * xFactor + limits.left();
		complex z(il.zx, il.zy);

		// Newton iteration
		for (quint16 i = 0; i < il.params.maxIterations; ++i) {
			const complex fz = func(z, il.params.roots);
			const complex dz = (func(z + STEP, il.params.roots) - fz) * INV_STEP;
			const complex z0 = z - il.params.damping * fz / dz;

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
