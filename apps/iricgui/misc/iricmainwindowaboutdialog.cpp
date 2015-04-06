#include "iricmainwindowaboutdialog.h"
#include "ui_iricmainwindowaboutdialog.h"

#include <misc/versionnumber.h>

#include <QDomElement>
#include <QDomNode>
#include <QPixmap>
#include <QUrl>

iRICMainWindowAboutDialog::iRICMainWindowAboutDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::iRICMainWindowAboutDialog)
{
	ui->setupUi(this);
	QPixmap logo(":/images/imgiRICLogo.png");
	ui->labelLogo->setPixmap(logo);
}

iRICMainWindowAboutDialog::~iRICMainWindowAboutDialog()
{
	delete ui;
}

void iRICMainWindowAboutDialog::init(const QDomNode& data)
{
	QDomElement elem = data.toElement();
	QString version = elem.attribute("version");
	VersionNumber vn;
	vn.fromString(version);
	version = vn.toAboutString();
	if (elem.hasAttribute("displayVersion")){
		version = elem.attribute("displayVersion");
	}
	ui->labelTitle->setText(tr("iRIC %1").arg(version));
	ui->labelRelease->setText(tr("Released on %1").arg(elem.attribute("release")));
	ui->labelCopyright->setText(tr("Copyright %1").arg(elem.attribute("copyright")));
	QString url = elem.attribute("url");
	QString link;
	if (url != ""){
		QUrl u(url);
		link.append("<a href=\"").append(u.toString()).append("\">").append(url).append("</a>");
	}
	ui->labelUrl->setText(link);
	url = elem.attribute("tou"); // Terms of use
	link = "";
	if (url != ""){
		QUrl u(url);
		link = tr("Terms of use: ");
		link.append("<a href=\"").append(u.toString()).append("\">").append(url).append("</a>");
		link.append(tr(" (Japanese)"));
	}
	ui->labelTOU->setText(link);
}
