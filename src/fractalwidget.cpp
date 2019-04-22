#include "fractalwidget.h"
#include <QPainter>
#include <QDebug>

FractalWidget::FractalWidget(QWidget *parent) :
	QWidget(parent)
{
	// Connect render thread
	connect(&renderThread_, &RenderThread::fractalRendered, this, &FractalWidget::updateFractal);
}

Parameters FractalWidget::params() const
{
	// Return current params
	return params_;
}

void FractalWidget::setParams(Parameters params)
{
	// Pass params to render thread
	params_ = params;
	renderThread_.render(params_);
}

void FractalWidget::resetRoots()
{
	// Reset roots to be equidistant
	quint8 rootCount = params_.roots.size();
	params_.roots = equidistantRoots(rootCount);
	setParams(params_);
	for (quint8 i = 0; i < rootCount; ++i) {
		emit rootMoved(i, params_.roots[i]);
	}
}

void FractalWidget::updateFractal(const QPixmap &pixmap)
{
	// Update
	pixmap_ = pixmap;
	update();
}

void FractalWidget::paintEvent(QPaintEvent *)
{
	// Static colors
	static const QPen whitePen(Qt::white);
	static const QPen rootPen(Qt::white, 2);
	static const QBrush opaqueBrush(QColor(0, 0, 0, 128));

	// Paint fractal
	QPainter painter(this);
	QString text;
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(rect(), Qt::darkGray);
	painter.setBrush(opaqueBrush);

	// Draw pixmap if rendered yet
	if (!pixmap_.isNull()) {
		painter.drawPixmap(rect(), pixmap_);
		text = tr("Press F1 to change settings.");

		// Draw roots
		painter.setPen(rootPen);
		quint8 rootCount = params_.roots.size();
		double xStretch = (double)width() / pixmap_.width();
		double yStretch = (double)height() / pixmap_.height();

		// TODO: create list with roots (x, y)

		for (quint8 i = 0; i < rootCount; ++i) {
			double zx = params_.roots[i].real();
			double zy = params_.roots[i].imag();
			int y = yStretch * (zy - YA) * (pixmap_.height() - 1) / (YB - YA);
			int x = xStretch * (zx - XA) * (pixmap_.width() - 1) / (XB - XA);
			painter.drawEllipse(QPoint(x, y), 5, 5);
		}
	} else text = tr("Rendering initial image, please wait...");

	// Draw text
	QFontMetrics metrics = painter.fontMetrics();
	int textWidth = metrics.horizontalAdvance(text);
	painter.setPen(Qt::NoPen);
	painter.drawRect((width() - textWidth) / 2 - 5, 0, textWidth + 10, metrics.lineSpacing() + 5);
	painter.setPen(whitePen);
	painter.drawText((width() - textWidth) / 2, metrics.leading() + metrics.ascent(), text);
}

void FractalWidget::mousePressEvent(QMouseEvent *event)
{

}

void FractalWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void FractalWidget::mouseReleaseEvent(QMouseEvent *event)
{

}
