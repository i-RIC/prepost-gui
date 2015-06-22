#ifndef GEODATARIVERPATHPOINTRENAMEDIALOG_H
#define GEODATARIVERPATHPOINTRENAMEDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataRiverPathPointRenameDialog;
}

class GeoDataRiverSurvey;
class GeoDataRiverPathPoint;
class GeoDataRiverPathPointRenameDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverPathPointRenameDialog(GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs, QWidget* parent = nullptr);
	~GeoDataRiverPathPointRenameDialog();

public slots:
	void accept() override;

private:
	Ui::GeoDataRiverPathPointRenameDialog* ui;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
};

#endif // GEODATARIVERPATHPOINTRENAMEDIALOG_H
