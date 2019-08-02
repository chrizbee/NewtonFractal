// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "renderthread.h"
#include "imageline.h"
#include <QtConcurrent>
#include <QImage>
#include <QPixmap>

inline void func(complex z, complex &f, complex &df, const QVector<Root> &roots)
{
	// Calculate f and derivative with given roots
	quint8 rootCount = roots.length();
	if (rootCount < 2) return;

	// TODO: algorithm documentation
	complex r = (z - roots[0].value());
	complex l = (z - roots[1].value());
	for (quint8 i = 1; i < rootCount - 1; ++i) {
		l = (z - roots[i + 1].value()) * (l + r);
		r *= (z - roots[i].value());
	}
	df = l + r;
	f = r * (z - roots[rootCount - 1].value());
}

inline void iterateX(ImageLine &il)
{
	// Iterate x-pixels
	const quint8 rootCount = il.params.roots.count();
	const Limits limits = il.params.limits;
	const double xFactor = limits.width() / (il.lineSize - 1);
	const complex d = il.params.damping;

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
			if (abs(z0 - z) < nf::EPS) {
				for (quint8 r = 0; r < rootCount; ++r) {
					if (abs(z0 - il.params.roots[r].value()) < nf::EPS) {
						il.scanLine[x] = il.params.roots[r].color().darker(50 + i * 8).rgb();
						goto POINT_DONE;
					}
				}
			}
			z = z0;
		}
		POINT_DONE:;
	}
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

	// Start working
	if (!isRunning()) {
		start(QThread::NormalPriority);
	}
}

void RenderThread::run()
{
	// Static var, to test if for first render
	static bool firstRender = false;

	// Run forever
	while (1) {

		// Start timer to measure fps
		timer_.start();
		bool paramsChanged = true;
		bool orbitChanged = true;

		// Get new parameters and return if same as before
		mutex_.lock();
		orbitChanged = nextParams_.orbitChanged(curParams_);
		paramsChanged = nextParams_.paramsChanged(curParams_);
		if (paramsChanged || orbitChanged) curParams_ = nextParams_;
		mutex_.unlock();

		// Stop if nothing changed
		if (!paramsChanged && !orbitChanged && firstRender)
			return;

		// Rerender pixmap
		if (paramsChanged || !firstRender)
			renderPixmap();

		// Rerender orbit
		if (orbitChanged)
			renderOrbit();

		// Abort if wanted
		if (abort_) return;
		firstRender = true;
	}
}

void RenderThread::renderPixmap()
{
	// Render whole pixmap
	QList<ImageLine> lines;
	QSize size = curParams_.size * ((curParams_.scaleDown && curParams_.processor != GPU_OPENGL) ? curParams_.scaleDownFactor : 1);

	// Create image for fast pixel IO
	QPixmap pixmap;
	QImage image(size, QImage::Format_RGB32);
	image.fill(Qt::black);
	const qint32 height = size.height();
	const Limits limits = curParams_.limits;
	const double yFactor = -limits.height() / (height - 1);

	// Iterate y-pixels
	for (int y = 0; y < height; ++y) {
		ImageLine il((QRgb*)(image.scanLine(y)), y, image.width(), curParams_);
		il.zy = y * yFactor + limits.top();
		lines.append(il);
	}

	// Iterate x-pixels with one threads
	if (curParams_.processor == CPU_SINGLE) {
		for (ImageLine &il : lines) {
			iterateX(il);
		}
		pixmap = QPixmap::fromImage(image);

	// Iterate x-pixels with multiple threads
	// TODO: use map to report progress with QFuture
	} else if (curParams_.processor == CPU_MULTI) {
		QtConcurrent::blockingMap(lines, iterateX);
		pixmap = QPixmap::fromImage(image);

	// Use OpenGL in FractalWidget because OpenGL is NOT threadsafe
	}

	// Emit signal
	emit fractalRendered(pixmap, 1000.0 / timer_.elapsed());
}

void RenderThread::renderOrbit()
{
	// Create vector of points
	QVector<QPoint> orbit;
	const complex d = curParams_.damping;

	// Create complex number from current pixel
	complex z = curParams_.point2complex(curParams_.orbitStart);
	orbit.append(curParams_.complex2point(z));

	// Newton iteration
	for (quint16 i = 0; i < curParams_.maxIterations; ++i) {
		complex f, df;
		func(z, f, df, curParams_.roots);
		complex z0 = z - d * f / df;

		// Append point to vector
		orbit.append(curParams_.complex2point(z0));

		// Break if root has been found
		if (abs(z0 - z) < nf::EPS) break;
		z = z0;
	}

	// Emit signal
	emit orbitRendered(orbit, 1000.0 / timer_.elapsed());
}
