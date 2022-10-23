#ifndef ARROWSCOLORSETTINGEDITWIDGET_H
#define ARROWSCOLORSETTINGEDITWIDGET_H

#include <QWidget>

#include <unordered_map>

namespace Ui {
class ArrowsColorSettingEditWidget;
}

class ArrowsSettingContainer;
class ColorMapSettingContainer;

class ArrowsColorSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ArrowsColorSettingEditWidget(QWidget *parent = nullptr);
	~ArrowsColorSettingEditWidget();

	void setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainer*>& settings);

	ArrowsSettingContainer setting() const;
	void updateSetting(ArrowsSettingContainer* setting) const;
	void setSetting(const ArrowsSettingContainer& setting);

	QUndoCommand* colorMapModifyCommand() const;

private slots:
	void handleColorScalarChange(int index);

private:
	std::unordered_map<std::string, ColorMapSettingContainer*> m_colorMapSettings;
	std::vector<std::string> m_colorMapNames;

	Ui::ArrowsColorSettingEditWidget *ui;
};

#endif // ARROWSCOLORSETTINGEDITWIDGET_H
