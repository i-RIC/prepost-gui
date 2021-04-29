#ifndef RECENTSOLVERSMANAGER_H
#define RECENTSOLVERSMANAGER_H

class QString;

class RecentSolversManager
{
public:
	static QString mostRecentSolver();

	static void append(const QString& folderName);
	static void remove(const QString& folderName);
	static void removeOldsIfTooMany();

private:
	RecentSolversManager();
};

#endif // RECENTSOLVERSMANAGER_H
