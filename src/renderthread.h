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
#include <QColor>

struct ImageLine {
	ImageLine(QRgb *scanLine, int lineIndex, int lineSize, const Parameters &params);
	QRgb *scanLine;
	int lineIndex;
	int lineSize;
	double zx;
	double zy;
	const Parameters &params;
};

class RenderThread : public QThread
{
	Q_OBJECT

public:
	RenderThread(QObject *parent = nullptr);
	~RenderThread();
	void render(Parameters params);

protected:
	void run() override;

signals:
	void fractalRendered(const QPixmap &pixmap, double fps);

private:
	bool first_;
	bool abort_;
	QMutex mutex_;
	QWaitCondition condition_;
	Parameters currentParams_;
	Parameters nextParams_;
};

void iterateX(ImageLine &il);
complex func(complex z, const RootVector &roots);

#endif // RENDERTHREAD_H
