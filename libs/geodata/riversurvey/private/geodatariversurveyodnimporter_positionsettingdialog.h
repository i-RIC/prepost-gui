#ifndef GEODATARIVERSURVEYODNIMPORTER_POSITIONSETTINGDIALOG_H
#define GEODATARIVERSURVEYODNIMPORTER_POSITIONSETTINGDIALOG_H

#include "../geodatariversurveyodnimporter.h"

#include <QDialog>

namespace Ui {
class GeoDataRiverSurveyOdnImporter_PositionSettingDialog;
}

class GeoDataRiverSurveyOdnImporter::PositionSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum class Position {Distance, KPName};

	explicit PositionSettingDialog(QWidget *parent = nullptr);
	~PositionSettingDialog();

	void disableDistance();
	Position position() const;

private:
	Ui::GeoDataRiverSurveyOdnImporter_PositionSettingDialog *ui;
};

#endif // GEODATARIVERSURVEYODNIMPORTER_POSITIONSETTINGDIALOG_H
