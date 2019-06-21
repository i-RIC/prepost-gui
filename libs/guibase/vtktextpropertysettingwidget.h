#ifndef VTKTEXTPROPERTYSETTINGWIDGET_H
#define VTKTEXTPROPERTYSETTINGWIDGET_H

#include "guibase_global.h"

#include <QString>
#include <QWidget>

class vtkTextPropertySettingContainer;

namespace Ui {
class vtkTextPropertySettingWidget;
}

class GUIBASEDLL_EXPORT vtkTextPropertySettingWidget : public QWidget
{
	Q_OBJECT

public:
	explicit vtkTextPropertySettingWidget(QWidget *parent = nullptr);
	~vtkTextPropertySettingWidget();
	/// The setting
	vtkTextPropertySettingContainer setting() const;
	/// Set the setting
	void setSetting(const vtkTextPropertySettingContainer& setting);

	/// Disable the widget to edit font size
	void disableSize();
	/// Disable the widget to edit font color
	void disableColor();
	/// Hide the widget to edit font color
	void hideColor();

private:
	QString m_prefix;
	Ui::vtkTextPropertySettingWidget *ui;
};

#endif // VTKTEXTPROPERTYSETTINGWIDGET_H
