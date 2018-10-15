#ifndef GEODATAPOINTMAPADDPOINTDIALOG_H
#define GEODATAPOINTMAPADDPOINTDIALOG_H

#include <QDialog>

#include <vector>

class GeoDataPointmap;

namespace Ui
{
	class GeoDataPointmapAddPointDialog;
}

class GeoDataPointmapAddPointDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapAddPointDialog(GeoDataPointmap* pmap, QWidget* parent = nullptr);
	~GeoDataPointmapAddPointDialog();
	std::vector<double> xpoint, ypoint, zpoint;

private:
	Ui::GeoDataPointmapAddPointDialog* ui;
	GeoDataPointmap* m_pmap;

private slots:
	void accept() override;

};

#endif // GEODATAPOINTMAPADDPOINTDIALOG_H
