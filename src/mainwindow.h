// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "defaults.h"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_settingsChanged();
	void on_btnExportClicked();
	void on_rootMoved(quint8 index, complex value);

private:
	Ui::MainWindow *ui_;

};

#endif // MAINWINDOW_H
