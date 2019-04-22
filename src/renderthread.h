#ifndef RENDERTHREAD_H
#define RENDERTHREAD_H

#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "parameters.h"

struct ImageLine {
	quint8 *scanLine;
	int lineIndex;
	int lineSize;
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
	complex f(complex z);

signals:
	void fractalRendered(const QPixmap &pixmap);

private:
	bool abort_;
	QMutex mutex_;
	QWaitCondition condition_;
	Parameters currentParams_;
	Parameters nextParams_;
};

#endif // RENDERTHREAD_H
