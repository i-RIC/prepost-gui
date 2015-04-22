#ifndef CGNSZONESELECTDIALOG_H
#define CGNSZONESELECTDIALOG_H

#include "guibase_global.h"
#include <QDialog>
#include <QList>
#include <QString>

namespace Ui {
	class CgnsZoneSelectDialog;
}

class GUIBASEDLL_EXPORT CgnsZoneSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CgnsZoneSelectDialog(QWidget *parent = nullptr);
	~CgnsZoneSelectDialog();
	void setZones(const QList<int>& zoneids, const QList<QString>& zonenames);
	int zoneId();
private:
	Ui::CgnsZoneSelectDialog *ui;
	QList<int> m_zoneIds;
};

#endif // CGNSZONESELECTDIALOG_H
