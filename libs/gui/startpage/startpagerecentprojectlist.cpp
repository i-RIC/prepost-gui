#include "startpagerecentproject.h"
#include "startpagerecentprojectlist.h"

#include <QSettings>
#include <QVBoxLayout>

StartPageRecentProjectList::StartPageRecentProjectList(QWidget* parent) :
	QWidget(parent)
{
	m_layout = new QVBoxLayout(this);
	setLayout(m_layout);
	m_numberOfProjects = 0;
	setupItems();
}

void StartPageRecentProjectList::setupItems()
{
	QSettings setting;
	QStringList recentProjects = setting.value("general/recentprojects", QStringList()).toStringList();
	for (const QString& proj : recentProjects) {
		add(proj);
	}
	setup();
}

void StartPageRecentProjectList::add(const QString& projectFileName)
{
	if (m_numberOfProjects >= MAXPROJECTS) {return;}

	StartPageRecentProject* p = new StartPageRecentProject(projectFileName, this);
	connect(p, SIGNAL(clicked()), this, SLOT(handleProjectSelection()));
	m_layout->addWidget(p);
	++m_numberOfProjects;
}

void StartPageRecentProjectList::setup()
{
	m_layout->addStretch(1);
}

void StartPageRecentProjectList::handleProjectSelection()
{
	QObject* s = sender();
	StartPageRecentProject* proj = dynamic_cast<StartPageRecentProject*>(s);
	emit projectSelected(proj->filename());
}
