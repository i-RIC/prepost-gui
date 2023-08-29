#include "addconnectiondialog.h"
#include "addmodeldialog.h"
#include "iricmimainwindow.h"
#include "ui_iricmimainwindow.h"
#include "private/iricmimainwindow_modelsdelegate.h"

iRICMIMainWindow::iRICMIMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::iRICMIMainWindow)
{
	ui->setupUi(this);
	connect(ui->exitAction, &QAction::triggered, [=](bool){close();});
	connect(ui->addConnectionButton, &QPushButton::clicked, this, &iRICMIMainWindow::addConnection);
	connect(ui->addModelButton, &QPushButton::clicked, this, &iRICMIMainWindow::addModel);

	ui->modelsTableView->setModel(&m_modelsModel);
	ui->connectionsTableView->setModel(&m_connectionsModel);

	auto delegate = new ModelsDelegate(this);
	ui->modelsTableView->setItemDelegate(delegate);

	setupModelsModel();
	setupConnectionsModel();
}

iRICMIMainWindow::~iRICMIMainWindow()
{
	delete ui;
}

void iRICMIMainWindow::addConnection()
{
	AddConnectionDialog dialog(this);
	dialog.exec();
}

void iRICMIMainWindow::addModel()
{
	AddModelDialog dialog(this);
	dialog.exec();
}

void iRICMIMainWindow::setupModelsModel()
{
	m_modelsModel.setColumnCount(4);
	m_modelsModel.setRowCount(4);

	auto h1 = new QStandardItem(tr("Folder"));
	m_modelsModel.setHorizontalHeaderItem(0, h1);
	auto h2 = new QStandardItem(tr("Model"));
	m_modelsModel.setHorizontalHeaderItem(1, h2);
	auto h3 = new QStandardItem(tr("Description"));
	m_modelsModel.setHorizontalHeaderItem(2, h3);
	auto h4 = new QStandardItem(tr("Open"));
	m_modelsModel.setHorizontalHeaderItem(3, h4);

	auto m11 = new QStandardItem(tr("channel1"));
	m_modelsModel.setItem(0, 0, m11);
	auto m12 = new QStandardItem(tr("Nays1dp 1.0.8"));
	m_modelsModel.setItem(0, 1, m12);
	auto m13 = new QStandardItem(tr("River1 main channel"));
	m_modelsModel.setItem(0, 2, m13);

	auto m21 = new QStandardItem(tr("channel2"));
	m_modelsModel.setItem(1, 0, m21);
	auto m22 = new QStandardItem(tr("Nays1dp 1.0.8"));
	m_modelsModel.setItem(1, 1, m22);
	auto m23 = new QStandardItem(tr("River2 main channel"));
	m_modelsModel.setItem(1, 2, m23);

	auto m31 = new QStandardItem(tr("channel3"));
	m_modelsModel.setItem(2, 0, m31);
	auto m32 = new QStandardItem(tr("Nays1dp 1.0.8"));
	m_modelsModel.setItem(2, 1, m32);
	auto m33 = new QStandardItem(tr("River2 sub channel"));
	m_modelsModel.setItem(2, 2, m33);

	auto m41 = new QStandardItem(tr("channel2d"));
	m_modelsModel.setItem(3, 0, m41);
	auto m42 = new QStandardItem(tr("FaSTMECH 1.1.3"));
	m_modelsModel.setItem(3, 1, m42);
	auto m43 = new QStandardItem(tr("River2 two-dimensional solution"));
	m_modelsModel.setItem(3, 2, m43);

	ui->modelsTableView->setColumnWidth(2, 200);
}

void iRICMIMainWindow::setupConnectionsModel()
{
	m_connectionsModel.setColumnCount(4);
	m_connectionsModel.setRowCount(4);

	auto h1 = new QStandardItem(tr("Source model"));
	m_connectionsModel.setHorizontalHeaderItem(0, h1);
	auto h2 = new QStandardItem(tr("Source output name"));
	m_connectionsModel.setHorizontalHeaderItem(1, h2);
	auto h3 = new QStandardItem(tr("Target model"));
	m_connectionsModel.setHorizontalHeaderItem(2, h3);
	auto h4 = new QStandardItem(tr("Target input name"));
	m_connectionsModel.setHorizontalHeaderItem(3, h4);

	auto m11 = new QStandardItem(tr("channel1"));
	m_connectionsModel.setItem(0, 0, m11);
	auto m12 = new QStandardItem(tr("Downstream discharge"));
	m_connectionsModel.setItem(0, 1, m12);
	auto m13 = new QStandardItem(tr("channel2"));
	m_connectionsModel.setItem(0, 2, m13);
	auto m14 = new QStandardItem(tr("Upstream discharge"));
	m_connectionsModel.setItem(0, 3, m14);

	auto m21 = new QStandardItem(tr("channel2"));
	m_connectionsModel.setItem(1, 0, m21);
	auto m22 = new QStandardItem(tr("Upstream water surface elevation"));
	m_connectionsModel.setItem(1, 1, m22);
	auto m23 = new QStandardItem(tr("channel1"));
	m_connectionsModel.setItem(1, 2, m23);
	auto m24 = new QStandardItem(tr("Downstream water surface elevation"));
	m_connectionsModel.setItem(1, 3, m24);

	auto m31 = new QStandardItem(tr("channel2"));
	m_connectionsModel.setItem(2, 0, m31);
	auto m32 = new QStandardItem(tr("Downstream discharge"));
	m_connectionsModel.setItem(2, 1, m32);
	auto m33 = new QStandardItem(tr("channel3"));
	m_connectionsModel.setItem(2, 2, m33);
	auto m34 = new QStandardItem(tr("Upstream discharge"));
	m_connectionsModel.setItem(2, 3, m34);

	auto m41 = new QStandardItem(tr("channel3"));
	m_connectionsModel.setItem(3, 0, m41);
	auto m42 = new QStandardItem(tr("Upstream water surface elevation"));
	m_connectionsModel.setItem(3, 1, m42);
	auto m43 = new QStandardItem(tr("channel2"));
	m_connectionsModel.setItem(3, 2, m43);
	auto m44 = new QStandardItem(tr("Downstream water surface elevation"));
	m_connectionsModel.setItem(3, 3, m44);

	ui->connectionsTableView->setColumnWidth(1, 200);
	ui->connectionsTableView->setColumnWidth(3, 200);
}
