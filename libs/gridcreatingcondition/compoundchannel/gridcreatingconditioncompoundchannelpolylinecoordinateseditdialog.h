#ifndef GRICCREATINGCONDITIONCOMPOUNDCHANNELPOLYLINECOORDINATESEDITDIALOG_H
#define GRICCREATINGCONDITIONCOMPOUNDCHANNELPOLYLINECOORDINATESEDITDIALOG_H

#include <QDialog>
#include <QVector>
#include <QVector2D>

namespace Ui {
	class GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog;
}

class GridCreatingConditionCompoundChannel;
class GridCreatingConditionCompoundChannelAbstractLine;
class QAbstractButton;
class QStandardItemModel;

class GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog : public QDialog
{
	Q_OBJECT
private:
	static const int defaultRowHeight = 20;
public:
	explicit GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog(GridCreatingConditionCompoundChannelAbstractLine* l, GridCreatingConditionCompoundChannel* cond, QWidget *parent = nullptr);
	~GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog();
public slots:
	void accept();
	void reject();
private slots:
	void handleButtonClick(QAbstractButton* button);
private:
	QVector<QVector2D> getCoords();
	void setupData();
	void apply();

	Ui::GridCreatingConditionCompoundChannelPolyLineCoordinatesEditDialog *ui;
	GridCreatingConditionCompoundChannel* m_condition;
	GridCreatingConditionCompoundChannelAbstractLine* m_line;
	QStandardItemModel* m_model;
	bool m_applyed;
};

#endif // GRICCREATINGCONDITIONCOMPOUNDCHANNELPOLYLINECOORDINATESEDITDIALOG_H
