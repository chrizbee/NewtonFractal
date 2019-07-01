// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "fractalwidget.h"
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
#include <QDebug>

static bool updateParamsAllowed = true;

SettingsWidget::SettingsWidget(Parameters *params, QWidget *parent) :
	QWidget(parent),
	ui_(new Ui::SettingsWidget),
	params_(params),
	rootMenu_(new QMenu(this))
{
	// Initialize ui
	ui_->setupUi(this);
	ui_->cbThreading->setEditable(true);
	ui_->cbThreading->lineEdit()->setReadOnly(true);
	ui_->cbThreading->lineEdit()->setAlignment(Qt::AlignCenter);
	updateSettings();

	// Create menu with actions for roots
	// Kind of ugly flags, but needed for e.g. KDE's "Focus strictly under mouse"
	rootMenu_->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	rootMenu_->addAction(QIcon("://resources/icons/remove.png"), tr("Remove root"), rootMenu_, &QMenu::hide);
	rootMenu_->addAction(QIcon("://resources/icons/color.png"), tr("Change color"), rootMenu_, &QMenu::hide);
	rootMenu_->addAction(QIcon("://resources/icons/mirrorx.png"), tr("Mirror on x-axis"), rootMenu_, &QMenu::hide);
	rootMenu_->addAction(QIcon("://resources/icons/mirrory.png"), tr("Mirror on y-axis"), rootMenu_, &QMenu::hide);

	// Connect ui signals to slots
	connect(ui_->btnExportImage, &QPushButton::clicked, this, &SettingsWidget::on_btnExportImageClicked);
	connect(ui_->btnExportSettings, &QPushButton::clicked, this, &SettingsWidget::on_btnExportSettingsClicked);
	connect(ui_->btnImportSettings, &QPushButton::clicked, this, &SettingsWidget::on_btnImportSettingsClicked);
	connect(ui_->lineSize, &SizeEdit::sizeChanged, this, &SettingsWidget::sizeChanged);
	connect(ui_->btnReset, &QPushButton::clicked, this, &SettingsWidget::reset);
	connect(ui_->spinScale, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinIterations, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinDegree, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->lineDamping, &RootEdit::valueChanged, this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinZoom, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->cbThreading, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsWidget::on_settingsChanged);

	// Connect external links
	connect(ui_->btnOpit7, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://github.com/opit7"));});
	connect(ui_->btnChrizbee, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://github.com/chrizbee"));});
	connect(ui_->btnOhm, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://www.th-nuernberg.de/fakultaeten/efi"));});
	connect(ui_->btnIcons8, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://icons8.com"));});

	// Initialize roots
	ui_->spinDegree->setValue(DRC);
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
	ui_->lineSize->setValue(params_->size);
	ui_->spinScale->setValue(params_->scaleDownFactor * 100);
	ui_->spinZoom->setValue(params_->limits.zoomFactor() * 100);
	ui_->spinIterations->setValue(params_->maxIterations);
	ui_->spinDegree->setValue(params_->roots.size());
	ui_->lineDamping->setValue(params_->damping);
	ui_->cbThreading->setCurrentIndex(params_->multiThreaded);
	for (quint8 i = 0; i < params_->roots.size(); ++i) {
		moveRoot(i, params_->roots[i].value());
	}
	updateParamsAllowed = true;
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
	Root root(value);
	int rootCount = params_->roots.size();
	if (color != Qt::black)
		root.setColor(color);
	else if (params_->roots.size() < 6)
		root.setColor(colors[rootCount]);
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
	ui_->spinDegree->setValue(params_->roots.size());
}

void SettingsWidget::removeRoot(qint8 index)
{
	// Remove rootedit and icon
	quint8 rootCount = params_->roots.size();
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
	ui_->spinDegree->setValue(params_->roots.size());
}

void SettingsWidget::moveRoot(quint8 index, complex value)
{
	// Update settings
	if (index < rootEdits_.size()) {
		rootEdits_[index]->setValue(value);
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
		while (params_->roots.size() < degree) { addRoot(); }
		while (params_->roots.size() > degree) { removeRoot(); }

		// Update fractal with new settings
		params_->limits.setZoomFactor(ui_->spinZoom->value() / 100.0);
		params_->maxIterations = ui_->spinIterations->value();
		params_->damping = ui_->lineDamping->value();
		params_->scaleDownFactor = ui_->spinScale->value() / 100.0;
		params_->multiThreaded = ui_->cbThreading->currentIndex();

		// Update rootEdit value
		if (rootEdits_.size() == params_->roots.size()) {
			for (quint8 i = 0; i < rootEdits_.size(); ++i) {
				params_->roots[i].setValue(rootEdits_[i]->value());
			}
		}

		// Re-render
		emit paramsChanged();
	}
}

void SettingsWidget::on_btnExportImageClicked()
{
	// Export pixmap to file
	QSettings settings;
	QString dir = settings.value("imagedir", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)).toString();
	dir = QFileDialog::getExistingDirectory(this, tr("Export fractal to"), dir);
	if (!dir.isEmpty()) {
		settings.setValue("imagedir", dir);
		emit exportImage(dir);
	}
}

void SettingsWidget::on_btnExportSettingsClicked()
{
	// Export roots to file
	QSettings settings;
	QString dir = settings.value("settingsdir", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString();
	dir = QFileDialog::getExistingDirectory(this, tr("Export settings to"), dir);
	if (!dir.isEmpty()) {
		settings.setValue("settingsdir", dir);
		emit exportRoots(dir);
	}
}

void SettingsWidget::on_btnImportSettingsClicked()
{
	// Import roots from file
	QSettings settings;
	QString dir = settings.value("settingsdir", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString();
	QString file = QFileDialog::getOpenFileName(this, tr("Import settings"), dir, tr("Ini-File (*.ini)"));
	if (!file.isEmpty()) {
		settings.setValue("settingsdir", dir);
		emit importRoots(file);
	}
}
