#ifndef GRIDCREATINGCONDITIONGRIDCOMBINESETTINGDIALOG_H
#define GRIDCREATINGCONDITIONGRIDCOMBINESETTINGDIALOG_H

#include <QDialog>
#include <QMap>

class Grid;
class PreProcessorGridCreatingConditionDataItemInterface;

namespace Ui
{
	class GridCreatingConditionGridCombineSettingDialog;
}

class GridCreatingConditionGridCombineSettingDialog : public QDialog
{
	Q_OBJECT
public:
	enum Type {Bind, Left, Right};
	GridCreatingConditionGridCombineSettingDialog(QWidget* parent = nullptr);
	~GridCreatingConditionGridCombineSettingDialog();

	void setType(Type type);
	void setMainstreamGridName(const QString& name);
	void setTributaryGridName(const QString& name);
	void setMainstreamCrossingBeginningIndex(int i);
	void setMainstreamCrossingEndingIndex(int i);
	void setTributaryCrossingBeginningIndex(int i);
	void setTributaryCrossingEndingIndex(int i);
	void setMainstreamFlowBeginningIndex(int i);
	void setMainstreamFlowEndingIndex(int i);
	void setObstacleUpstreamX(double x);
	void setObstacleUpstreamY(double y);
	void setObstacleDownstreamX(double x);
	void setObstacleDownstreamY(double y);
	void setCorrectionValue(double c);
	void setObstacleElevation(double e);

	Type type();
	Grid* mainstreamGrid();
	Grid* tributaryGrid();
	QString mainstreamGridName();
	QString tributaryGridName();
	int mainstreamCrossingBeginningIndex();
	int mainstreamCrossingEndingIndex();
	int tributaryCrossingBeginningIndex();
	int tributaryCrossingEndingIndex();
	int mainstreamFlowBeginningIndex();
	int mainstreamFlowEndingIndex();
	double obstacleUpstreamX();
	double obstacleUpstreamY();
	double obstacleDownstreamX();
	double obstacleDownstreamY();
	double correctionValue();
	double obstacleElevation();

	void setupComboBox(PreProcessorGridCreatingConditionDataItemInterface* item);
	void setupConnections();
	void setupDisability();
public slots:
	void informConfluenceTypeChange(bool b);
	void informGridChange(int i);
	void setupGridIndex();
signals:
	void gridInformationChanged();
private slots:
	void changeFlowEndingIndex(int index);
private:
	Ui::GridCreatingConditionGridCombineSettingDialog* ui;
	QMap<QString, Grid*> m_gridMap;
	int offset;
};

#endif // GRIDCREATINGCONDITIONGRIDCOMBINESETTINGDIALOG_H
