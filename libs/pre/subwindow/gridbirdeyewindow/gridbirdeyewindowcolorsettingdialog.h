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

	GridBirdEyeWindowDataModel::ColorType colorType() const;
	void setColorType(GridBirdEyeWindowDataModel::ColorType type);

	std::string attributeName() const;
	void setAttributeName(const std::string& name);

	QColor customColor() const;
	void setCustomColor(const QColor& c);

	bool axesVisible() const;
	void setAxesVisible(bool visible);

	QColor axesColor() const;
	void setAxisColor(const QColor& c);

private:
	QList<SolverDefinitionGridAttribute*> m_nodeConds;
	QList<SolverDefinitionGridAttribute*> m_cellConds;
	Ui::GridBirdEyeWindowColorSettingDialog* ui;
};

#endif // GRIDBIRDEYEWINDOWCOLORSETTINGDIALOG_H
