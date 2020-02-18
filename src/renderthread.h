// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include "parameters.h"
#include <QThread>
#include <QtConcurrent>
#include <QWaitCondition>
#include <QElapsedTimer>
#include <QMutex>
#include <QColor>

class RenderThread : public QThread
{
	Q_OBJECT

public:
	RenderThread(QObject *parent = nullptr);
	~RenderThread();
	void render(const Parameters &params);

protected:
	void run() override;
	void renderPixmap();
	void renderOrbit();
	void renderBenchmark();

signals:
	void fractalRendered(const QPixmap &pixmap, double fps);
	void orbitRendered(const QVector<QPoint> &orbit, double fps);
	void benchmarkProgress(int min, int max, int progress);
	void benchmarkFinished(const QImage &image);

private:
	bool abort_;
	QMutex mutex_;
	QWaitCondition condition_;
	QElapsedTimer timer_;
	Parameters curParams_;
	Parameters nextParams_;
};

#endif // RENDERTHREAD_H
