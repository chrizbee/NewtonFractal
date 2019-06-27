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
#include <QUrl>

SettingsWidget::SettingsWidget(Parameters *params, QWidget *parent) :
	QWidget(parent),
	ui_(new Ui::SettingsWidget),
	params_(params)
{
	// Initialize ui
	ui_->setupUi(this);
	ui_->cbThreading->setEditable(true);
	ui_->cbThreading->lineEdit()->setReadOnly(true);
	ui_->cbThreading->lineEdit()->setAlignment(Qt::AlignCenter);
	ui_->lineSize->setValue(params_->size);
	ui_->spinScale->setValue(params_->scaleDownFactor * 100);
	ui_->spinIterations->setValue(params_->maxIterations);
	ui_->spinDegree->setValue(params_->roots.size());
	ui_->spinDamping->setValue(params_->damping);
	ui_->cbThreading->setCurrentIndex(params_->multiThreaded);
	ui_->spinZoom->setValue(params_->limits.zoomFactor() * 100);

	// Connect ui signals to slots
	connect(ui_->btnExport, &QPushButton::clicked, this, &SettingsWidget::on_btnExportClicked);
	connect(ui_->lineSize, &SizeEdit::sizeChanged, this, &SettingsWidget::sizeChanged);
	connect(ui_->btnReset, &QPushButton::clicked, this, &SettingsWidget::reset);
	connect(ui_->spinScale, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinIterations, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinDegree, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinDamping, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
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

void SettingsWidget::addRoot()
{
	// Create new root and apend it to parameters
	Root root;
	params_->roots.append(root);

	// Create RootEdit, add it to list and layout and connect its signal
	RootEdit *edit = new RootEdit(root.value(), this);
	rootEdits_.append(edit);
	ui_->gridRoots->addWidget(edit, params_->roots.size(), 1);
	connect(edit, &RootEdit::rootChanged, this, &SettingsWidget::on_settingsChanged);

	// Create label icon
	RootIcon *icon = new RootIcon(root.color(), this);
	rootIcons_.append(icon);
	ui_->gridRoots->addWidget(icon, params_->roots.size(), 0);
	connect(icon, &RootIcon::clicked, this, &SettingsWidget::changeRootColor);
}

void SettingsWidget::removeLastRoot()
{
	// Remove last rootedit and icon
	RootEdit *edit = rootEdits_.takeLast();
	RootIcon *icon = rootIcons_.takeLast();
	ui_->gridRoots->removeWidget(edit);
	ui_->gridRoots->removeWidget(icon);
	params_->roots.removeLast();
	delete edit;
	delete icon;
}

void SettingsWidget::moveRoot(quint8 index, complex value)
{
	// Update settings
	if (index < rootEdits_.size()) {
		rootEdits_[index]->setValue(value);
	}
}

void SettingsWidget::changeRootColor()
{
	// Change root color
	RootIcon *icon = static_cast<RootIcon*>(sender());
	if (icon) {
		QColor color = QColorDialog::getColor(Qt::red, this);
		if (color.isValid()) {
			icon->setColor(color);
			params_->roots[rootIcons_.indexOf(icon)].setColor(color);
		}
	}

	// Re-render
	emit paramsChanged();
}

void SettingsWidget::on_settingsChanged()
{
	// Add / remove roots
	int degree = ui_->spinDegree->value();
	while (params_->roots.size() < degree) { addRoot(); }
	while (params_->roots.size() > degree) { removeLastRoot(); }

	// Update fractal with new settings
	params_->limits.setZoomFactor(ui_->spinZoom->value() / 100.0);
	params_->maxIterations = ui_->spinIterations->value();
	params_->damping = ui_->spinDamping->value();
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

void SettingsWidget::on_btnExportClicked()
{
	// Export pixmap to file
	QSettings settings;
	QString exportDir = settings.value("exportdir", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)).toString();
	exportDir = QFileDialog::getExistingDirectory(this, tr("Export fractal to"), exportDir);
	if (!exportDir.isEmpty()) {
		settings.setValue("exportdir", exportDir);
		emit exportTo(exportDir);
	}
}
