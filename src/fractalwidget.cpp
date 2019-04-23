#include "fractalwidget.h"
#include <QMouseEvent>
#include <QDateTime>
#include <QPainter>
#include <QDebug>

static bool dragging = false;
static int draggedIndex = -1;

FractalWidget::FractalWidget(QWidget *parent) :
	QWidget(parent)
{
	// Enable mouse movements and connect render thread
	setMouseTracking(true);
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

void FractalWidget::exportTo(const QString &exportDir)
{
	// Export fractal to file
	QString filePath = exportDir + "/fractal_" +
		QDateTime::currentDateTime().toString("yyMMdd_HHmmss_") +
		QString::number(params_.roots.size()) + "roots_" +
		QString::number(params_.resultSize.width()) + "px.png";
	QFile f(filePath);
	f.open(QIODevice::WriteOnly | QIODevice::Truncate);
	pixmap_.save(&f, "png");
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
		rootPoints_.clear();
		painter.setPen(rootPen);
		quint8 rootCount = params_.roots.size();
		for (quint8 i = 0; i < rootCount; ++i) {
			QPoint rootPoint = complex2point(params_.roots[i], rect(), pixmap_.rect());
			painter.drawEllipse(rootPoint, RR, RR);
			rootPoints_.append(rootPoint);
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
	// Check if mouse press is on root
	QPoint pos = event->pos();
	quint8 rootCount = rootPoints_.length();
	for (quint8 i = 0; i < rootCount; ++i) {
		if (rootContainsPoint(rootPoints_[i], pos)) {
			setCursor(Qt::SizeAllCursor);
			dragging = true;
			draggedIndex = i;
			return;
		}
	}
}

void FractalWidget::mouseMoveEvent(QMouseEvent *event)
{
	// Test if event over root
	QPoint pos = event->pos();
	if (dragging && draggedIndex >= 0 && draggedIndex < rootPoints_.length()) {
		params_.roots[draggedIndex] = point2complex(pos, rect(), pixmap_.rect());
		setParams(params_);
		emit rootMoved(draggedIndex, params_.roots[draggedIndex]);
	} else {
		quint8 rootCount = rootPoints_.length();
		for (quint8 i = 0; i < rootCount; ++i) {
			if (rootContainsPoint(rootPoints_[i], pos)) {
				setCursor(Qt::SizeAllCursor);
				return;
			}
		}
		setCursor(Qt::ArrowCursor);
	}
}

void FractalWidget::mouseReleaseEvent(QMouseEvent *event)
{
	// Reset dragging
	Q_UNUSED(event);
	dragging = false;
	draggedIndex = -1;
}
