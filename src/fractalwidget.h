// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef FRACTALWIDGET_H
#define FRACTALWIDGET_H

#include "renderthread.h"
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>

struct Parameters;
class SettingsWidget;

enum DraggingMode : quint8 { NoDragging, DraggingRoot, DraggingFractal };

struct Dragger {
	Dragger();
	DraggingMode mode;
	QPoint previousPos;
	int index;
};

class FractalWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	FractalWidget(QWidget *parent = nullptr);
	void updateParams();
	void exportImageTo(const QString &dir);
	void exportSettingsTo(const QString &dir);
	void importSettingsFrom(const QString &file);
	void reset();

public slots:
	void updateFractal(const QPixmap &pixmap, double fps);
	void updateOrbit(const QVector<QPoint> &orbit, double fps);

protected:
	void initializeGL() override;
	void paintGL() override;
	void resizeGL(int w, int h) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

signals:
	void rootMoved(quint8 index, complex value);
	void zoomChanged(double factor);
	void sizeChanged(QSize size);

private:
	QTimer timer_;
	QPixmap pixmap_;
	QVector<QPoint> orbit_;
	Parameters *params_;
	SettingsWidget *settingsWidget_;
	QOpenGLShaderProgram *program_;
	RenderThread renderThread_;
	Dragger dragger_;
	double fps_;
	bool legend_;
	bool position_;
};

#endif // FRACTALWIDGET_H
