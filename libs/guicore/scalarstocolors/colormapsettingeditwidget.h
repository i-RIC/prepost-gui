#ifndef COLORMAPSETTINGEDITWIDGET_H
#define COLORMAPSETTINGEDITWIDGET_H

#include "../guicore_global.h"
#include "colormapsettingcontainer.h"
#include "colormapsettingeditwidgeti.h"

namespace Ui {
class ColorMapSettingEditWidget;
}

class QTableView;

class GUICOREDLL_EXPORT ColorMapSettingEditWidget : public ColorMapSettingEditWidgetI
{
	Q_OBJECT

public:
	explicit ColorMapSettingEditWidget(QWidget *parent);
	~ColorMapSettingEditWidget();

	ColorMapSettingContainer concreteSetting() const;
	void setConcreteSetting(const ColorMapSettingContainer& setting);

	QUndoCommand* createModifyCommand() const override;

public slots:
	void importSetting() override;
	void exportSetting() override;

private slots:
	void selectColorMapSetting(int index);
	void switchToAutoMode();
	void switchToManualMode();
	void switchTransitionMode();

	void switchAutoValueRange(bool automatic);
	void changeFillLower(bool fill);
	void changeFillUpper(bool fill);
	void changeMinValue(double min);
	void changeMaxValue(double max);

	void addColor();
	void removeColor();

private:
	QTableView* colorTable() const;
	QPushButton* removeButton() const;

	void importSetting(const QString& fileName, bool ignoreLegendSetting);

	void applySettingOtherThanLegend();
	void updateLegendNumberOfLabels();
	void fixData();
	void setupColorPatternSelectComboBox();
	void updateSwitchButtonText();
	void switchTransitionModeToContinuous();
	void switchTransitionModeToDiscrete();

	void setupWidget() override;

	ColorMapSettingContainer m_concreteSetting;
	bool m_applying;

	class ColorTableController;
	ColorTableController* m_colorTableController;

	std::vector<QString> m_importTargetFileNames;

	Ui::ColorMapSettingEditWidget *ui;

	class SwitchToDiscreteDialog;
};

#endif // COLORMAPSETTINGEDITWIDGET_H
