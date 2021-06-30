#include "recentsolversmanager.h"

#include <QSettings>

#define SETTING_PATH "general/recentsolvers"

namespace {

const int MAX_RECENT_SOLVERS = 10;

} // namespace

QString RecentSolversManager::mostRecentSolver()
{
	QSettings settings;
	auto recentSolvers = settings.value(SETTING_PATH, QStringList()).toStringList();
	if (recentSolvers.isEmpty()) {return "";}

	return recentSolvers.at(0);
}

void RecentSolversManager::append(const QString& folderName)
{
	QSettings setting;
	auto recentSolvers = setting.value(SETTING_PATH, QStringList()).toStringList();
	recentSolvers.removeAll(folderName);
	recentSolvers.prepend(folderName);
	setting.setValue(SETTING_PATH, recentSolvers);

	removeOldsIfTooMany();
}

void RecentSolversManager::remove(const QString& folderName)
{
	QSettings setting;
	auto recentSolvers = setting.value(SETTING_PATH, QStringList()).toStringList();
	recentSolvers.removeAll(folderName);
	setting.setValue(SETTING_PATH, recentSolvers);

	removeOldsIfTooMany();
}

void RecentSolversManager::removeOldsIfTooMany()
{
	QSettings setting;

	auto recentSolvers = setting.value(SETTING_PATH, QStringList()).toStringList();
	while (recentSolvers.size() > MAX_RECENT_SOLVERS) {
		recentSolvers.removeLast();
	}
	setting.setValue(SETTING_PATH, recentSolvers);
}
