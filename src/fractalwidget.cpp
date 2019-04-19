#include "fractalwidget.h"
#include <QPainter>

FractalWidget::FractalWidget(QWidget *parent) :
	QWidget(parent)
{
	// Connect render thread
	connect(&renderThread_, &RenderThread::fractalRendered, this, &FractalWidget::updateFractal);
}

void FractalWidget::render(Parameters params)
{
	// Pass params to render thread
	renderThread_.render(params);
}

void FractalWidget::updateFractal(const QPixmap &pixmap)
{
	// Update
	pixmap_ = pixmap;
	update();
}

void FractalWidget::paintEvent(QPaintEvent *)
{
	// Paint fractal
	QPainter painter(this);
	QString text;
	painter.fillRect(rect(), Qt::gray);

	// Draw pixmap if rendered yet
	if (!pixmap_.isNull()) {
		painter.drawPixmap(rect(), pixmap_);
		text = tr("Press F1 to change settings.");
	} else {
		text = tr("Rendering initial image, please wait...");
	}

	// Draw text
	QFontMetrics metrics = painter.fontMetrics();
	int textWidth = metrics.horizontalAdvance(text);
	painter.setPen(Qt::NoPen);
	painter.setBrush(QColor(0, 0, 0, 127));
	painter.drawRect((width() - textWidth) / 2 - 5, 0, textWidth + 10, metrics.lineSpacing() + 5);
	painter.setPen(Qt::white);
	painter.drawText((width() - textWidth) / 2, metrics.leading() + metrics.ascent(), text);

	// TODO: scaling & scrolling
}
