#ifndef GEODATAPOINTMAPEDITPTSDIALOG_H
#define GEODATAPOINTMAPEDITPTSDIALOG_H

#include <QDialog>

class GeoDataPointmap;

namespace Ui
{
	class GeoDataPointmapEditPtsDialog;
}

class GeoDataPointmapEditPtsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapEditPtsDialog(GeoDataPointmap* pmap, QWidget* parent = nullptr);
	~GeoDataPointmapEditPtsDialog();
	double pointX();
	double pointY();
	double value();

private:
	Ui::GeoDataPointmapEditPtsDialog* ui;
	GeoDataPointmap* m_pointMap;
};

#endif // GEODATAPOINTMAPEDITPTSDIALOG_H
