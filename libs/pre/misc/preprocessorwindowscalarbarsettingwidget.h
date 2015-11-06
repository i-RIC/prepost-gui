#ifndef PREPROCESSORWINDOWSCALARBARSETTINGWIDGET_H
#define PREPROCESSORWINDOWSCALARBARSETTINGWIDGET_H

#include <guibase/scalarbarsetting.h>
#include <guibase/vtktextpropertysettingcontainer.h>
#include <QWidget>

class QButtonGroup;

namespace Ui
{
	class PreProcessorWindowScalarBarSettingWidget;
}

class PreProcessorWindowScalarBarSettingWidget : public QWidget
{
	Q_OBJECT

public:
	struct Setting {
		static const int DEFAULT_NUMOFLABELS = 8;
		Setting();
		bool visible;
		ScalarBarSetting::Orientation orientation;
		int numberOfLabels;
		double width;
		double height;
		double positionX;
		double positionY;
	};

	PreProcessorWindowScalarBarSettingWidget(QWidget* parent = nullptr);
	~PreProcessorWindowScalarBarSettingWidget();

	void setVisibility(bool visible);
	void setOrientation(ScalarBarSetting::Orientation o);
	void setTitle(const QString& title);
	void setNumberOfLabels(int labels);
	void setMaxNumberOfLabels(int max);
	void setWidth(double width);
	void setHeight(double height);
	void setPositionX(double x);
	void setPositionY(double y);
	void setSetting(const Setting& setting);
	void setTitleTextSetting(const vtkTextPropertySettingContainer& cont);
	void setLabelTextSetting(const vtkTextPropertySettingContainer& cont);
	void setLabelFormat(const QString& format);

	bool visibility() const;
	ScalarBarSetting::Orientation orientation() const;
	QString title() const;
	int numberOfLabels() const;
	double width() const;
	double height() const;
	double positionX() const;
	double positionY() const;
	Setting setting() const;
	const vtkTextPropertySettingContainer& titleTextSetting() const;
	const vtkTextPropertySettingContainer& labelTextSetting() const;
	QString labelFormat() const;
	void hideDisplayCheckBox();

public slots:
	void setTitleModeToManual(bool checked);
	void setDisplayScalarBar(bool checked);
	void editTitleTextSetting();
	void editLabelTextSetting();

private:
	vtkTextPropertySettingContainer m_titleTextSetting;
	vtkTextPropertySettingContainer m_labelTextSetting;

	Ui::PreProcessorWindowScalarBarSettingWidget* ui;
	QButtonGroup* m_orientationGroup;
	QButtonGroup* m_titleGroup;
};

#endif // PREPROCESSORWINDOWSCALARBARSETTINGWIDGET_H
