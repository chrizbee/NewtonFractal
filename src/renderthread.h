// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include "parameters.h"
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QElapsedTimer>
#include <QColor>

class RenderThread : public QThread
{
	Q_OBJECT

public:
	RenderThread(QObject *parent = nullptr);
	~RenderThread();
	void render(Parameters params);

protected:
	void run() override;
	void renderPixmap();
	void renderOrbit();

signals:
	void fractalRendered(const QPixmap &pixmap, double fps);
	void orbitRendered(const QVector<QPoint> &orbit, double fps);

private:
	bool abort_;
	QMutex mutex_;
	QWaitCondition condition_;
	QElapsedTimer timer_;
	Parameters curParams_;
	Parameters nextParams_;
};

#endif // RENDERTHREAD_H
