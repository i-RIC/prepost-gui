#include "stringtool.h"

#include <QColor>
#include <QString>
#include <QTextCodec>

#include <string>

#define CSVPARSER_STATUS_AFTERITEM    0
#define CSVPARSER_STATUS_INIT         1
#define CSVPARSER_STATUS_STR          2
#define CSVPARSER_STATUS_INNERQUOT    3

namespace iRIC
{
	std::string toStr(const QString& str)
	{
		std::string ret = str.toLocal8Bit().constData();
		return ret;
	}

	bool isAscii(const QString& str)
	{
		QTextCodec* asciiCodec = QTextCodec::codecForName("latin1");
		return asciiCodec->canEncode(str);
	}

	QColor QColorFromString(QString qstring)
	{
		int r, g, b;
		bool rOK, gOK, bOK;

		r = qstring.mid(1, 2).toInt(&rOK, 16);
		g = qstring.mid(3, 2).toInt(&gOK, 16);
		b = qstring.mid(5, 2).toInt(&bOK, 16);

		QColor color(0, 0, 0);
		if (rOK && gOK && bOK) {
			color.setRgb(r, g, b);
		}

		return color;
	}

	QString QColorToString(QColor qcolor)
	{
		QString r, g, b;
		r.setNum(qcolor.red() + 256, 16);
		g.setNum(qcolor.green() + 256, 16);
		b.setNum(qcolor.blue() + 256, 16);

		r = r.mid(1, 2);
		g = g.mid(1, 2);
		b = b.mid(1, 2);

		QString qstr = QString("#").append(r).append(g).append(b);
		return qstr;
	}
	QList<QString> parseCSVLine(const QString& line)
	{
		int index = 0;
		bool wrapped = false;
		QList<QString> ret;
		QString current;
		while (index < line.count()) {
			QChar c = line.at(index);
			if (c == '\r' || c == '\n') {continue;}
			if (wrapped) {
				if (c == '"') {
					wrapped = false;
				} else {
					current.append(c);
				}
			} else {
				if (c == ',') {
					ret.append(current);
					current = "";
				} else if (c == '"') {
					wrapped = true;
				} else {
					current.append(c);
				}
			}
			++ index;
		}
		if (! current.isEmpty()) {
			ret.append(current);
		}
		return ret;
	}
}
