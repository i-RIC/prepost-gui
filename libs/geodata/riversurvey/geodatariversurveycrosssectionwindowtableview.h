#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOWTABLEVIEW_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOWTABLEVIEW_H

#include <QTableView>

class GeoDataRiverSurveyCrosssectionWindowTableView : public QTableView
{
	Q_OBJECT

public:
	explicit GeoDataRiverSurveyCrosssectionWindowTableView(QWidget* parent = nullptr);

protected:
	void resizeEvent(QResizeEvent* event) override;
//	void mouseMoveEvent(QMouseEvent *event) override;

private:
	QModelIndex m_editIndex;
	const static int activeWidth = 50;
	const static int columnMargin = 0;
	QObject* m_editor;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOWTABLEVIEW_H
