#ifndef GEODATARIVERSURVEY_CALCAREACONDITIONDIALOG_H
#define GEODATARIVERSURVEY_CALCAREACONDITIONDIALOG_H

#include "../geodatariversurvey.h"

#include <QDialog>

namespace Ui {
class GeoDataRiverSurvey_CalcAreaConditionDialog;
}

class GeoDataRiverSurvey::CalcAreaConditionDialog : public QDialog
{
	Q_OBJECT

public:
	enum Mode {Statistic, Each};

	explicit CalcAreaConditionDialog(QWidget *parent = nullptr);
	~CalcAreaConditionDialog();

	void setCompareTargets(const QStringList& targets);

	int compareTargetIndex() const;

	QString filename() const;
	void setFilename(const QString& name);

	Mode mode() const;

private:
	Ui::GeoDataRiverSurvey_CalcAreaConditionDialog *ui;
};

#endif // GEODATARIVERSURVEY_CALCAREACONDITIONDIALOG_H
