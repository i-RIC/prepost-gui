#include "ui_iricauthdialog.h"

#include "iricauthclient.h"
#include "iricauthdialog.h"
#include "telemetrywidget.h"

iRICAuthDialog::iRICAuthDialog(iRICAuthClient* client, QWidget* parent) :
	QDialog(parent),
	m_widget {new TelemetryWidget(client, this)},
	ui {new Ui::iRICAuthDialog}
{
	ui->setupUi(this);
	ui->contentLayout->addWidget(m_widget);
}

iRICAuthDialog::~iRICAuthDialog()
{
	delete ui;
}

void iRICAuthDialog::accept()
{
	m_widget->applyToSettings();
	QDialog::accept();
}
