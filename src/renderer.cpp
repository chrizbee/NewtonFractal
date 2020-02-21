// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "renderer.h"
#include <QImage>
#include <QPixmap>
#include <QFutureWatcher>

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
	const quint8 rootCount = il.params->roots.count();
	const double left = il.params->limits.left();
	const double xFactor = il.params->limits.width() / (il.lineSize - 1);
	const complex d = il.params->damping;

	for (int x = 0; x < il.lineSize; ++x) {

		// Create complex number from current pixel
		il.zx = x * xFactor + left;
		complex z(il.zx, il.zy);

		// Newton iteration
		for (quint16 i = 0; i < il.params->maxIterations; ++i) {
			complex f, df;
			func(z, f, df, il.params->roots);
			complex z0 = z - d * f / df; // <- expensive division

			// If root has been found set color and break
			if (abs(z0 - z) < nf::EPS) {
				for (quint8 r = 0; r < rootCount; ++r) {
					if (abs(z0 - il.params->roots[r].value()) < nf::EPS) {
						il.scanLine[x] = il.params->roots[r].color().darker(60 + i * 8).rgb();
						goto POINT_DONE;
					}
				}
			}
			z = z0;
		}
		POINT_DONE:;
	}
}

Renderer::Renderer(QObject *parent) :
	QObject(parent)
{
	// Connect signals
	connect(&watcher_, &QFutureWatcher<void>::finished, this, &Renderer::onFinished);
	connect(&watcher_, &QFutureWatcher<void>::progressValueChanged, this, &Renderer::onProgressChanged);
}

Renderer::~Renderer()
{
	// Cleanup
}

void Renderer::render(const Parameters &params)
{
	// Set next params and run if not running
	nextParams_ = params;
	if (!watcher_.isRunning())
		run();
}

void Renderer::stop()
{
	// Stop if running
	if (watcher_.isRunning())
		watcher_.future().cancel();
}

void Renderer::onProgressChanged(int value)
{
	// Emit signal if benchmarking
	if (curParams_.benchmark)
		emit benchmarkProgress(watcher_.progressMinimum(), watcher_.progressMaximum(), value);
}

void Renderer::onFinished()
{
	// Emit signal
	if (!imagep_.isNull()) {
		if (curParams_.benchmark) emit benchmarkFinished(imagep_.data());
		else emit fractalRendered(QPixmap::fromImage(*imagep_.data()), 1000.0 / timer_.elapsed());
	}
}

void Renderer::run()
{
	// Start timer to measure fps
	timer_.start();
	bool paramsChanged = nextParams_.paramsChanged(curParams_);
	bool orbitChanged =	nextParams_.orbitChanged(curParams_);
	if (paramsChanged || orbitChanged)
		curParams_ = nextParams_;
	else return;

	// Rerender pixmap
	if (paramsChanged)
		renderFractal();

	// Rerender orbit
	if (orbitChanged && !curParams_.benchmark)
		renderOrbit();
}

void Renderer::renderFractal()
{
	// OpenGL not here
	if (curParams_.processor == GPU_OPENGL) {
		emit fractalRendered(QPixmap(), 0);
		return;
	}

	// Get new size
	QSize size = curParams_.size;
	bool bm = curParams_.benchmark;
	bool sd = curParams_.scaleDown;
	size *= bm ? curParams_.scaleUpFactor : sd ? curParams_.scaleDownFactor : 1;

	// TODO: Allocate memory on hard disk and write directly to a file
	// Otherwise RAM won't be enough
	// Until then, the max size is 32767x32767 pixels
	if (bm && (size.width() > 32767 || size.height() > 32767)) {
		emit benchmarkFinished(nullptr); // <- stupid
		return;
	}

	// Create image for fast pixel IO
	const qint32 height = size.height();
	const double yFactor = -curParams_.limits.height() / (height - 1);
	QVector<ImageLine> *lines = new QVector<ImageLine>(height);
	QImage *image = new QImage(size, QImage::Format_RGB32);
	linesp_.reset(lines);
	imagep_.reset(image);
	image->fill(Qt::black);

	// Iterate y-pixels
	for (int y = 0; y < height; ++y) {
		ImageLine il((QRgb*)(image->scanLine(y)), y, image->width(), &curParams_);
		il.zy = y * yFactor + curParams_.limits.top();
		(*lines)[y] = il;
	}

	// Set thread count to either single or multicore
	uint threadCount = curParams_.processor == CPU_SINGLE ? 1 : QThread::idealThreadCount();
	QThreadPool::globalInstance()->setMaxThreadCount(threadCount);

	// Iterate x-pixels with watcher
	watcher_.setFuture(QtConcurrent::map(*lines, iterateX));
}

void Renderer::renderOrbit()
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
