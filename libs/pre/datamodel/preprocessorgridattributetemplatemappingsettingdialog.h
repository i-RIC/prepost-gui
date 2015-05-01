#ifndef PREPROCESSORGRIDATTRIBUTETEMPLATEMAPPINGSETTINGDIALOG_H
#define PREPROCESSORGRIDATTRIBUTETEMPLATEMAPPINGSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class PreProcessorGridAttributeTemplateMappingSettingDialog;
}

class PreProcessorGridAttributeTemplateMappingSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PreProcessorGridAttributeTemplateMappingSettingDialog(QWidget* parent = nullptr);
	~PreProcessorGridAttributeTemplateMappingSettingDialog();

	bool autoMode();
	void setAutoMode(bool a);
	double streamWiseLength();
	void setStreamWiseLength(double len);
	double crossStreamLength();
	void setCrossStreamLength(double len);
	int numExpansion();
	void setNumExpansion(int num);
	double weightExponent();
	void setWeightExponent(double exp);
private:
	Ui::PreProcessorGridAttributeTemplateMappingSettingDialog* ui;
};

#endif // PREPROCESSORGRIDATTRIBUTETEMPLATEMAPPINGSETTINGDIALOG_H
