#ifndef STANDARDERRORMESSAGES_H
#define STANDARDERRORMESSAGES_H

#include "misc_global.h"

#include <QObject>

class MISCDLL_EXPORT StandardErrorMessages : public QObject
{
	Q_OBJECT

public:
	static QString fileOpenError(const QString& filename);

private:
	StandardErrorMessages();
};

#endif // STANDARDERRORMESSAGES_H
