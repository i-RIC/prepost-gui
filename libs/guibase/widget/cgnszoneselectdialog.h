#ifndef CGNSZONESELECTDIALOG_H
#define CGNSZONESELECTDIALOG_H

#include "../guibase_global.h"
#include <QDialog>
#include <QList>
#include <QString>

namespace Ui
{
	class CgnsZoneSelectDialog;
}

/// Dialog to select a zone in a CGNS file.
class GUIBASEDLL_EXPORT CgnsZoneSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CgnsZoneSelectDialog(QWidget* parent = nullptr);
	~CgnsZoneSelectDialog();
	/**
	 * @brief Set the list of zones from which user should select one.
	 * @param zoneids the list of zone ids
	 * @param zonenames the list of zone names
	 */
	void setZones(const QList<int>& zoneids, const QList<QString>& zonenames);
	/// The selected zone ID. the value is one of the values in zoneids specified in setZones().
	int zoneId();

private:
	Ui::CgnsZoneSelectDialog* ui;
	QList<int> m_zoneIds;
};

#endif // CGNSZONESELECTDIALOG_H
