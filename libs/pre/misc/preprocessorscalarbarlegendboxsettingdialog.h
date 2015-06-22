#ifndef PREPROCESSORSCALARBARLEGENDBOXSETTINGDIALOG_H
#define PREPROCESSORSCALARBARLEGENDBOXSETTINGDIALOG_H

#include <guibase/vtktextpropertysettingcontainer.h>
#include <QDialog>
#include <QMap>

class PreProcessorGeoDataTopDataItemInterface;
class PreProcessorGeoDataGroupDataItemInterface;
class SolverDefinitionGridAttribute;

namespace Ui
{
	class PreProcessorScalarBarLegendBoxSettingDialog;
}

class PreProcessorScalarBarLegendBoxSettingDialog : public QDialog
{
	Q_OBJECT

public:
	PreProcessorScalarBarLegendBoxSettingDialog(QWidget* parent = nullptr);
	~PreProcessorScalarBarLegendBoxSettingDialog();

	void setupComboBox(PreProcessorGeoDataTopDataItemInterface* item);

	void setActor2DVisibility(bool vis);
	void setCondition(SolverDefinitionGridAttribute* condition);
	void setTitleTextSetting(const vtkTextPropertySettingContainer& cont);
	void setLabelTextSetting(const vtkTextPropertySettingContainer& cont);
	bool actor2DVisibility();
	SolverDefinitionGridAttribute* condition();
	const vtkTextPropertySettingContainer& titleTextSetting() const {return m_titleTextSetting;}
	const vtkTextPropertySettingContainer& labelTextSetting() const {return m_labelTextSetting;}

public slots:
	void setDisplay(bool disp);
	void editScalarBar();

private:
	QMap<QString, PreProcessorGeoDataGroupDataItemInterface*> m_itemMap;
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;

	Ui::PreProcessorScalarBarLegendBoxSettingDialog* ui;
};

#endif // PREPROCESSORSCALARBARLEGENDBOXSETTINGDIALOG_H
