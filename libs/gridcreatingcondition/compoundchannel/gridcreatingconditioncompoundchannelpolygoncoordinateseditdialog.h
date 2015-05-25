#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNELPOLYGONCOORDINATESEDITDIALOG_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNELPOLYGONCOORDINATESEDITDIALOG_H

#include <QDialog>
#include <QVector>
#include <QVector2D>

namespace Ui
{
	class GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog;
}

class GridCreatingConditionCompoundChannel;
class GridCreatingConditionCompoundChannelAbstractPolygon;
class QAbstractButton;
class QStandardItemModel;

class GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog : public QDialog
{
	Q_OBJECT
private:
	static const int defaultRowHeight = 20;
public:
	explicit GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog(GridCreatingConditionCompoundChannelAbstractPolygon* polygon, GridCreatingConditionCompoundChannel* cond, QWidget* parent = nullptr);
	~GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog();
public slots:
	void accept() override;
	void reject() override;
private slots:
	void handleButtonClick(QAbstractButton* button);
private:
	QVector<QVector2D> getCoords();
	void setupData();
	void apply();

	Ui::GridCreatingConditionCompoundChannelPolygonCoordinatesEditDialog* ui;
	GridCreatingConditionCompoundChannelAbstractPolygon* m_polygon;
	GridCreatingConditionCompoundChannel* m_condition;
	QStandardItemModel* m_model;
	bool m_applyed;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNELPOLYGONCOORDINATESEDITDIALOG_H
