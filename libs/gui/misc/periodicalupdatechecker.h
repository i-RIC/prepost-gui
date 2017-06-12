#ifndef PERIODICALUPDATECHECKER_H
#define PERIODICALUPDATECHECKER_H

#include "../gui_api.h"

#include <QObject>

class GUI_API PeriodicalUpdateChecker : public QObject
{
	Q_OBJECT

public:
	const static int DEFAULT_INTERVAL_DAYS;

	static bool checkUpdate();

private:
	PeriodicalUpdateChecker();
};

#endif // PERIODICALUPDATECHECKER_H
