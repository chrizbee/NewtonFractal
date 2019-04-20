#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("inf4");
	app.setOrganizationDomain("th-nuernberg.de");
	app.setApplicationName("NewtonFractal");
	app.setApplicationVersion(APP_VERSION);
	app.setStyle("Fusion");

	MainWindow window;
	window.show();
	return app.exec();
}
