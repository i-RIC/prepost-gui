#include "qundocommandhelper.h"
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

namespace iRIC
{
	int generateCommandId(const QString& commandName)
	{
		QByteArray data = QCryptographicHash::hash(commandName.toLatin1(), QCryptographicHash::Md5);
		return data.left(4).toInt();
	}
}
