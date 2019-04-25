#include "fractalwidget.h"
#include <QApplication>
#include <QMouseEvent>
#include <QDateTime>
#include <QPainter>

static QPoint lastPos;
static int draggedIndex = -1;

FractalWidget::FractalWidget(QWidget *parent) :
	QWidget(parent),
	params_(Parameters(DRC)),
	draggingMode_(NoDragging)
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

void FractalWidget::updateParams(Parameters params)
{
	// Pass params to render thread
	params_ = params;
	renderThread_.render(params_);
}

void FractalWidget::reset()
{
	// Reset roots to be equidistant
	quint8 rootCount = params_.roots.size();
	params_.roots = equidistantRoots(rootCount);
	params_.limits = Limits();
	updateParams(params_);
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
		text = tr("F1 → Settings");

		// Draw roots
		rootPoints_.clear();
		painter.setPen(rootPen);
		quint8 rootCount = params_.roots.size();
		for (quint8 i = 0; i < rootCount; ++i) {
			QPoint point = complex2point(params_.roots[i], pixmap_.rect(), rect(), params_.limits);
			painter.drawEllipse(point, RR, RR);
			rootPoints_.append(point);
		}

	} else text = tr("Rendering initial image, please wait...");

	// Draw text
	QFontMetrics metrics = painter.fontMetrics();
	int textWidth = metrics.horizontalAdvance(text);
	painter.setPen(Qt::NoPen);
	painter.drawRect((width() - textWidth) / 2 - 6, 0, textWidth + 10, metrics.lineSpacing() + 6);
	painter.setPen(whitePen);
	painter.drawText((width() - textWidth) / 2, metrics.leading() + metrics.ascent() + 1, text);
}

void FractalWidget::mousePressEvent(QMouseEvent *event)
{
	// Check if mouse press is on root
	QPoint pos = event->pos();
	quint8 rootCount = rootPoints_.length();
	for (quint8 i = 0; i < rootCount; ++i) {
		if (rootContainsPoint(rootPoints_[i], pos)) {
			setCursor(Qt::ClosedHandCursor);
			draggingMode_ = DraggingRoot;
			draggedIndex = i;
			return;
		}
	}

	// Else dragging fractal
	setCursor(Qt::SizeAllCursor);
	draggingMode_ = DraggingFractal;
	lastPos = event->pos();
}

void FractalWidget::mouseMoveEvent(QMouseEvent *event)
{
	// Move root if dragging
	QPoint pos = event->pos();
	if (draggingMode_ == DraggingRoot && draggedIndex >= 0 && draggedIndex < rootPoints_.length()) {
		params_.roots[draggedIndex] = point2complex(pos, pixmap_.rect(), rect(), params_.limits);
		updateParams(params_);
		emit rootMoved(draggedIndex, params_.roots[draggedIndex]);

	// Else move fractal if dragging
	} else if (draggingMode_ == DraggingFractal) {
		complex d = distance2complex(lastPos - event->pos(), pixmap_.rect(), rect(), params_.limits);
		params_.limits.move(d.real(), d.imag());
		updateParams(params_);
		lastPos = event->pos();

	// Else if event over root change cursor
	} else {
		quint8 rootCount = rootPoints_.length();
		for (quint8 i = 0; i < rootCount; ++i) {
			if (rootContainsPoint(rootPoints_[i], pos)) {
				setCursor(Qt::OpenHandCursor);
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
	draggingMode_ = NoDragging;
	draggedIndex = -1;
}

void FractalWidget::wheelEvent(QWheelEvent *event)
{
	// Calculate weight
	double xw = 0.5, yw = 0.5;
	if (params_.zoomToCursor) {
		xw = (double)event->pos().x() / width();
		yw = (double)event->pos().y() / height();
	}

	// Zoom fractal in / out
	params_.limits.zoom(event->angleDelta().y() > 0, xw, yw);
	updateParams(params_);
}
