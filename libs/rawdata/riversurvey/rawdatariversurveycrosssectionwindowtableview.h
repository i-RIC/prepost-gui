#ifndef RAWDATARIVERSURVEYCROSSSECTIONWINDOWTABLEVIEW_H
#define RAWDATARIVERSURVEYCROSSSECTIONWINDOWTABLEVIEW_H

#include <QTableView>

class RawDataRiverSurveyCrosssectionWindowTableView : public QTableView
{
	Q_OBJECT
public:
	explicit RawDataRiverSurveyCrosssectionWindowTableView(QWidget* parent = nullptr);
protected:
	void resizeEvent(QResizeEvent* event);
//	void mouseMoveEvent(QMouseEvent *event);
private:
	QModelIndex m_editIndex;
	const static int activeWidth = 50;
	const static int columnMargin = 0;
	QObject* m_editor;
};

#endif // RAWDATARIVERSURVEYCROSSSECTIONWINDOWTABLEVIEW_H
