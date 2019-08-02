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
#include <QSettings>
#include <QPainter>
#include <QAction>
#include <QIcon>
#include <QDebug>

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

	// Add actions and connect signals
	QAction *quit = new QAction(this);
	QAction *hide = new QAction(this);
	QAction *orbit = new QAction(this);
	QAction *position = new QAction(this);
	QAction *settings = new QAction(this);
	quit->setShortcut(QKeySequence("Ctrl+Q"));
	hide->setShortcut(Qt::Key_Escape);
	orbit->setShortcut(Qt::Key_F2);
	position->setShortcut(Qt::Key_F3);
	settings->setShortcut(Qt::Key_F1);
	addActions(QList<QAction*>() << quit << hide << orbit << position << settings);
	connect(quit, &QAction::triggered, QApplication::instance(), &QCoreApplication::quit);
	connect(hide, &QAction::triggered, [this]() { legend_ = !legend_; update(); });
	connect(orbit, &QAction::triggered, [this]() { params_->orbitMode = !params_->orbitMode; updateParams(); });
	connect(position, &QAction::triggered, [this]() { position_ = !position_; update(); });
	connect(settings, &QAction::triggered, settingsWidget_, &SettingsWidget::toggle);

	// Initialize general stuff
	setMinimumSize(nf::MSI, nf::MSI);
	setMouseTracking(true);
	setWindowTitle(QApplication::applicationName());
	setWindowIcon(QIcon("://resources/icons/icon.png"));
	timer_.setInterval(nf::DTI);
	timer_.setSingleShot(true);
	resize(params_->size);
	settingsWidget_->hide();

	// Connect settingswidget signals
	connect(settingsWidget_, &SettingsWidget::paramsChanged, this, &FractalWidget::updateParams);
	connect(settingsWidget_, &SettingsWidget::sizeChanged, this, QOverload<const QSize &>::of(&FractalWidget::resize));
	connect(settingsWidget_, &SettingsWidget::exportImage, this, &FractalWidget::exportImage);
	connect(settingsWidget_, &SettingsWidget::exportRoots, this, &FractalWidget::exportRoots);
	connect(settingsWidget_, &SettingsWidget::importRoots, this, &FractalWidget::importRoots);
	connect(settingsWidget_, &SettingsWidget::reset, this, &FractalWidget::reset);
	connect(this, &FractalWidget::rootMoved, settingsWidget_, &SettingsWidget::moveRoot);
	connect(this, &FractalWidget::zoomChanged, settingsWidget_, &SettingsWidget::changeZoom);
	connect(this, &FractalWidget::sizeChanged, settingsWidget_, &SettingsWidget::changeSize);

	// Connect renderthread and timer signals
	connect(&renderThread_, &RenderThread::fractalRendered, this, &FractalWidget::updateFractal);
	connect(&renderThread_, &RenderThread::orbitRendered, this, &FractalWidget::updateOrbit);
	connect(&timer_, &QTimer::timeout, [this]() {
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

void FractalWidget::exportImage(const QString &dir)
{
	// Export fractal to file
	QString filePath = dir + "/fractal_" +
		QDateTime::currentDateTime().toString("yyMMdd_HHmmss_") +
		QString::number(params_->roots.count()) + "roots_" +
		QString::number(params_->size.width()) + "x" + QString::number(params_->size.width()) + ".png";
	QFile f(filePath);
	f.open(QIODevice::WriteOnly | QIODevice::Truncate);
	pixmap_.save(&f, "png");
}

void FractalWidget::exportRoots(const QString &dir)
{
	// Export roots to file
	quint8 rootCount = params_->roots.count();
	QString filePath = dir + "/fractal_" +
		QDateTime::currentDateTime().toString("yyMMdd_HHmmss_") +
		QString::number(rootCount) + "roots_" +
		QString::number(params_->size.width()) + "x" + QString::number(params_->size.width()) + ".ini";

	// Create ini file
	QSettings ini(filePath, QSettings::IniFormat, this);

	// General parameters
	ini.beginGroup("Parameters");
	ini.setValue("size", params_->size);
	ini.setValue("maxIterations", params_->maxIterations);
	ini.setValue("damping", complex2string(params_->damping));
	ini.setValue("scaleDownFactor", params_->scaleDownFactor);
	ini.setValue("scaleDown", params_->scaleDown);
	ini.setValue("processor", static_cast<quint8>(params_->processor));
	ini.setValue("orbitMode", params_->orbitMode);
	ini.setValue("orbitStart", params_->orbitStart);
	ini.endGroup();

	// Limits
	ini.beginGroup("Limits");
	ini.setValue("left", params_->limits.left());
	ini.setValue("right", params_->limits.right());
	ini.setValue("top", params_->limits.top());
	ini.setValue("bottom", params_->limits.bottom());
	ini.setValue("left_original", params_->limits.original()->left());
	ini.setValue("right_original", params_->limits.original()->right());
	ini.setValue("top_original", params_->limits.original()->top());
	ini.setValue("bottom_original", params_->limits.original()->bottom());
	ini.endGroup();

	// Roots
	ini.beginGroup("Roots");
	for (quint8 i = 0; i < rootCount; ++i) {
		ini.setValue(
			"root" + QString::number(i),
			complex2string(params_->roots[i].value(), 10) + " : " + params_->roots[i].color().name()
		);
	}
	ini.endGroup();
}

void FractalWidget::importRoots(const QString &file)
{
	// Open ini file
	QSettings ini(file, QSettings::IniFormat, this);

	// General parameters
	ini.beginGroup("Parameters");
	params_->size = ini.value("size", QSize(nf::DSI, nf::DSI)).toSize();
	params_->maxIterations = ini.value("maxIterations", nf::DMI).toUInt();
	params_->damping = string2complex(ini.value("damping", complex2string(nf::DDP)).toString());
	params_->scaleDownFactor = ini.value("scaleDownFactor", nf::DSC).toDouble();
	params_->scaleDown = ini.value("scaleDown", false).toBool();
	params_->processor = static_cast<Processor>(ini.value("processor", 1).toUInt());
	params_->orbitMode = ini.value("orbitMode", false).toBool();
	params_->orbitStart = ini.value("orbitStart").toPoint();
	ini.endGroup();


	// Limits
	ini.beginGroup("Limits");
	params_->limits.set(
		ini.value("left", 1).toDouble(), ini.value("right", 1).toDouble(),
		ini.value("top", 1).toDouble(), ini.value("bottom", 1).toDouble());
	params_->limits.original()->set(
		ini.value("left_original", 1).toDouble(), ini.value("right_original", 1).toDouble(),
		ini.value("top_original", 1).toDouble(), ini.value("bottom_original", 1).toDouble());
	ini.endGroup();

	// Update settings
	resize(params_->size);
	settingsWidget_->updateSettings();

	// Remove old roots
	quint8 rootCount = params_->roots.count();
	for (quint8 i = 0; i < rootCount; ++i) {
		settingsWidget_->removeRoot();
	}

	// Add Roots
	ini.beginGroup("Roots");
	for (QString key : ini.childKeys()) {
		QStringList str = ini.value(key).toString().split(":");
		if (str.length() >= 2) {
			settingsWidget_->addRoot(string2complex(str.first()), QColor(str.last().simplified()));
		}
	}
	ini.endGroup();

}

void FractalWidget::reset()
{
	// Reset roots to be equidistant
	params_->reset();
	updateParams();
	settingsWidget_->updateSettings();
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
	glEnable(GL_MULTISAMPLE);

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

	// Draw roots
	painter.setPen(circlePen);
	painter.setBrush(opaqueBrush);
	quint8 rootCount = params_->roots.count();
	for (quint8 i = 0; i < rootCount; ++i) {
		QPoint point = params_->complex2point(params_->roots[i].value());
		painter.drawEllipse(point, nf::RIR, nf::RIR);
	}

	// Draw legend if enabled
	if (legend_) {
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
	if (!timer_.isActive() && params_->processor != GPU_OPENGL)
		params_->scaleDown = true;

	// Restart timer
	timer_.start();

	// Change resolution on resize
	QSize newSize(w, h);
	params_->resize(newSize);
	glViewport(0, 0, w, h);
	updateParams();
	emit sizeChanged(newSize);
}

void FractalWidget::mousePressEvent(QMouseEvent *event)
{
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
	// Move root if dragging
	mousePosition = event->pos();
	if (dragger_.mode == DraggingRoot && dragger_.index >= 0 && dragger_.index < params_->roots.count()) {
		if (event->modifiers() == Qt::KeyboardModifier::ShiftModifier) {
			QPointF distance = mousePosition - dragger_.previousPos;
			params_->roots[dragger_.index] += params_->distance2complex(distance * nf::MOD);
			dragger_.previousPos = mousePosition;
		} else params_->roots[dragger_.index] = params_->point2complex(mousePosition);
		updateParams();
		emit rootMoved(dragger_.index, params_->roots[dragger_.index].value());

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
	// Reset dragging and render actual size
	Q_UNUSED(event);
	params_->scaleDown = false;
	dragger_.mode = NoDragging;
	dragger_.index = -1;
	updateParams();
}

void FractalWidget::wheelEvent(QWheelEvent *event)
{
	// Calculate weight
	double xw = (double)event->pos().x() / width();
	double yw = (double)event->pos().y() / height();

	// Overwrite size with scaleSize for smooth moving
	if (!timer_.isActive() && params_->processor != GPU_OPENGL)
		params_->scaleDown = true;

	// Restart timer
	timer_.start();

	// Zoom fractal in / out
	bool in = event->angleDelta().y() > 0;
	params_->limits.zoom(in, xw, yw);
	updateParams();
	emit zoomChanged(params_->limits.zoomFactor());
}
