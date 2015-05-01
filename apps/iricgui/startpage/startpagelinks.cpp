#include "startpagelink.h"
#include "startpagelinks.h"

#include <QDesktopServices>
#include <QDomDocument>
#include <QFile>
#include <QLabel>
#include <QUrl>
#include <QVBoxLayout>

StartPageLinks::StartPageLinks(QWidget* parent) :
	QWidget(parent)
{
	m_layout = nullptr;
}

void StartPageLinks::setupLinks(const QString& locale)
{
	m_layout = new QVBoxLayout(this);
	setLayout(m_layout);

	QString resource = QString(":/data/startpagelinks_%1.xml").arg(locale);
	if (! QFile::exists(resource)) {
		resource = ":/data/startpagelinks.xml";
	}
	QFile f(resource);
	QDomDocument doc;
	QString errorStr;
	int errorLine;
	int errorColumn;
	QString errorHeader = "Error occured while loading %1\n";
	doc.setContent(&f, &errorStr, &errorLine, &errorColumn);
	QDomElement element = doc.documentElement().toElement();

	QDomNodeList childNodes = element.childNodes();
	for (int i = 0; i < childNodes.count(); ++i) {
		QDomNode child = childNodes.at(i);
		QString groupTitle = child.toElement().attribute("title");
		addGroup(groupTitle);
		QDomNodeList linkNodes = child.toElement().childNodes();
		for (int j = 0; j < linkNodes.count(); ++j) {
			QDomElement linkElement = linkNodes.at(j).toElement();
			addLink(linkElement.attribute("title"), linkElement.attribute("url"));
		}
	}
	m_layout->addStretch(1);
}

void StartPageLinks::addGroup(const QString& title)
{
	QLabel* label = new QLabel(this);
	label->setFont(QFont("SanSerif", 14));
	label->setText(title);
	m_layout->addWidget(label);
}

void StartPageLinks::addLink(const QString& title, const QString& url)
{
	StartPageLink* link = new StartPageLink(title, url, this);
	connect(link, SIGNAL(clicked()), this, SLOT(handleLinkClick()));
	m_layout->addWidget(link);
}

void StartPageLinks::handleLinkClick()
{
	StartPageLink* link = dynamic_cast<StartPageLink*>(sender());
	QString url = link->url();
	QDesktopServices::openUrl(QUrl(url));
}
