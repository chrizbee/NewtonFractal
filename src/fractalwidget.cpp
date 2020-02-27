// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "fractalwidget.h"
#include "settingswidget.h"
#include "parameters.h"
#include <QStandardPaths>
#include <QApplication>
#include <QMessageBox>
#include <QFileDialog>
#include <QMouseEvent>
#include <QHBoxLayout>
#include <QShortcut>
#include <QSettings>
#include <QPainter>
#include <QAction>
#include <QIcon>

#define newSC(x) (new QShortcut(QKeySequence(x), this))

static QPoint mousePosition;
static const QVector<QVector3D> vertices = QVector<QVector3D>() <<
	QVector3D(-1, -1, 0) << QVector3D(1, -1, 0) << QVector3D(1, 1, 0) << QVector3D(-1, 1, 0);

Dragger::Dragger() :
	mode(NoDragging),
	index(-1)
{
}

FractalWidget::FractalWidget(QWidget *parent) :
	QOpenGLWidget(parent),
	enabled_(true),
	params_(new Parameters()),
	settingsWidget_(new SettingsWidget(params_, this)),
	fps_(0),
	legend_(true),
	position_(false)
{
	// Initialize layout
	QSpacerItem *spacer = new QSpacerItem(nf::DSI / 2.0, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QHBoxLayout *centralLayout = new QHBoxLayout(this);
	centralLayout->setSpacing(0);
	centralLayout->setContentsMargins(0, 0, 0, 0);
	centralLayout->addItem(spacer);
	centralLayout->addWidget(settingsWidget_);
	setLayout(centralLayout);

	// Initialize general stuff
	setMinimumSize(nf::MSI, nf::MSI);
	setMouseTracking(true);
	setWindowTitle(QApplication::applicationName());
	setWindowIcon(QIcon("://resources/icons/icon.png"));
	scaleDownTimer_.setInterval(nf::DTI);
	scaleDownTimer_.setSingleShot(true);
	resize(params_->size);
	settingsWidget_->hide();

	// Connect new shortcut signals
	connect(newSC("Ctrl+Q"), &QShortcut::activated, QApplication::instance(), &QCoreApplication::quit);
	connect(newSC(Qt::Key_Escape), &QShortcut::activated, [this]() { legend_ = !legend_; update(); });
	connect(newSC(Qt::Key_F2), &QShortcut::activated, [this]() { params_->orbitMode = !params_->orbitMode; updateParams(); });
	connect(newSC(Qt::Key_F3), &QShortcut::activated, [this]() { position_ = !position_; update(); });
	connect(newSC(Qt::Key_F1), &QShortcut::activated, settingsWidget_, &SettingsWidget::toggle);
	connect(newSC("Ctrl+R"), &QShortcut::activated, settingsWidget_, &SettingsWidget::reset);
	connect(newSC("Ctrl+S"), &QShortcut::activated, settingsWidget_, &SettingsWidget::exportImage);
	connect(newSC("Ctrl+E"), &QShortcut::activated, settingsWidget_, &SettingsWidget::exportSettings);
	connect(newSC("Ctrl+I"), &QShortcut::activated, settingsWidget_, &SettingsWidget::importSettings);

	// Connect settingswidget signals
	connect(settingsWidget_, &SettingsWidget::paramsChanged, this, &FractalWidget::updateParams);
	connect(settingsWidget_, &SettingsWidget::sizeChanged, this, QOverload<const QSize &>::of(&FractalWidget::resize));
	connect(settingsWidget_, &SettingsWidget::exportImageRequested, this, &FractalWidget::exportImageTo);
	connect(settingsWidget_, &SettingsWidget::reset, this, &FractalWidget::reset);

	// Connect renderthread and timer signals
	connect(&renderer_, &Renderer::orbitRendered, this, &FractalWidget::updateOrbit);
#ifndef WASM
	connect(&renderer_, &Renderer::fractalRendered, this, &FractalWidget::updateFractal);
	connect(&scaleDownTimer_, &QTimer::timeout, [this]() {
		params_->scaleDown = false;
		updateParams();
	});

	// Connect benchmark signals
	connect(settingsWidget_, &SettingsWidget::startBenchmarkRequested, this, &FractalWidget::runBenchmark);
	connect(settingsWidget_, &SettingsWidget::stopBenchmarkRequested, &renderer_, &Renderer::stop);
	connect(&renderer_, &Renderer::benchmarkFinished, this, &FractalWidget::finishBenchmark);
	connect(&renderer_, &Renderer::benchmarkProgress, settingsWidget_, &SettingsWidget::setBenchmarkProgress);
#endif

	// Initialize parameters
	reset();
}

FractalWidget::~FractalWidget()
{
	// Delete params only -> other pointers are being handled by Qt
	// TODO: Use QSharedPointer for params
	delete params_;
}

void FractalWidget::updateParams()
{
	// Pass params to renderthread by const reference
	if (enabled_) {
		renderer_.render(*params_);
	}
}

void FractalWidget::exportImageTo(const QString &dir)
{
	// Close settingsWidget
	bool closed = settingsWidget_->isHidden();
	settingsWidget_->setHidden(true);

	// Export fractal to file
	QFile f(dir + "/" + dynamicFileName(*params_, "png"));
	f.open(QIODevice::WriteOnly | QIODevice::Truncate);
	grab().save(&f, "png");

	// Reopen if needed
	settingsWidget_->setHidden(closed);
}

void FractalWidget::reset()
{
	// Reset roots to be equidistant
	params_->reset();
	updateParams();
	settingsWidget_->updateSettings();
}

void FractalWidget::runBenchmark()
{
	// Disable editing and set params
	enable(false);
	params_->benchmark = true;
	params_->scaleDown = false;
	settingsWidget_->toggleBenchmarking(true);

	// Run benchmark
	benchmarkTimer_.start();
	renderer_.render(*params_);
}

void FractalWidget::finishBenchmark(const QImage *image)
{
	// Static output string
	static const QString out = "Rendered %1 pixels in:\n%2 hr, %3 min, %4 sec and %5 ms";

	// Get time and number of pixels
	if (image != nullptr) {
		int pixels = image->width() * image->height();
		qint64 elapsed = benchmarkTimer_.elapsed();
		int s = elapsed / 1000;
		int ms = elapsed % 1000;
		int m = s / 60;
		s %= 60;
		int h = m / 60;
		m %= 60;

		// Show stats
		QMessageBox::StandardButton btn = QMessageBox::question(
			this, tr("Benchmark finished"),
			out.arg(pixels).arg(h).arg(m).arg(s).arg(ms),
			QMessageBox::Save | QMessageBox::Cancel);

		// Save image
		if (btn == QMessageBox::Save) {
			QSettings settings;
			QString dir = settings.value("imagedir", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)).toString();
			dir = QFileDialog::getExistingDirectory(this, tr("Export fractal to"), dir);
			if (!dir.isEmpty()) {
				settings.setValue("imagedir", dir);
				image->save(dir + "/" + dynamicFileName(*params_, "bmp"), "BMP", 100);
			}
		}
	}

	// Enable editing again and reset params
	settingsWidget_->toggleBenchmarking(false);
	enable(true);
	params_->benchmark = false;
	updateParams();
}

void FractalWidget::enable(bool value)
{
	// Toggle all actions
	enabled_ = value;
	QList<QShortcut*> childShortcuts = findChildren<QShortcut*>();
	for (QShortcut *sc : childShortcuts) {
		sc->setEnabled(value);
	}
}

void FractalWidget::updateFractal(const QPixmap &pixmap, double fps)
{
	// Update
	pixmap_ = pixmap;
	fps_ = fps;
	update();
}

void FractalWidget::updateOrbit(const QVector<QPoint> &orbit)
{
	// Update
	orbit_ = orbit;
	update();
}

void FractalWidget::initializeGL()
{
	// Check for support
	initializeOpenGLFunctions();
	if (glGetString(GL_VERSION) == 0) {
		settingsWidget_->disableOpenGL();
	}

	// Initialize OpenGL and shader program
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	program_ = new QOpenGLShaderProgram(this);
	program_->addShaderFromSourceFile(QOpenGLShader::Fragment, "://src/fractal.fsh");
	program_->addShaderFromSourceFile(QOpenGLShader::Vertex, "://src/vertex.vsh");
	program_->link();
	program_->bind();
	program_->setUniformValue("EPS", float(nf::EPS));
}

void FractalWidget::paintGL()
{
	// Static pens and brushes
	static const QPen circlePen(Qt::white, 2);
	static const QBrush opaqueBrush(QColor(0, 0, 0, 128));

	// Static pixmaps
	static const QPixmap pixHide("://resources/icons/hide.png");
	static const QPixmap pixSettings("://resources/icons/settings.png");
	static const QPixmap pixOrbit("://resources/icons/orbit.png");
	static const QPixmap pixPosition("://resources/icons/position.png");
	static const QPixmap pixFps("://resources/icons/fps.png");

	// Static legend geometry
	static const int spacing = 10;
	static const QFont consolas("Consolas", 12);
	static const QFontMetrics metrics(consolas);
#if QT_VERSION >= 0x050B00 // For any Qt Version >= 5.11.0 metrics.width() is deprecated
	static const int textWidth = 3 * spacing + pixFps.width() + metrics.horizontalAdvance("999.99");
#else
	static const int textWidth = 3 * spacing + pixFps.width() + metrics.width("999.99");
#endif
	static const int textHeight = spacing + 5 * (pixFps.height() + spacing);
	static const QRect legendRect(spacing, spacing, textWidth, textHeight);
	static const QPoint ptHide(legendRect.topLeft() + QPoint(spacing, spacing));
	static const QPoint ptSettings(ptHide + QPoint(0, pixHide.height() + spacing));
	static const QPoint ptOrbit(ptSettings + QPoint(0, pixSettings.height() + spacing));
	static const QPoint ptPosition(ptOrbit + QPoint(0, pixOrbit.height() + spacing));
	static const QPoint ptFps(ptPosition + QPoint(0, pixPosition.height() + spacing));

	// Paint fractal
	QPainter painter(this);
	painter.setFont(consolas);
	painter.setRenderHint(QPainter::Antialiasing);
	glEnable(0x809D); //GL_MULTISAMPLE_

	// Draw pixmap if rendered yet and cpu mode
	if (params_->processor != GPU_OPENGL && !pixmap_.isNull()) {
		painter.drawPixmap(rect(), pixmap_);
	} else {
		// Update params and draw
		quint8 rootCount = params_->roots.count();
		program_->bind();
		program_->enableAttributeArray(0);
		program_->setAttributeArray(0, vertices.constData());
		program_->setUniformValue("rootCount", rootCount);
		program_->setUniformValue("limits", params_->limits.vec4());
		program_->setUniformValue("maxIterations", params_->maxIterations);
		program_->setUniformValue("damping", complex2vec2(params_->damping));
		program_->setUniformValue("size", QVector2D(size().width(), size().height()));
		program_->setUniformValueArray("roots", params_->rootsVec2().constData(), rootCount);
		program_->setUniformValueArray("colors", params_->colorsVec3().constData(), rootCount);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	}


	// Circle pen / brush
	painter.setPen(circlePen);
	painter.setBrush(opaqueBrush);

	// Draw roots and legend if enabled
	if (legend_) {

		// Draw roots
		quint8 rootCount = params_->roots.count();
		for (quint8 i = 0; i < rootCount; ++i) {
			QPoint point = params_->complex2point(params_->roots[i].value());
			painter.drawEllipse(point, nf::RIR, nf::RIR);
		}

		// Draw legend
		painter.drawRoundedRect(legendRect, 10, 10);
		painter.drawPixmap(ptHide, pixHide);
		painter.drawPixmap(ptSettings, pixSettings);
		painter.drawPixmap(ptOrbit, pixOrbit);
		painter.drawPixmap(ptPosition, pixPosition);
		painter.drawPixmap(ptFps, pixFps);
		painter.drawText(ptHide + QPoint(pixHide.width() + spacing, metrics.height() - 4), "ESC");
		painter.drawText(ptSettings + QPoint(pixSettings.width() + spacing, metrics.height() - 4), "F1");
		painter.drawText(ptOrbit + QPoint(pixOrbit.width() + spacing, metrics.height() - 4), "F2");
		painter.drawText(ptPosition + QPoint(pixPosition.width() + spacing, metrics.height() - 4), "F3");
		painter.drawText(ptFps + QPoint(pixFps.width() + spacing, metrics.height() - 4), QString::number(fps_, 'f', 2));
	}

	// Draw position if enabled
	if (position_) {
		QString zstr = complex2string(params_->point2complex(mousePosition));
#if QT_VERSION >= 0x050B00 // For any Qt Version >= 5.11.0 metrics.width() is deprecated
		QRect posRect(mousePosition, QSize(metrics.horizontalAdvance(zstr) + spacing, metrics.height() + spacing));
#else
		QRect posRect(mousePosition, QSize(metrics.width(zstr) + spacing, metrics.height() + spacing));
#endif
		painter.drawRoundedRect(posRect, 6, 6);
		painter.drawText(posRect, Qt::AlignCenter, zstr);
	}

	// Draw orbit if enabled
	if (params_->orbitMode) {
		quint32 orbitSize = orbit_.size();
		if (orbitSize >= 1) {
			painter.drawEllipse(orbit_[0], nf::OIR, nf::OIR);
			for (quint32 i = 1; i < orbitSize; ++i) {
				painter.drawLine(orbit_[i - 1], orbit_[i]);
				painter.drawEllipse(orbit_[i], nf::OIR, nf::OIR);
			}
		}
	}
}

void FractalWidget::resizeGL(int w, int h)
{
	// Overwrite size with scaleSize for smooth resizing
	if (!scaleDownTimer_.isActive() && params_->processor != GPU_OPENGL)
		params_->scaleDown = true;

	// Restart timer
	scaleDownTimer_.start();

	// Change resolution on resize
	QSize newSize(w, h);
	params_->resize(newSize);
	glViewport(0, 0, w, h);
	settingsWidget_->changeSize(newSize);
	updateParams();
}

void FractalWidget::mousePressEvent(QMouseEvent *event)
{
	// Return if disabled
	if (!enabled_) return;

	// Set scaleDown, previousPos and orbit
	QPoint pos = event->pos();
	dragger_.previousPos = pos;
	if (params_->processor != GPU_OPENGL)
		params_->scaleDown = true;

	// Check if mouse press is on root
	int i = params_->rootContainsPoint(pos);
	if (i >= 0) {
		setCursor(Qt::ClosedHandCursor);
		dragger_.mode = DraggingRoot;
		dragger_.index = i;
		return;
	}

	// Else dragging fractal
	setCursor(Qt::SizeAllCursor);
	dragger_.mode = DraggingFractal;
}

void FractalWidget::mouseMoveEvent(QMouseEvent *event)
{
	// Return if disabled
	if (!enabled_) return;

	// Move root if dragging
	mousePosition = event->pos();
	if (dragger_.mode == DraggingRoot && dragger_.index >= 0 && dragger_.index < params_->roots.count()) {
		if (event->modifiers() == Qt::KeyboardModifier::ShiftModifier) {
			QPointF distance = mousePosition - dragger_.previousPos;
			params_->roots[dragger_.index] += params_->distance2complex(distance * nf::MOD);
			dragger_.previousPos = mousePosition;
		} else params_->roots[dragger_.index] = params_->point2complex(mousePosition);
		settingsWidget_->moveRoot(dragger_.index, params_->roots[dragger_.index].value());
		updateParams();

	// Else move fractal if dragging
	} else if (dragger_.mode == DraggingFractal) {
		params_->limits.move(dragger_.previousPos - mousePosition, params_->size);
		dragger_.previousPos = event->pos();
		updateParams();

	// Else if event over root change cursor
	} else {
		if (params_->rootContainsPoint(mousePosition) >= 0) {
			setCursor(Qt::OpenHandCursor);
		} else setCursor(Qt::ArrowCursor);
	}

	// Update position if enabled
	if (position_) {
		update();
	}

	// Render orbit if enabled
	if (params_->orbitMode) {
		params_->orbitStart = mousePosition;
		updateParams();
	}
}

void FractalWidget::mouseReleaseEvent(QMouseEvent *event)
{
	// Return if disabled
	if (!enabled_) return;

	// Reset dragging and render actual size
	Q_UNUSED(event);
	params_->scaleDown = false;
	dragger_.mode = NoDragging;
	dragger_.index = -1;
	updateParams();
}

void FractalWidget::wheelEvent(QWheelEvent *event)
{
	// Return if disabled
	if (!enabled_) return;

	// Calculate weight
	double xw = (double)event->pos().x() / width();
	double yw = (double)event->pos().y() / height();

	// Overwrite size with scaleSize for smooth moving
	if (!scaleDownTimer_.isActive() && params_->processor != GPU_OPENGL)
		params_->scaleDown = true;

	// Restart timer
	scaleDownTimer_.start();

	// Zoom fractal in / out
	bool in = event->angleDelta().y() > 0;
	params_->limits.zoom(in, xw, yw);
	settingsWidget_->changeZoom(params_->limits.zoomFactor());
	updateParams();
}
