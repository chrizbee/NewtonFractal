#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopServices>
#include <QStandardPaths>
#include <QFileDialog>
#include <QSettings>

static QList<QLineEdit*> rootEdits;

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
	rootEdits.append(QList<QLineEdit*>() << ui_->lineRoot0 << ui_->lineRoot1 << ui_->lineRoot2 << ui_->lineRoot3 << ui_->lineRoot4 << ui_->lineRoot5);
	addAction(ui_->actionSettings);
	resize(DSI, DSI);

	// Connect fractal signals and slots
	connect(ui_->fractalWidget, &FractalWidget::rootMoved, this, &MainWindow::on_rootMoved);
	connect(ui_->spinScale, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::on_settingsChanged);
	connect(ui_->spinIterations, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_settingsChanged);
	connect(ui_->spinDegree, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_settingsChanged);
	connect(ui_->spinZoom, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &MainWindow::on_settingsChanged);
	connect(ui_->cbThreading, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_settingsChanged);
	connect(ui_->btnZoom, &QPushButton::clicked, this, &MainWindow::on_settingsChanged);
	connect(ui_->btnExport, &QPushButton::clicked, this, &MainWindow::on_btnExportClicked);
	connect(ui_->btnReset, &QPushButton::clicked, ui_->fractalWidget, &FractalWidget::reset);
	connect(ui_->actionSettings, &QAction::triggered, [this]() {
		ui_->settingsScroll->setVisible(ui_->settingsScroll->isHidden());
	});
	for (QLineEdit *rootEdit : rootEdits) {
		connect(rootEdit, &QLineEdit::editingFinished, this, &MainWindow::on_settingsChanged);
	}

	// Connect external links
	connect(ui_->btnOpit7, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://github.com/opit7"));});
	connect(ui_->btnChrizbee, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://github.com/chrizbee"));});
	connect(ui_->btnOhm, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://www.th-nuernberg.de/fakultaeten/efi"));});
	connect(ui_->btnIcons8, &QPushButton::clicked, [this]() {QDesktopServices::openUrl(QUrl("https://icons8.com"));});

	// Start rendering with 3 default roots
	Parameters defaults = ui_->fractalWidget->params();
	ui_->spinScale->setValue(defaults.scaleDownFactor);
	ui_->spinIterations->setValue(defaults.maxIterations);
	ui_->spinDegree->setValue(defaults.roots.count());
	ui_->cbThreading->setCurrentIndex(defaults.multiThreaded);
	ui_->spinZoom->setValue(defaults.limits.zoomFactor);
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
	params.scaleDownFactor = ui_->spinScale->value();
	params.maxIterations = ui_->spinIterations->value();
	params.multiThreaded = ui_->cbThreading->currentIndex();
	params.zoomToCursor = ui_->btnZoom->isChecked();
	params.limits = ui_->fractalWidget->params().limits;
	params.size = ui_->fractalWidget->params().size;
	params.limits.zoomFactor = ui_->spinZoom->value();

	// Update rootEdit visibility
	for (quint8 i = 0; i < NRT; ++i) {
		rootEdits[i]->setEnabled(i < degree);
		if (i < degree) {
			params.roots.append(string2complex(rootEdits[i]->text()));
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
		rootEdits[index]->setText(complex2string(value));
	}
}
