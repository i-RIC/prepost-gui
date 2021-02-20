#ifndef TPOEXPORTER_H
#define TPOEXPORTER_H

#include "misc_global.h"

#include <QPointF>
#include <QTextStream>

class QFile;
class QString;
class QWidget;

class MISCDLL_EXPORT TpoExporter
{
public:
	TpoExporter(QWidget* w);
	~TpoExporter();

	bool open(const QString& fileName);

	void close();

	inline void addPoint(double x, double y, double z);

	void setOffset(double x, double y);
	void setOffset(const QPointF& offset);

private:
	QPointF m_offset;

	QFile* m_file;
	QTextStream* m_stream;
	QWidget* m_widget;
};

void TpoExporter::addPoint(double x, double y, double z)
{
	if (! m_stream) {return;}

	*(m_stream) << x + m_offset.x() << " ";
	*(m_stream) << y + m_offset.y() << " ";
	*(m_stream) << z << endl;
}

#endif // TPOEXPORTER_H
