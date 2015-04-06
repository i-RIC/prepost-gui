#ifndef SCALARBARWIDGET_H
#define SCALARBARWIDGET_H

#include "guibase_global.h"

#include "vtktextpropertysettingcontainer.h"

#include <QWidget>

class ScalarBarSetting;

namespace Ui {
	class ScalarBarWidget;
}

class GUIBASEDLL_EXPORT ScalarBarWidget : public QWidget {
	Q_OBJECT

public:
	explicit ScalarBarWidget(QWidget *parent = 0);
	~ScalarBarWidget();

	void setSetting(const ScalarBarSetting& setting);
	void setTitle(const QString& title);
	void setTitleTextSetting(const vtkTextPropertySettingContainer& cont);
	void setLabelTextSetting(const vtkTextPropertySettingContainer& cont);

	const ScalarBarSetting setting();
	QString title();
	const vtkTextPropertySettingContainer& titleTextSetting() const {return m_titleTextSetting;}
	const vtkTextPropertySettingContainer& labelTextSetting() const {return m_labelTextSetting;}

private slots:
	void editTitleTextSetting();
	void editLabelTextSetting();

private:
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;

	Ui::ScalarBarWidget *ui;
};

#endif // SCALARBARWIDGET_H
