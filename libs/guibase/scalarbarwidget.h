#ifndef SCALARBARWIDGET_H
#define SCALARBARWIDGET_H

#include "guibase_global.h"

#include "scalarbarsetting.h"

#include <QWidget>

class ScalarBarSetting;

namespace Ui
{
	class ScalarBarWidget;
}

/// Widget to edit scalar bar setting
class GUIBASEDLL_EXPORT ScalarBarWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ScalarBarWidget(QWidget* parent = nullptr);
	~ScalarBarWidget();

	ScalarBarSetting setting() const;
	void setSetting(const ScalarBarSetting& setting);

	QString title() const;
	void setTitle(const QString& title);

private slots:
	void editTitleTextSetting();
	void editLabelTextSetting();

private:
	ScalarBarSetting m_setting;

	Ui::ScalarBarWidget* ui;
};

#endif // SCALARBARWIDGET_H
