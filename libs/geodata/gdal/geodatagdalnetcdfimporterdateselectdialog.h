#ifndef GEODATAGDALNETCDFIMPORTERDATESELECTDIALOG_H
#define GEODATAGDALNETCDFIMPORTERDATESELECTDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataGdalNetcdfImporterDateSelectDialog;
}

class GeoDataGdalNetcdfImporterDateSelectDialog : public QDialog
{
	Q_OBJECT

public:
	enum class TimeUnit {
		Years,
		Days,
		Hours,
		Minutes,
		Seconds,
	};

	explicit GeoDataGdalNetcdfImporterDateSelectDialog(QWidget* parent = nullptr);
	~GeoDataGdalNetcdfImporterDateSelectDialog();

	void setUnit(const QString& unit);
	void setOriginalDateTime(const QDateTime& orig);
	QDateTime originalDateTime() const;
	TimeUnit timeUnit() const;
	QTimeZone timeZone() const;
	static int timeUnitScale(TimeUnit unit);

private:
	Ui::GeoDataGdalNetcdfImporterDateSelectDialog* ui;
};

#endif // GEODATAGDALNETCDFIMPORTERDATESELECTDIALOG_H
