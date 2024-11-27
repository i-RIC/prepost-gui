#ifndef GEODATAGDALIMPORTERDATESELECTDIALOG_H
#define GEODATAGDALIMPORTERDATESELECTDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataGdalImporterDateSelectDialog;
}

class GeoDataGdalImporterDateSelectDialog : public QDialog
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

	explicit GeoDataGdalImporterDateSelectDialog(QWidget* parent = 0);
	~GeoDataGdalImporterDateSelectDialog();

	void setUnit(const QString& unit);
	void setOriginalDateTime(const QDateTime& orig);
	QDateTime originalDateTime() const;
	TimeUnit timeUnit() const;
	QTimeZone timeZone() const;
	static int timeUnitScale(TimeUnit unit);

private:
	Ui::GeoDataGdalImporterDateSelectDialog* ui;
};

#endif // GEODATAGDALIMPORTERDATESELECTDIALOG_H
