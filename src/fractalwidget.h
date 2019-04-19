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
	void render(Parameters params);

public slots:
	void updateFractal(const QPixmap &pixmap);

protected:
	void paintEvent(QPaintEvent *);

private:
	QPixmap pixmap_;
	RenderThread renderThread_;
};

#endif // FRACTALWIDGET_H
