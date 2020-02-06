#include "qundocommandhelper.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QString>

namespace iRIC
{
	int generateCommandId(const QString& commandName)
	{
		QByteArray data = QCryptographicHash::hash(commandName.toLatin1(), QCryptographicHash::Md5);
		return data.left(4).toInt();
	}
}
