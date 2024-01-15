#include "recentprojectsmanager.h"

#include <QDir>
#include <QMenu>
#include <QSettings>

#define SETTING_PATH "general/mi_recentprojects"

namespace {

const int MAX_RECENT_PROJECTS = 10;

}

void RecentProjectsManager::append(const QString& filename)
{
	QSettings setting;
	auto recentProjects = setting.value(SETTING_PATH, QStringList()).toStringList();
	recentProjects.removeAll(filename);
	recentProjects.prepend(filename);
	setting.setValue(SETTING_PATH, recentProjects);

	removeOldsIfTooMany();
}

void RecentProjectsManager::remove(const QString& filename)
{
	QSettings setting;
	auto recentProjects = setting.value(SETTING_PATH, QStringList()).toStringList();
	recentProjects.removeAll(filename);
	setting.setValue(SETTING_PATH, recentProjects);

	removeOldsIfTooMany();
}

void RecentProjectsManager::setupMenu(QMenu* menu)
{
	for (auto a : menu->actions()) {
		delete a;
	}
	menu->clear();
	QSettings setting;
	auto recentProjects = setting.value(SETTING_PATH, QStringList()).toStringList();
	for (int i = 0; i < recentProjects.size(); ++i) {
		auto text = QString("&%1 %2").arg(i + 1).arg(QDir::toNativeSeparators(recentProjects.at(i)));
		QAction* action = new QAction(text, menu);
		action->setData(recentProjects.at(i));
		menu->addAction(action);
	}
}

void RecentProjectsManager::removeOldsIfTooMany()
{
	QSettings setting;

	auto recentProjects = setting.value(SETTING_PATH, QStringList()).toStringList();
	while (recentProjects.size() > MAX_RECENT_PROJECTS) {
		recentProjects.removeLast();
	}
	setting.setValue(SETTING_PATH, recentProjects);
}
