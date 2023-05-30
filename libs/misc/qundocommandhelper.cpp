#include "qundocommandhelper.h"

#include <QByteArray>
#include <QCryptographicHash>
#include <QString>

namespace iRIC
{
	int generateCommandId(const QString& commandName)
	{
		QByteArray data = QCryptographicHash::hash(commandName.toLatin1(), QCryptographicHash::Md5);
		auto v = data.left(4).toInt();
		if (v == 0) {v = 1;}

		return v;
	}
}
