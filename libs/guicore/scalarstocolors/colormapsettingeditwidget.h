#ifndef COLORMAPSETTINGEDITWIDGET_H
#define COLORMAPSETTINGEDITWIDGET_H

#include "../guicore_global.h"
#include "colormapsettingcontainer.h"
#include "colormapsettingeditwidgeti.h"

namespace Ui {
class ColorMapSettingEditWidget;
}

class QPushButton;
class QTableView;

class GUICOREDLL_EXPORT ColorMapSettingEditWidget : public ColorMapSettingEditWidgetI
{
	Q_OBJECT

public:
	explicit ColorMapSettingEditWidget(QWidget *parent);
	~ColorMapSettingEditWidget();

	ColorMapSettingContainer concreteSetting() const;
	void setConcreteSetting(const ColorMapSettingContainer& setting);

	void setDisableOtherThanLegendVisible(bool disabled) override;
	QUndoCommand* createModifyCommand() const override;

public slots:
	void importSetting() override;
	void exportSetting() override;

private slots:
	void switchToAutoMode();
	void switchToManualMode();
	void switchTransitionMode();

	void reverseColors();
	void divideEqually();

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

	void applySettingOtherThanLegend();
	void updateLegendNumberOfLabels();
	void fixData();
	void updateSwitchButtonText();
	void switchTransitionModeToContinuous();
	void switchTransitionModeToDiscrete();
	void normalizeColors();

	void setupWidget() override;
	void updateImageSetting() override;

	ColorMapSettingContainer m_concreteSetting;
	bool m_applying;

	class ColorTableController;
	ColorTableController* m_colorTableController;

	std::vector<QString> m_importTargetFileNames;

	Ui::ColorMapSettingEditWidget *ui;

	class ImportDialog;
	class SwitchToDiscreteDialog;
};

#endif // COLORMAPSETTINGEDITWIDGET_H
