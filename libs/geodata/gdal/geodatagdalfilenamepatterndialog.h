#ifndef GEODATAGDALFILENAMEPATTERNDIALOG_H
#define GEODATAGDALFILENAMEPATTERNDIALOG_H

#include <QDialog>

namespace Ui {
class GeoDataGdalFileNamePatternDialog;
}

class GeoDataGdalFileNameMatcher;

class GeoDataGdalFileNamePatternDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataGdalFileNamePatternDialog(QWidget *parent = nullptr);
	~GeoDataGdalFileNamePatternDialog();

	void setFilename(const QString& filename);
	void setPattern(const QString& pattern);

	QTimeZone timeZone() const;
	void setTimeZone(const QTimeZone& timeZone);

	GeoDataGdalFileNameMatcher* matcher();

private slots:
	void updateMatcher();

private:
	QString m_filename;
	GeoDataGdalFileNameMatcher* m_matcher;

	Ui::GeoDataGdalFileNamePatternDialog *ui;
};

#endif // GEODATAGDALFILENAMEPATTERNDIALOG_H
