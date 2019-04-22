#ifndef FRACTALWIDGET_H
#define FRACTALWIDGET_H

#include <QWidget>
#include "renderthread.h"
#include "parameters.h"

class FractalWidget : public QWidget
{
	Q_OBJECT

public:
	FractalWidget(QWidget *parent = nullptr);
	Parameters params() const;
	void setParams(Parameters params);
	void resetRoots();

public slots:
	void updateFractal(const QPixmap &pixmap);

protected:
	void paintEvent(QPaintEvent *);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

signals:
	void rootMoved(quint8 index, complex value);

private:
	QPixmap pixmap_;
	Parameters params_;
	RenderThread renderThread_;
};

#endif // FRACTALWIDGET_H
