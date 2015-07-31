#ifndef PREPROCESSORSCALARBAREDITDIALOG_H
#define PREPROCESSORSCALARBAREDITDIALOG_H

#include "preprocessorwindowscalarbarsettingwidget.h"

#include <QDialog>

namespace Ui
{
	class PreProcessorScalarBarEditDialog;
}

class PreProcessorScalarBarEditDialog : public QDialog
{
	Q_OBJECT

public:
	PreProcessorScalarBarEditDialog(QWidget* parent = nullptr);
	~PreProcessorScalarBarEditDialog();

	void setScalarBarTitle(const QString& title);
	void setOrientation(ScalarBarSetting::Orientation o);
	void setNumberOfLabels(int num);
	void setWidth(double width);
	void setHeight(double height);
	void setPositionX(double x);
	void setPositionY(double y);
	void setColorBarSetting(PreProcessorWindowScalarBarSettingWidget::Setting s);
	void setTitleTextSetting(const vtkTextPropertySettingContainer& cont);
	void setLabelTextSetting(const vtkTextPropertySettingContainer& cont);
	void setLabelFormat(const QString& format);

	QString scalarBarTitle() const;
	ScalarBarSetting::Orientation orientation() const;
	int numberOfLabels() const;
	double width() const;
	double height() const;
	double positionX() const;
	double positionY() const;
	PreProcessorWindowScalarBarSettingWidget::Setting colorBarSetting() const;
	const vtkTextPropertySettingContainer& titleTextSetting() const;
	const vtkTextPropertySettingContainer& labelTextSetting() const;
	QString labelFormat() const;

	void hideDisplayCheckBox();

private:
	Ui::PreProcessorScalarBarEditDialog* ui;
};

#endif // PREPROCESSORSCALARBAREDITDIALOG_H
