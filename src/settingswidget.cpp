// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "settingswidget.h"
#include "ui_settingswidget.h"
#include "fractalwidget.h"
#include "parameters.h"
#include "rootedit.h"
#include <QDesktopServices>
#include <QStandardPaths>
#include <QFileDialog>
#include <QSettings>
#include <QUrl>

SettingsWidget::SettingsWidget(Parameters *params, QWidget *parent) :
	QWidget(parent),
	ui_(new Ui::SettingsWidget),
	params_(params)
{
	// Initialize
	ui_->setupUi(this);
	ui_->cbThreading->setEditable(true);
	ui_->cbThreading->lineEdit()->setReadOnly(true);
	ui_->cbThreading->lineEdit()->setAlignment(Qt::AlignCenter);
	quint8 degree = params_->roots.size();
	ui_->lineSize->setValue(params_->size);
	ui_->spinScale->setValue(params_->scaleDownFactor * 100);
	ui_->spinIterations->setValue(params_->maxIterations);
	ui_->spinDegree->setValue(degree);
	ui_->spinDamping->setValue(params_->damping);
	ui_->cbThreading->setCurrentIndex(params_->multiThreaded);
	ui_->spinZoom->setValue(params_->limits.zoomFactor() * 100);
	rootEdits_.append(QList<RootEdit*>() << ui_->lineRoot0 << ui_->lineRoot1 << ui_->lineRoot2 << ui_->lineRoot3 << ui_->lineRoot4 << ui_->lineRoot5);
	for (quint8 i = 0; i < rootEdits_.size(); ++i) {
		rootEdits_[i]->setEnabled(i < degree);
		if (i < degree) {
			rootEdits_[i]->setValue(params_->roots[i]);
		}
	}

	// Connect ui signals to slots
	connect(ui_->lineSize, &SizeEdit::sizeChanged, this, &SettingsWidget::sizeChanged);
	connect(ui_->btnReset, &QPushButton::clicked, this, &SettingsWidget::reset);
	connect(ui_->spinScale, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinIterations, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinDegree, QOverload<int>::of(&QSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinDamping, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->spinZoom, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &SettingsWidget::on_settingsChanged);
	connect(ui_->cbThreading, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsWidget::on_settingsChanged);
	for (RootEdit *rootEdit : rootEdits_) {
		connect(rootEdit, &RootEdit::rootChanged, this, &SettingsWidget::on_settingsChanged);
	}

	// Connect external links
	connect(ui_->btnOpit7, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://github.com/opit7"));});
	connect(ui_->btnChrizbee, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://github.com/chrizbee"));});
	connect(ui_->btnOhm, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://www.th-nuernberg.de/fakultaeten/efi"));});
	connect(ui_->btnIcons8, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://icons8.com"));});
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

void SettingsWidget::moveRoot(quint8 index, complex value)
{
	// Update settings
	if (index < rootEdits_.size()) {
		rootEdits_[index]->setValue(value);
	}
}

void SettingsWidget::on_settingsChanged()
{
	// Update fractal with new settings
	quint8 degree = ui_->spinDegree->value();
	params_->roots.clear();
	params_->limits.setZoomFactor(ui_->spinZoom->value() / 100.0);
	params_->maxIterations = ui_->spinIterations->value();
	params_->damping = ui_->spinDamping->value();
	params_->scaleDownFactor = ui_->spinScale->value() / 100.0;
	params_->multiThreaded = ui_->cbThreading->currentIndex();

	// Update rootEdit visibility
	for (quint8 i = 0; i < rootEdits_.size(); ++i) {
		rootEdits_[i]->setEnabled(i < degree);
		if (i < degree) {
			params_->roots.append(rootEdits_[i]->value());
		}
	}

	// Render
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
