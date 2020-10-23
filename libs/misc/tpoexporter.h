#ifndef TPOEXPORTER_H
#define TPOEXPORTER_H

#include "misc_global.h"

class QPointF;
class QString;
class QWidget;

class MISCDLL_EXPORT TpoExporter
{
public:
	TpoExporter(QWidget* w);
	~TpoExporter();

	bool open(const QString& fileName);
	bool setup();

	void close();

	void addPoint(double x, double y, double z);

	void setOffset(double x, double y);
	void setOffset(const QPointF& offset);

private:
	class Impl;
	class SettingDialog;

	Impl* impl;
};

#endif // TPOEXPORTER_H
