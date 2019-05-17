// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "fractalwidget.h"
#include "settingswidget.h"
#include "parameters.h"
#include <QApplication>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QDateTime>
#include <QPainter>
#include <QAction>
#include <QIcon>
#include <QDebug>

static bool wasOrbit = false;

Dragger::Dragger() :
	mode(NoDragging),
	index(-1)
{
}

FractalWidget::FractalWidget(QWidget *parent) :
	QWidget(parent),
	params_(new Parameters(DRC)),
	settingsWidget_(new SettingsWidget(params_, this)),
	fps_(0),
	legend_(true)
{
	// Initialize layout
	QSpacerItem *spacer = new QSpacerItem(DSI / 2.0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QHBoxLayout *centralLayout = new QHBoxLayout(this);
	centralLayout->setSpacing(0);
	centralLayout->setContentsMargins(0, 0, 0, 0);
	centralLayout->addItem(spacer);
	centralLayout->addWidget(settingsWidget_);
	setLayout(centralLayout);

	// Add actions and connect signals
	QAction *quit = new QAction(this);
	QAction *hide = new QAction(this);
	QAction *orbit = new QAction(this);
	QAction *settings = new QAction(this);
	quit->setShortcut(QKeySequence("Ctrl+Q"));
	hide->setShortcut(Qt::Key_Escape);
	orbit->setShortcut(Qt::Key_F2);
	settings->setShortcut(Qt::Key_F1);
	addActions(QList<QAction*>() << quit << hide << orbit << settings);
	connect(quit, &QAction::triggered, QApplication::instance(), &QCoreApplication::quit);
	connect(hide, &QAction::triggered, [this]() { legend_ = !legend_; update(); });
	connect(orbit, &QAction::triggered, [this]() { params_->orbitMode = !params_->orbitMode; updateParams(); });
	connect(settings, &QAction::triggered, settingsWidget_, &SettingsWidget::toggle);

	// Initialize general stuff
	setMinimumSize(MSI, MSI);
	setMouseTracking(true);
	setWindowTitle(QApplication::applicationName());
	setWindowIcon(QIcon("://resources/icons/icon.png"));
	timer_.setInterval(DTI);
	timer_.setSingleShot(true);
	resize(params_->size);
	settingsWidget_->hide();

	// Connect settingswidget signals
	connect(settingsWidget_, &SettingsWidget::paramsChanged, this, &FractalWidget::updateParams);
	connect(settingsWidget_, &SettingsWidget::sizeChanged, this, QOverload<const QSize &>::of(&FractalWidget::resize));
	connect(settingsWidget_, &SettingsWidget::exportTo, this, &FractalWidget::exportTo);
	connect(settingsWidget_, &SettingsWidget::reset, this, &FractalWidget::reset);
	connect(this, &FractalWidget::rootMoved, settingsWidget_, &SettingsWidget::moveRoot);
	connect(this, &FractalWidget::zoomChanged, settingsWidget_, &SettingsWidget::changeZoom);
	connect(this, &FractalWidget::sizeChanged, settingsWidget_, &SettingsWidget::changeSize);

	// Connect renderthread and timer signals
	connect(&renderThread_, &RenderThread::fractalRendered, this, &FractalWidget::updateFractal);
	connect(&renderThread_, &RenderThread::orbitRendered, this, &FractalWidget::updateOrbit);
	connect(&timer_, &QTimer::timeout, [this]() {
		params_->orbitMode = wasOrbit;
		params_->scaleDown = false;
		updateParams();
	});

	// Initialize parameters
	reset();
}

void FractalWidget::updateParams()
{
	// Pass params by value to render thread
	renderThread_.render(*params_);
}

void FractalWidget::exportTo(const QString &exportDir)
{
	// Export fractal to file
	QString filePath = exportDir + "/fractal_" +
		QDateTime::currentDateTime().toString("yyMMdd_HHmmss_") +
		QString::number(params_->roots.size()) + "roots_" +
		QString::number(params_->size.width()) + "x" + QString::number(params_->size.width()) + ".png";
	QFile f(filePath);
	f.open(QIODevice::WriteOnly | QIODevice::Truncate);
	pixmap_.save(&f, "png");
}

void FractalWidget::reset()
{
	// Reset roots to be equidistant
	params_->reset();
	updateParams();
	quint8 rootCount = params_->roots.size();
	for (quint8 i = 0; i < rootCount; ++i) {
		emit rootMoved(i, params_->roots[i]);
	}
	emit zoomChanged(params_->limits.zoomFactor());
}

void FractalWidget::updateFractal(const QPixmap &pixmap, double fps)
{
	// Update
	pixmap_ = pixmap;
	fps_ = fps;
	update();
}

void FractalWidget::updateOrbit(const QVector<QPoint> &orbit, double fps)
{
	// Update
	orbit_ = orbit;
	fps_ = fps;
	update();
}

void FractalWidget::paintEvent(QPaintEvent *)
{
	// Static pens and brushes
	static const QPen circlePen(Qt::white, 2);
	static const QBrush opaqueBrush(QColor(0, 0, 0, 128));

	// Static pixmaps
	static const QPixmap pixHide("://resources/icons/hide.png");
	static const QPixmap pixSettings("://resources/icons/settings.png");
	static const QPixmap pixOrbit("://resources/icons/orbit.png");
	static const QPixmap pixFps("://resources/icons/fps.png");

	// Static legend geometry
	static const int spacing = 10;
	static const QFont consolas("Consolas", 12);
	static const QFontMetrics metrics(consolas);
	static const int textWidth = 3 * spacing + pixFps.width() + metrics.width("999.99");
	static const int textHeight = spacing + 4 * (pixFps.height() + spacing);
	static const QRect legendRect(spacing, spacing, textWidth, textHeight);
	static const QPoint ptHide(legendRect.topLeft() + QPoint(spacing, spacing));
	static const QPoint ptSettings(ptHide + QPoint(0, pixHide.height() + spacing));
	static const QPoint ptOrbit(ptSettings + QPoint(0, pixSettings.height() + spacing));
	static const QPoint ptFps(ptOrbit + QPoint(0, pixOrbit.height() + spacing));

	// Paint fractal
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	// Draw pixmap if rendered yet
	if (!pixmap_.isNull()) {
		painter.drawPixmap(rect(), pixmap_);

		// Draw roots
		rootPoints_.clear();
		painter.setPen(circlePen);
		painter.setBrush(opaqueBrush);
		quint8 rootCount = params_->roots.size();
		for (quint8 i = 0; i < rootCount; ++i) {
			QPoint point = complex2point(params_->roots[i], *params_);
			painter.drawEllipse(point, RIR, RIR);
			rootPoints_.append(point);
		}

		// Draw legend if enabled
		if (legend_) {
			painter.setFont(consolas);
			painter.drawRoundedRect(legendRect, 10, 10);
			painter.drawPixmap(ptHide, pixHide);
			painter.drawPixmap(ptSettings, pixSettings);
			painter.drawPixmap(ptOrbit, pixOrbit);
			painter.drawPixmap(ptFps, pixFps);
			painter.drawText(ptHide + QPoint(pixHide.width() + spacing, metrics.height() - 4), "ESC");
			painter.drawText(ptSettings + QPoint(pixSettings.width() + spacing, metrics.height() - 4), "F1");
			painter.drawText(ptOrbit + QPoint(pixOrbit.width() + spacing, metrics.height() - 4), "F2");
			painter.drawText(ptFps + QPoint(pixFps.width() + spacing, metrics.height() - 4), QString::number(fps_, 'f', 2));
		}

		// Draw orbit if enabled
		if (params_->orbitMode) {
			quint32 orbitSize = orbit_.size();
			if (orbitSize >= 1) {
				painter.drawEllipse(orbit_[0], OIR, OIR);
				for (quint32 i = 1; i < orbitSize; ++i) {
					painter.drawLine(orbit_[i - 1], orbit_[i]);
					painter.drawEllipse(orbit_[i], OIR, OIR);
				}
			}
		}
	}
}

void FractalWidget::mousePressEvent(QMouseEvent *event)
{
	// Set scaleDown, previousPos and orbit
	QPoint pos = event->pos();
	wasOrbit = params_->orbitMode;
	params_->orbitMode = false;
	params_->scaleDown = true;
	dragger_.previousPos = pos;

	// Check if mouse press is on root
	quint8 rootCount = rootPoints_.length();
	for (quint8 i = 0; i < rootCount; ++i) {
		if (rootContainsPoint(rootPoints_[i], pos)) {
			setCursor(Qt::ClosedHandCursor);
			dragger_.mode = DraggingRoot;
			dragger_.index = i;
			return;
		}
	}

	// Else dragging fractal
	setCursor(Qt::SizeAllCursor);
	dragger_.mode = DraggingFractal;
}

void FractalWidget::mouseMoveEvent(QMouseEvent *event)
{
	// Move root if dragging
	QPoint pos = event->pos();
	if (dragger_.mode == DraggingRoot && dragger_.index >= 0 && dragger_.index < rootPoints_.length()) {
		if (event->modifiers() == Qt::KeyboardModifier::ShiftModifier) {
			QPointF distance = pos - dragger_.previousPos;
			params_->roots[dragger_.index] += distance2complex(distance * MOD, *params_);
			dragger_.previousPos = pos;
		} else params_->roots[dragger_.index] = point2complex(pos, *params_);
		updateParams();
		emit rootMoved(dragger_.index, params_->roots[dragger_.index]);

	// Else move fractal if dragging
	} else if (dragger_.mode == DraggingFractal) {
		params_->limits.move(dragger_.previousPos - pos, params_->size);
		dragger_.previousPos = event->pos();
		updateParams();

	// Render orbit if enabled
	} else if (params_->orbitMode) {
		params_->orbitStart = pos;
		updateParams();

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
	// Reset dragging and render actual size
	Q_UNUSED(event);
	params_->scaleDown = false;
	params_->orbitMode = wasOrbit;
	dragger_.mode = NoDragging;
	dragger_.index = -1;
	updateParams();
}

void FractalWidget::resizeEvent(QResizeEvent *event)
{
	// Overwrite size with scaleSize for smooth resizing
	if (!timer_.isActive()) {
		params_->scaleDown = true;
		wasOrbit = params_->orbitMode;
		params_->orbitMode = false;
	}
	timer_.start();

	// Change resolution on resize
	params_->resize(event->size());
	updateParams();
	emit sizeChanged(event->size());
}

void FractalWidget::wheelEvent(QWheelEvent *event)
{
	// Calculate weight
	double xw = (double)event->pos().x() / width();
	double yw = (double)event->pos().y() / height();

	// Overwrite size with scaleSize for smooth moving
	if (!timer_.isActive()) {
		params_->scaleDown = true;
		wasOrbit = params_->orbitMode;
		params_->orbitMode = false;
	}
	timer_.start();

	// Zoom fractal in / out
	bool in = event->angleDelta().y() > 0;
	params_->limits.zoom(in, xw, yw);
	updateParams();
	emit zoomChanged(params_->limits.zoomFactor());
}
