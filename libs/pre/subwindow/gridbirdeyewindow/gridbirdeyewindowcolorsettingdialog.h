#ifndef GRIDBIRDEYEWINDOWCOLORSETTINGDIALOG_H
#define GRIDBIRDEYEWINDOWCOLORSETTINGDIALOG_H

#include <QDialog>
#include <QList>
#include "gridbirdeyewindowdatamodel.h"

namespace Ui
{
	class GridBirdEyeWindowColorSettingDialog;
}

class SolverDefinitionGridType;
class SolverDefinitionGridAttribute;

class GridBirdEyeWindowColorSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridBirdEyeWindowColorSettingDialog(QWidget* parent = nullptr);
	~GridBirdEyeWindowColorSettingDialog();
	void setGridType(SolverDefinitionGridType* type);
	void setColorType(GridBirdEyeWindowDataModel::ColorType type);
	void setAttributeName(const QString& name);
	void setCustomColor(const QColor& c);
	void setAxesVisible(bool visible);
	void setAxisColor(const QColor& c);
	bool axesVisible();
	QColor axesColor();

	GridBirdEyeWindowDataModel::ColorType colorType();
	const QString attributeName() const;
	const QColor customColor() const;

private:
	QList<SolverDefinitionGridAttribute*> m_nodeConds;
	QList<SolverDefinitionGridAttribute*> m_cellConds;
	Ui::GridBirdEyeWindowColorSettingDialog* ui;
};

#endif // GRIDBIRDEYEWINDOWCOLORSETTINGDIALOG_H
