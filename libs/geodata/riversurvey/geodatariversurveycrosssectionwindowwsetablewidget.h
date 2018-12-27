#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOWWSETABLEWIDGET_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOWWSETABLEWIDGET_H

#include <QTableWidget>

class QAction;
class QMenu;

class GeoDataRiverSurveyCrosssectionWindowWseTableWidget : public QTableWidget
{
	Q_OBJECT

public:
	GeoDataRiverSurveyCrosssectionWindowWseTableWidget(QWidget* parent = nullptr);

	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;

signals:
	void moveUpRequested(int index);
	void moveDownRequested(int index);
	void deleteRequested(int index);

private slots:
	void moveUpSelected();
	void moveDownSelected();
	void deleteSelected();

private:
	void setupMenu();

	QMenu* m_menu;
	QAction* m_moveUpAction;
	QAction* m_moveDownAction;
	QAction* m_deleteAction;

	QPoint m_pressPosition;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOWWSETABLEWIDGET_H
