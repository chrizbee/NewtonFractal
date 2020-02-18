// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "parameters.h"
#include "rootedit.h"
#include "rooticon.h"
#include <QDesktopServices>
#include <QStandardPaths>
#include <QColorDialog>
#include <QFileDialog>
#include <QSettings>
#include <QDateTime>
#include <QMenu>
#include <QUrl>

static bool updateParamsAllowed = true;

SettingsWidget::SettingsWidget(Parameters *params, QWidget *parent) :
	QWidget(parent),
	ui_(new Ui::SettingsWidget),
	params_(params),
	rootMenu_(new QMenu(this))
{
	// Initialize ui
	ui_->setupUi(this);
	ui_->progressBenchmark->setVisible(false);
	updateSettings();

	// Create menu with actions for roots
	// Kind of ugly flags, but needed for e.g. KDE's "Focus strictly under mouse"
	rootMenu_->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	rootMenu_->addAction(QIcon("://resources/icons/remove.png"), tr("Remove root"), rootMenu_, &QMenu::hide);
	rootMenu_->addAction(QIcon("://resources/icons/color.png"), tr("Change color"), rootMenu_, &QMenu::hide);
	rootMenu_->addAction(QIcon("://resources/icons/mirrorx.png"), tr("Mirror on x-axis"), rootMenu_, &QMenu::hide);
	rootMenu_->addAction(QIcon("://resources/icons/mirrory.png"), tr("Mirror on y-axis"), rootMenu_, &QMenu::hide);

	// Connect ui signals to slots
	connect(ui_->btnExportImage, &QPushButton::clicked, this, &SettingsWidget::exportImage);
	connect(ui_->btnExportSettings, &QPushButton::clicked, this, &SettingsWidget::exportSettings);
	connect(ui_->btnImportSettings, &QPushButton::clicked, this, &SettingsWidget::importSettings);
	connect(ui_->lineSize, &SizeEdit::sizeChanged, this, &SettingsWidget::sizeChanged);
	connect(ui_->btnReset, &QPushButton::clicked, this, &SettingsWidget::reset);
	connect(ui_->spinScaleDownFactor, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinIterations, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinDegree, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->lineDamping, &RootEdit::valueChanged, this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinZoom, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->cbThreading, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinScaleUpFactor, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->btnBenchmark, &QPushButton::clicked, this, &SettingsWidget::benchmarkRequested);

	// Connect external links
	connect(ui_->btnOpit7, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://github.com/opit7"));});
	connect(ui_->btnChrizbee, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://github.com/chrizbee"));});
	connect(ui_->btnOhm, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://www.th-nuernberg.de/fakultaeten/efi"));});
	connect(ui_->btnIcons8, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://icons8.com"));});

	// Initialize roots
	ui_->spinDegree->setValue(nf::DRC);
}

SettingsWidget::~SettingsWidget()
{
	// Clean
	delete ui_;
}

void SettingsWidget::updateSettings()
{
	updateParamsAllowed = false;

	// Update settings from params
	quint8 rootCount = params_->roots.count();
	ui_->lineSize->setValue(params_->size);
	ui_->spinScaleDownFactor->setValue(params_->scaleDownFactor * 100);
	ui_->spinZoom->setValue(params_->limits.zoomFactor() * 100);
	ui_->spinIterations->setValue(params_->maxIterations);
	ui_->spinDegree->setValue(rootCount);
	ui_->lineDamping->setValue(params_->damping);
	ui_->cbThreading->setCurrentIndex(static_cast<quint8>(params_->processor));
	for (quint8 i = 0; i < rootCount; ++i) {
		moveRoot(i, params_->roots[i].value());
	}
	updateParamsAllowed = true;
}

void SettingsWidget::disableOpenGL()
{
	// Disable combobox entry
	ui_->cbThreading->setItemData(2, 0, Qt::UserRole - 1);
}

void SettingsWidget::toggle()
{
	// Toggle visibility
	setVisible(isHidden());
}

void SettingsWidget::changeSize(QSize size)
{
	// Change sizeedit
	ui_->lineSize->setValue(size);
}

void SettingsWidget::changeZoom(double factor)
{
	// Update settings
	ui_->spinZoom->setValue(factor * 100);
}

void SettingsWidget::addRoot(complex value, QColor color)
{
	// Create new root and apend it to parameters
	int rootCount = params_->roots.count();
	if (rootCount < nf::MRC) {
		Root root(value, color == Qt::black ? nf::predefColors[rootCount] : color);
		params_->roots.append(root);

		// Create RootEdit, add it to list and layout and connect its signal
		RootEdit *edit = new RootEdit(this);
		edit->setValue(root.value());
		rootEdits_.append(edit);
		ui_->gridRoots->addWidget(edit, rootCount, 1);
		connect(edit, &RootEdit::valueChanged, this, &SettingsWidget::on_settingsChanged);

		// Create label icon
		RootIcon *icon = new RootIcon(root.color(), this);
		rootIcons_.append(icon);
		ui_->gridRoots->addWidget(icon, rootCount, 0);
		connect(icon, &RootIcon::clicked, this, &SettingsWidget::openRootContextMenu);

		// Update spinbox
		ui_->spinDegree->setValue(params_->roots.count());
	}
}

void SettingsWidget::removeRoot(qint8 index)
{
	// Remove rootedit and icon
	quint8 rootCount = params_->roots.count();
	index = index < 0 ? rootCount - 1 : index;
	RootEdit *edit = rootEdits_.takeAt(index);
	RootIcon *icon = rootIcons_.takeAt(index);
	ui_->gridRoots->removeWidget(edit);
	ui_->gridRoots->removeWidget(icon);
	params_->roots.removeAt(index);
	delete edit;
	delete icon;

	// Shift others up
	for (quint8 i = index; i < rootCount - 1; ++i) {
		ui_->gridRoots->removeWidget(rootEdits_[i]);
		ui_->gridRoots->removeWidget(rootIcons_[i]);
		ui_->gridRoots->addWidget(rootEdits_[i], i + 1, 1);
		ui_->gridRoots->addWidget(rootIcons_[i], i + 1, 0);
	}

	// Update spinbox
	ui_->spinDegree->setValue(params_->roots.count());
}

void SettingsWidget::moveRoot(quint8 index, complex value)
{
	// Update settings
	if (index < rootEdits_.size()) {
		rootEdits_[index]->setValue(value);
	}
}

void SettingsWidget::setBenchmarkProgress(int min, int max, int progress)
{
	// Set progress
	ui_->progressBenchmark->setValue(progress);
	ui_->progressBenchmark->setMinimum(min);
	ui_->progressBenchmark->setMaximum(max);
}

void SettingsWidget::showBenchmarkProgress(bool value)
{
	// Show / hide button and progress bar
	ui_->btnBenchmark->setVisible(!value);
	ui_->spinScaleUpFactor->setVisible(!value);
	ui_->progressBenchmark->setVisible(value);
}

void SettingsWidget::exportImage()
{
	// Export pixmap to file
	QSettings settings;
	QString dir = settings.value("imagedir", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)).toString();
	dir = QFileDialog::getExistingDirectory(this, tr("Export fractal to"), dir);
	if (!dir.isEmpty()) {
		settings.setValue("imagedir", dir);
		emit exportImageTo(dir);
	}
}

void SettingsWidget::exportSettings()
{
	// Export roots to file
	QSettings settings;
	QString dir = settings.value("settingsdir", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString();
	dir = QFileDialog::getExistingDirectory(this, tr("Export settings to"), dir);
	if (!dir.isEmpty()) {
		settings.setValue("settingsdir", dir);
		emit exportSettingsTo(dir);
	}
}

void SettingsWidget::importSettings()
{
	// Import roots from file
	QSettings settings;
	QString dir = settings.value("settingsdir", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString();
	QString file = QFileDialog::getOpenFileName(this, tr("Import settings"), dir, tr("Ini-File (*.ini)"));
	if (!file.isEmpty()) {
		settings.setValue("settingsdir", dir);
		emit importSettingsFrom(file);
	}
}

void SettingsWidget::openRootContextMenu()
{
	// Open root context menu
	RootIcon *icon = dynamic_cast<RootIcon*>(sender()); // meh
	if (icon != nullptr) {
		quint8 index = rootIcons_.indexOf(icon);
		QList<QAction*> actions = rootMenu_->actions();
		QAction *clicked = rootMenu_->exec(icon->mapToGlobal(QPoint(0, 0)), actions[0]);

		// Remove root
		if (clicked == actions[0]) {
			removeRoot(index);

		// Open color dialog
		} else if (clicked == actions[1]) {
			QColor color = QColorDialog::getColor(params_->roots[index].color(), this);
			if (color.isValid()) {
				icon->setColor(color);
				params_->roots[index].setColor(color);
			}

		// Mirror root on x-axis
		} else if (clicked == actions[2]) {
			complex root = params_->roots[index].value();
			root.imag(-root.imag());
			addRoot(root);

		// Mirror root on y-axis
		} else if (clicked == actions[3]) {
			complex root = params_->roots[index].value();
			root.real(-root.real());
			addRoot(root);
		}
	}

	// Re-render
	emit paramsChanged();
}

void SettingsWidget::on_settingsChanged()
{
	// Don't update if edited automatically
	if (updateParamsAllowed) {

		// Add / remove roots
		int degree = ui_->spinDegree->value();
		while (params_->roots.count() < degree) { addRoot(); }
		while (params_->roots.count() > degree) { removeRoot(); }

		// Update fractal with new settings
		params_->limits.setZoomFactor(ui_->spinZoom->value() / 100.0);
		params_->maxIterations = ui_->spinIterations->value();
		params_->damping = ui_->lineDamping->value();
		params_->scaleDownFactor = ui_->spinScaleDownFactor->value() / 100.0;
		params_->processor = static_cast<Processor>(ui_->cbThreading->currentIndex());
		params_->scaleUpFactor = ui_->spinScaleUpFactor->value();

		// Update rootEdit value
		if (rootEdits_.size() == params_->roots.count()) {
			for (quint8 i = 0; i < rootEdits_.size(); ++i) {
				params_->roots[i].setValue(rootEdits_[i]->value());
			}
		}

		// Re-render
		emit paramsChanged();
	}
}
