#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QColor>
#include "parameters.h"

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
	void fractalRendered(const QPixmap &pixmap);

private:
	bool abort_;
	QMutex mutex_;
	QWaitCondition condition_;
	Parameters currentParams_;
	Parameters nextParams_;
};

void iterateX(ImageLine &il);
complex func(complex z, const RootVector &roots);

#endif // RENDERTHREAD_H
