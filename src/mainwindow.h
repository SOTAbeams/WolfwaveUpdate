#ifndef mainwindow_h
#define mainwindow_h

#include <QFile>
#include <QProcess>
#include <QMainWindow>


#include "libFirmwareUpdate++/dfu.hpp"
#include "libFirmwareUpdate++/LogMsg.hpp"

namespace Ui {
class MainWindow;
}

class DfuWorker : public QObject {
	Q_OBJECT
public:
	DfuWorker();
	~DfuWorker();
public slots:
	void run(QString filename);
signals:
	void finished(bool success);
	void log(FwUpd::LogMsg msg);
	void progress(float x, QString desc);
};


class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow( QWidget *parent = 0 );
	~MainWindow();

private slots:
	void browseFiles();
	void dfuFlashBinary();
	void log(QString txt);
	void resetLog();

	void dfuFinished(bool success);
	void dfuLog(FwUpd::LogMsg msg);
	void dfuProgress(float x, QString desc);

signals:
	void startFlash(QString filename);

private:
	void setControlsState(bool enabled);
	bool isDownloading = false;
	void logInstructions();

	Ui::MainWindow *ui;
	DfuWorker *dfuWorker;
	QThread *dfuWorkerThread;

	QProcess dfuUtilProcess;
	QString binaryPath;

	QIcon appIcon;

};

#endif
