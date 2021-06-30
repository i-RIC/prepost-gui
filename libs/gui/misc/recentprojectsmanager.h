#ifndef RECENTPROJECTSMANAGER_H
#define RECENTPROJECTSMANAGER_H

class QMenu;
class QString;

class RecentProjectsManager
{
public:
	static void append(const QString& filename);
	static void remove(const QString& filename);
	static void setupMenu(QMenu* menu);
	static void removeOldsIfTooMany();

private:
	RecentProjectsManager();
};

#endif // RECENTPROJECTSMANAGER_H
