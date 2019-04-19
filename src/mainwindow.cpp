#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui_(new Ui::MainWindow)
{
	// Initialize
	ui_->setupUi(this);
	ui_->settingsWidget->hide();
	addAction(ui_->actionSettings);

	// Connect signals and slots
	connect(ui_->actionSettings, &QAction::triggered, [this]() {
		ui_->settingsWidget->setVisible(ui_->settingsWidget->isHidden());
	});

	// Render with default parameters
	ui_->fractalWidget->render(Parameters());
}

MainWindow::~MainWindow()
{
	// Clean
	delete ui_;
}
