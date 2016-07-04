#include "ui_iricmainwindowaboutdialog.h"
#include "iricmainwindowaboutdialog.h"

#include <guicore/misc/iricmetadata.h>
#include <misc/versionnumber.h>

#include <QPixmap>
#include <QUrl>

iRICMainWindowAboutDialog::iRICMainWindowAboutDialog(QWidget* parent) :
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

void iRICMainWindowAboutDialog::init(const iRICMetaData &data)
{
	ui->labelTitle->setText(tr("iRIC %1").arg(data.displayVersionNumber()));
	ui->labelRelease->setText(tr("Released on %1").arg(data.releaseDate().toString("yyyy/MM/dd")));
	ui->labelCopyright->setText(tr("Copyright %1").arg(data.copyright()));
	QString url = data.homeUrl();
	QString link;
	if (url != "") {
		QUrl u(url);
		link.append("<a href=\"").append(u.toString()).append("\">").append(url).append("</a>");
	}
	ui->labelUrl->setText(link);
	url = data.termsOfUseUrl();
	link = "";
	if (url != "") {
		QUrl u(url);
		link = tr("Terms of use: ");
		link.append("<a href=\"").append(u.toString()).append("\">").append(url).append("</a>");
		link.append(tr(" (Japanese)"));
	}
	ui->labelTOU->setText(link);
}
