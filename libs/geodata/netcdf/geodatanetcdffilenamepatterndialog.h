#ifndef GEODATANETCDFFILENAMEPATTERNDIALOG_H
#define GEODATANETCDFFILENAMEPATTERNDIALOG_H

#include <QDialog>

namespace Ui {
class GeoDataNetcdfFileNamePatternDialog;
}

class GeoDataNetcdfFileNameMatcher;

class GeoDataNetcdfFileNamePatternDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataNetcdfFileNamePatternDialog(QWidget *parent = nullptr);
	~GeoDataNetcdfFileNamePatternDialog();

	void setFilename(const QString& filename);

	GeoDataNetcdfFileNameMatcher* matcher();

private slots:
	void updateMatcher();

private:
	QString m_filename;
	GeoDataNetcdfFileNameMatcher* m_matcher;

	Ui::GeoDataNetcdfFileNamePatternDialog *ui;
};

#endif // GEODATANETCDFFILENAMEPATTERNDIALOG_H
