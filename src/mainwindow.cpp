// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rootedit.h"
#include <QDesktopServices>
#include <QStandardPaths>
#include <QFileDialog>
#include <QSettings>

static QList<RootEdit*> rootEdits;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui_(new Ui::MainWindow)
{
	// Initialize
	ui_->setupUi(this);
	ui_->settingsScroll->hide();
	ui_->cbThreading->setEditable(true);
	ui_->cbThreading->lineEdit()->setReadOnly(true);
	ui_->cbThreading->lineEdit()->setAlignment(Qt::AlignCenter);
	rootEdits.append(QList<RootEdit*>() << ui_->lineRoot0 << ui_->lineRoot1 << ui_->lineRoot2 << ui_->lineRoot3 << ui_->lineRoot4 << ui_->lineRoot5);
	addAction(ui_->actionSettings);
	addAction(ui_->actionQuit);
	resize(DSI, DSI);

	// Connect fractal signals to slots
	connect(ui_->fractalWidget, &FractalWidget::rootMoved, this, &MainWindow::on_rootMoved);
	connect(ui_->spinScale, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_settingsChanged);
	connect(ui_->spinIterations, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_settingsChanged);
	connect(ui_->spinDegree, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_settingsChanged);
	connect(ui_->spinZoom, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_settingsChanged);
	connect(ui_->cbThreading, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_settingsChanged);
	connect(ui_->btnZoom, &QPushButton::clicked, this, &MainWindow::on_settingsChanged);
	connect(ui_->btnExport, &QPushButton::clicked, this, &MainWindow::on_btnExportClicked);
	connect(ui_->btnReset, &QPushButton::clicked, ui_->fractalWidget, &FractalWidget::reset);
	for (RootEdit *rootEdit : rootEdits) {
		connect(rootEdit, &RootEdit::rootChanged, this, &MainWindow::on_settingsChanged);
	}

	// Connect action signals to slots
	connect(ui_->actionQuit, &QAction::triggered, QApplication::instance(), &QCoreApplication::quit);
	connect(ui_->actionSettings, &QAction::triggered, [this]() {
		ui_->settingsScroll->setVisible(ui_->settingsScroll->isHidden());
	});

	// Connect external links
	connect(ui_->btnOpit7, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://github.com/opit7"));});
	connect(ui_->btnChrizbee, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://github.com/chrizbee"));});
	connect(ui_->btnOhm, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://www.th-nuernberg.de/fakultaeten/efi"));});
	connect(ui_->btnIcons8, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://icons8.com"));});

	// Set ui parameters to defaults
	Parameters defaults = ui_->fractalWidget->params();
	ui_->spinScale->setValue(defaults.scaleDownFactor * 100);
	ui_->spinIterations->setValue(defaults.maxIterations);
	ui_->spinDegree->setValue(defaults.roots.size());
	ui_->cbThreading->setCurrentIndex(defaults.multiThreaded);
	ui_->spinZoom->setValue(defaults.limits.zoomFactor() * 100);
	ui_->btnZoom->setChecked(defaults.zoomToCursor);
	ui_->fractalWidget->updateParams(defaults);
	ui_->fractalWidget->reset();
}

MainWindow::~MainWindow()
{
	// Clean
	delete ui_;
}

void MainWindow::on_settingsChanged()
{
	// Update fractal with new settings
	Parameters params;
	quint8 degree = ui_->spinDegree->value();
	params.roots.clear();
	params.scaleDownFactor = ui_->spinScale->value() / 100.0;
	params.maxIterations = ui_->spinIterations->value();
	params.multiThreaded = ui_->cbThreading->currentIndex();
	params.zoomToCursor = ui_->btnZoom->isChecked();
	params.limits = ui_->fractalWidget->params().limits;
	params.size = ui_->fractalWidget->params().size;
	params.limits.setZoomFactor(ui_->spinZoom->value() / 100.0);

	// Update rootEdit visibility
	for (quint8 i = 0; i < NRT; ++i) {
		rootEdits[i]->setEnabled(i < degree);
		if (i < degree) {
			params.roots.append(rootEdits[i]->root());
		}
	}

	// Render
	ui_->fractalWidget->updateParams(params);
}

void MainWindow::on_btnExportClicked()
{
	// Export pixmap to file
	QSettings settings;
	QString exportDir = settings.value("exportdir", QStandardPaths::standardLocations(QStandardPaths::PicturesLocation)).toString();
	exportDir = QFileDialog::getExistingDirectory(this, tr("Export fractal to"), exportDir);
	if (!exportDir.isEmpty()) {
		settings.setValue("exportdir", exportDir);
		ui_->fractalWidget->exportTo(exportDir);
	}
}

void MainWindow::on_rootMoved(quint8 index, complex value)
{
	// Update settings
	if (index < NRT) {
		rootEdits[index]->setRoot(value);
	}
}
