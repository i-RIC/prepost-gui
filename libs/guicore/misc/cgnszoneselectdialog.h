#ifndef CGNSZONESELECTDIALOG_H
#define CGNSZONESELECTDIALOG_H

#include "../guicore_global.h"

#include <QDialog>
#include <QString>

#include <vector>

namespace Ui
{
	class CgnsZoneSelectDialog;
}

namespace iRICLib {
	class H5CgnsZone;
} // namespace


/// Dialog to select a zone in a CGNS file.
class GUICOREDLL_EXPORT CgnsZoneSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit CgnsZoneSelectDialog(QWidget* parent = nullptr);
	~CgnsZoneSelectDialog();

	void setMessage(const QString& message);
	void setZones(const std::vector<iRICLib::H5CgnsZone*>& zones);
	iRICLib::H5CgnsZone* selectedZone() const;

private:
	Ui::CgnsZoneSelectDialog* ui;
	std::vector<iRICLib::H5CgnsZone*> m_zones;
};

#endif // CGNSZONESELECTDIALOG_H
