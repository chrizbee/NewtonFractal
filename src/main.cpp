#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	// Initialize application
	QApplication app(argc, argv);
	app.setOrganizationName("inf4");
	app.setOrganizationDomain("th-nuernberg.de");
	app.setApplicationName("NewtonFractal");
	app.setApplicationVersion(APP_VERSION);
	app.setStyle("Fusion");
	app.setFont(QFont("Consolas", 10));

	// Create window
	MainWindow window;
	window.show();
	return app.exec();
}
