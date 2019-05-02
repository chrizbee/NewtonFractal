// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef FRACTALWIDGET_H
#define FRACTALWIDGET_H

#include "renderthread.h"
#include "parameters.h"
#include <QTimer>
#include <QWidget>

enum DraggingMode : quint8 { NoDragging, DraggingRoot, DraggingFractal };

struct Dragger {
	Dragger();
	DraggingMode mode;
	QPoint previousPos;
	int index;
};

class FractalWidget : public QWidget
{
	Q_OBJECT

public:
	FractalWidget(QWidget *parent = nullptr);
	Parameters params() const;
	void updateParams(Parameters params);
	void exportTo(const QString &exportDir);
	void reset();

public slots:
	void updateFractal(const QPixmap &pixmap, double fps);

protected:
	void paintEvent(QPaintEvent *) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

signals:
	void rootMoved(quint8 index, complex value);
	void zoomChanged(double factor);

private:
	QTimer timer_;
	QPixmap pixmap_;
	Parameters params_;
	RenderThread renderThread_;
	QList<QPoint> rootPoints_;
	Dragger dragger_;
	double fps_;
};

#endif // FRACTALWIDGET_H
