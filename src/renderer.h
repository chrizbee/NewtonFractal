// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef RENDERER_H
#define RENDERER_H

#include "parameters.h"
#include <QObject>

#ifndef WASM
#include "imageline.h"
#include <QElapsedTimer>
#include <QtConcurrent>
#endif

class Renderer : public QObject
{
	Q_OBJECT

public:
	Renderer(QObject *parent = nullptr);
	~Renderer();
	void render(const Parameters &params);
#ifndef WASM
	void stop();

public slots:
	void onProgressChanged(int value);
	void onFinished();
#endif

protected:
	void run();
	void renderOrbit();
#ifndef WASM
	void renderFractal();
#endif

signals:
	void orbitRendered(const QVector<QPoint> &orbit);
#ifndef WASM
	void fractalRendered(const QPixmap &pixmap, double fps);
	void benchmarkProgress(int min, int max, int progress);
	void benchmarkFinished(const QImage *image);
#endif

private:
	Parameters curParams_;
	Parameters nextParams_;
#ifndef WASM
	QElapsedTimer timer_;
	QScopedPointer<QImage> imagep_;
	QScopedPointer<QVector<ImageLine>> linesp_;
	QFutureWatcher<void> watcher_;
#endif
};

#endif // RENDERER_H
