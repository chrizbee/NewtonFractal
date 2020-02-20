// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef RENDERER_H
#define RENDERER_H

#include "parameters.h"
#include "imageline.h"
#include <QObject>
#include <QtConcurrent>
#include <QElapsedTimer>

class Renderer : public QObject
{
	Q_OBJECT

public:
	Renderer(QObject *parent = nullptr);
	~Renderer();
	void render(const Parameters &params);
	void stop();

public slots:
	void onProgressChanged(int value);
	void onFinished();

protected:
	void run();
	void renderFractal();
	void renderOrbit();

signals:
	void fractalRendered(const QPixmap &pixmap, double fps);
	void orbitRendered(const QVector<QPoint> &orbit, double fps);
	void benchmarkProgress(int min, int max, int progress);
	void benchmarkFinished(const QImage *image);

private:
	QElapsedTimer timer_;
	Parameters curParams_;
	Parameters nextParams_;
	QScopedPointer<QImage> imagep_;
	QScopedPointer<QVector<ImageLine>> linesp_;
	QFutureWatcher<void> watcher_;
};

#endif // RENDERER_H
