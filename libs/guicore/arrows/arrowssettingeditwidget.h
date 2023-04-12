#ifndef ARROWSSETTINGEDITWIDGET_H
#define ARROWSSETTINGEDITWIDGET_H

#include "../guicore_global.h"
#include "../scalarstocolors/colormapsettingcontainer.h"

#include <QWidget>

#include <map>
#include <unordered_map>

namespace Ui {
class ArrowsSettingEditWidget;
}

class ArrowsSettingContainer;
class ColorMapSettingContainer;

class QTabWidget;
class QUndoCommand;

class GUICOREDLL_EXPORT ArrowsSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ArrowsSettingEditWidget(QWidget *parent = nullptr);
	~ArrowsSettingEditWidget();

	void setValueNames(const std::map<std::string, QString> names);
	void setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainer*>& settings);
	void setAdditionalSettingWidget(QWidget* widget);

	QTabWidget* tabWidget() const;

	ArrowsSettingContainer setting() const;
	void setSetting(ArrowsSettingContainer* setting);

	QUndoCommand* createModifyCommand() const;

private slots:
	void handleTargetChange(const QString& value);

private:
	std::vector<std::string> m_valueNames;

	ArrowsSettingContainer* m_setting;

	class ModifyCommand;

	Ui::ArrowsSettingEditWidget *ui;
};

#endif // ARROWSSETTINGEDITWIDGET_H