// This file is part of the NewtonFractal project.
// Copyright (C) 2019 Christian Bauer and Timon Foehl
// License: GNU General Public License version 3 or later,
// see the file LICENSE in the main directory.

#include "fractalwidget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	// Register metatype
	qRegisterMetaType<QVector<QPoint>>("QVector<QPoint>");

	// Initialize application
	QApplication app(argc, argv);
	app.setOrganizationName("inf4");
	app.setOrganizationDomain("th-nuernberg.de");
	app.setApplicationName("NewtonFractal");
	app.setApplicationVersion(APP_VERSION);

	// Set format
	QSurfaceFormat fmt;
	fmt.setSamples(10);
	QSurfaceFormat::setDefaultFormat(fmt);

	// Create widget
	FractalWidget widget;
	widget.show();
	return app.exec();
}
