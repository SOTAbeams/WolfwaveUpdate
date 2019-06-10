#include <QFileDialog>
#include <QScrollBar>
#include <QThread>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "libFirmwareUpdate++/dfu.hpp"

#include <sstream>
#include <string>
#include <cmath>

Q_DECLARE_METATYPE(FwUpd::LogMsg)

MainWindow::MainWindow( QWidget *parent ) : QMainWindow( parent ), ui( new Ui::MainWindow )
{
	qRegisterMetaType<FwUpd::LogMsg>();
	Q_INIT_RESOURCE(wolfwave);

	ui->setupUi( this );
	dfuWorkerThread = new QThread();
	dfuWorker = new DfuWorker();
	dfuWorker->moveToThread(dfuWorkerThread);
	dfuWorkerThread->start();

	connect( ui->fileBrowseButton, SIGNAL( released() ), this, SLOT( browseFiles() ) );
	connect( ui->flashButton, SIGNAL( released() ), this, SLOT( dfuFlashBinary() ) );

	connect( dfuWorker, SIGNAL( finished(bool) ), this, SLOT( dfuFinished(bool) ) );
	connect( dfuWorker, SIGNAL( log(FwUpd::LogMsg) ), this, SLOT( dfuLog(FwUpd::LogMsg) ) );
	connect( dfuWorker, SIGNAL( progress(float, QString) ), this, SLOT( dfuProgress(float, QString) ) );
	connect( this, SIGNAL( startFlash(QString) ), dfuWorker, SLOT( run(QString) ) );

	resetLog();
	logInstructions();

	appIcon.addFile(":wolfwave-icon-256.png");
	appIcon.addFile(":wolfwave-icon-32.png");
	appIcon.addFile(":wolfwave-icon-16.png");
	setWindowIcon(appIcon);
}

MainWindow::~MainWindow()
{
	dfuWorkerThread->quit();
	dfuWorkerThread->deleteLater();
	dfuWorker->deleteLater();
	delete ui;
	Q_CLEANUP_RESOURCE(wolfwave);
}

void MainWindow::browseFiles()
{
	ui->fileBrowseLineEdit->setText(
		QFileDialog::getOpenFileName(
			this,
			"Select dfu binary",
			"",
			"DFU Binary ( *.dfu.bin *.bin *.dfu );;All Files ( * )"
		)
	);
}

void MainWindow::dfuFlashBinary()
{
	// Check if file exists
	QFile flashFile(ui->fileBrowseLineEdit->text());
	if ( !flashFile.exists() )
	{
		// Error if no file selected
		if (flashFile.fileName() == QString())
		{
			log("No file selected...");
		}
		// Error if it doesn't exist
		else
		{
			log(tr("'%1' does not exist...").arg(flashFile.fileName()));
		}

		logInstructions();
		return;
	}

	resetLog();
	log("Started update...");
	setControlsState(false);
	ui->progressBar->setValue(0);
	isDownloading = true;

	emit startFlash(flashFile.fileName());
}


void MainWindow::log(QString txt)
{
	ui->dfuResultsTextEdit->append(txt);
	// Scroll to bottom
	ui->dfuResultsTextEdit->verticalScrollBar()->setValue( ui->dfuResultsTextEdit->verticalScrollBar()->maximum() );
}

void MainWindow::resetLog()
{
	ui->dfuResultsTextEdit->clear();
}

void MainWindow::dfuFinished(bool success)
{
	isDownloading = false;
	setControlsState(true);
	ui->flashButton->setFocus(Qt::FocusReason::OtherFocusReason);

	if (success)
	{
		log("Firmware successfully updated");
	}
	else
	{
		log("Firmware update failed");
		ui->progressBar->setValue(0);
		logInstructions();
	}
}

void MainWindow::dfuLog(FwUpd::LogMsg msg)
{
	const char *prefix = nullptr;
	switch (msg.level)
	{
	default:
	case FwUpd::LogLevel::Verbose:
		break;
	case FwUpd::LogLevel::Info:
		prefix = "Info";
		break;
	case FwUpd::LogLevel::Warn:
		prefix = "Warning";
		break;
	case FwUpd::LogLevel::Error:
		prefix = "Error";
		break;
	}
	std::string txt = msg.txt;
	if (prefix)
		txt = std::string(prefix)+": "+txt;
	log(QString::fromStdString(txt));
}

void MainWindow::dfuProgress(float x, QString desc)
{
	ui->statusTxt->setText(desc);
	ui->progressBar->setValue(static_cast<int>(std::round(x*1000)));
}

void MainWindow::setControlsState(bool enabled)
{
	ui->flashButton->setEnabled(enabled);
	ui->fileBrowseButton->setEnabled(enabled);
	ui->fileBrowseLineEdit->setEnabled(enabled);
}

void MainWindow::logInstructions()
{
	log("\nSelect a firmware file by clicking the \"Browse\" button.\nThen, with the WOLFWAVE in firmware update mode, click \"Start Update\".\n\nTo put your WOLFWAVE into firmware update mode, disconnect it from USB and DC power. Then plug in the USB cable while holding down the firmware update button (located next to the USB socket). If the WOLFWAVE is in firmware update mode, then the screen will remain blank.\n\nTo get the latest firmware file, and for more detailed instructions, see http://wolfwave.co.uk/");
}


DfuWorker::DfuWorker()
{}

DfuWorker::~DfuWorker()
{}

void DfuWorker::run(QString filename)
{
	std::shared_ptr<FwUpd::Context> ctx = std::make_shared<FwUpd::Context>();
	std::shared_ptr<FwUpd::DfuFile> file = std::make_shared<FwUpd::DfuFile>(ctx);

	ctx->setProductName("WOLFWAVE");
	ctx->setMinLogLevel(FwUpd::LogLevel::Warn);
	ctx->setProgressHandler([this](float x, std::string desc){
		emit progress(x, QString::fromStdString(desc));
	});
	ctx->setLogHandler([this](const FwUpd::LogMsg &msg){
		emit log(msg);
	});

	try {
		file->loadFile(filename.toStdString());
	} catch (...) {
		FwUpd::LogMsg msg;
		msg.level = FwUpd::LogLevel::Error;
		msg.type = FwUpd::LogMsgType::FileIoError;
		msg.txt = "Failed to load firmware file";
		emit log(msg);
		emit finished(false);
	}

	FwUpd::DfuDownloader d(ctx, file);
	d.probe.match_usbId = FwUpd::UsbId(0x0483, 0xdf11);
	d.probe.match_usbId_dfu = d.probe.match_usbId;
	d.probe.match_iface_alt_index = 0;
	d.probe.match_iface_alt_name = "";
	d.dfuseOpts->leave = true;

	if (d.run())
	{
		emit finished(true);
	}
	else
	{
		emit finished(false);
	}
}
