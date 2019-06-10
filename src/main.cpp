#include "mainwindow.h"
#include <QApplication>

#ifdef QT_STATIC
#include <QtPlugin>
#if defined(__MINGW32__)
Q_IMPORT_PLUGIN (QWindowsIntegrationPlugin);
Q_IMPORT_PLUGIN(QWindowsVistaStylePlugin);
#endif
#endif

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	MainWindow w;
	w.show();
	return a.exec();
}

