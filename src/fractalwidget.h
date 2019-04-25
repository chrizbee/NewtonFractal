#ifndef FRACTALWIDGET_H
#define FRACTALWIDGET_H

#include <QWidget>
#include "renderthread.h"
#include "parameters.h"

enum DraggingMode : quint8 { NoDragging, DraggingRoot, DraggingFractal };

class FractalWidget : public QWidget
{
	Q_OBJECT

public:
	FractalWidget(QWidget *parent = nullptr);
	Parameters params() const;
	void updateParams(Parameters params);
	void reset();
	void exportTo(const QString &exportDir);

public slots:
	void updateFractal(const QPixmap &pixmap);

protected:
	void paintEvent(QPaintEvent *) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

signals:
	void rootMoved(quint8 index, complex value);

private:
	QPixmap pixmap_;
	Parameters params_;
	RenderThread renderThread_;
	QList<QPoint> rootPoints_;
	DraggingMode draggingMode_;
};

#endif // FRACTALWIDGET_H
