// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include "defaults.h"
#include <QWidget>
#include <QMouseEvent>

class RootEdit;
struct Parameters;

namespace Ui {
class SettingsWidget;
}

class SettingsWidget : public QWidget
{
	Q_OBJECT

public:
	SettingsWidget(Parameters *params, QWidget *parent = nullptr);
	~SettingsWidget();

public slots:
	void toggle();
	void changeSize(QSize size);
	void changeZoom(double factor);
	void moveRoot(quint8 index, complex value);

private slots:
	void on_settingsChanged();
	void on_btnExportClicked();

protected:
	// Don't pass any events to parent while settings are shown
	void mousePressEvent(QMouseEvent *event) override { event->accept(); }
	void mouseMoveEvent(QMouseEvent *event) override { event->accept(); }
	void mouseReleaseEvent(QMouseEvent *event) override { event->accept(); }
	void resizeEvent(QResizeEvent *event) override { event->accept(); }
	void wheelEvent(QWheelEvent *event) override { event->accept(); }

signals:
	void paramsChanged();
	void sizeChanged(QSize size);
	void exportTo(const QString &path);
	void reset();

private:
	Ui::SettingsWidget *ui_;
	Parameters *params_;
	QList<RootEdit*> rootEdits_;
};

#endif // SETTINGSWIDGET_H
