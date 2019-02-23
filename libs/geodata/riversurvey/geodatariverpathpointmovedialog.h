#ifndef GEODATARIVERPATHPOINTMOVEDIALOG_H
#define GEODATARIVERPATHPOINTMOVEDIALOG_H

#include <QDialog>
#include <QVector2D>

class GeoDataRiverSurvey;
class QAbstractButton;

namespace Ui
{
	class GeoDataRiverPathPointMoveDialog;
}

class GeoDataRiverPathPointMoveDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverPathPointMoveDialog(GeoDataRiverSurvey* rs, QWidget* parent = nullptr);
	~GeoDataRiverPathPointMoveDialog();
	void setSingleSelection(bool single);
	void setCurrentCenter(const QPointF& current);

public slots:
	void accept() override;
	void reject() override;

private slots:
	void offsetChange();
	void centerChange();
	void handleButtonClick(QAbstractButton* button);

private:
	void doReset();
	void apply();

	Ui::GeoDataRiverPathPointMoveDialog* ui;
	QPointF m_currentCenter;
	GeoDataRiverSurvey* m_rs;
	bool m_applyed;
};

#endif // GEODATARIVERPATHPOINTMOVEDIALOG_H
