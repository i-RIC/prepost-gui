#ifndef GEODATANETCDFIMPORTERDATESELECTDIALOG_H
#define GEODATANETCDFIMPORTERDATESELECTDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataNetcdfImporterDateSelectDialog;
}

class GeoDataNetcdfImporterDateSelectDialog : public QDialog
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

	explicit GeoDataNetcdfImporterDateSelectDialog(QWidget* parent = 0);
	~GeoDataNetcdfImporterDateSelectDialog();

	void setUnit(const QString& unit);
	void setOriginalDateTime(const QDateTime& orig);
	QDateTime originalDateTime() const;
	TimeUnit timeUnit() const;
	QTimeZone timeZone() const;
	static int timeUnitScale(TimeUnit unit);

private:
	Ui::GeoDataNetcdfImporterDateSelectDialog* ui;
};

#endif // GEODATANETCDFIMPORTERDATESELECTDIALOG_H
