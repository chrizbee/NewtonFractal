#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "parameters.h"

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
	void on_rootMoved(quint8 index, complex value);

private:
	Ui::MainWindow *ui_;

};

#endif // MAINWINDOW_H
