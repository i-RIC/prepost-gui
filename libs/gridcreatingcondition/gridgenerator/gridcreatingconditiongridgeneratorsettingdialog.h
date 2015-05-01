#ifndef GRIDCREATINGCONDITIONGRIDGENERATORSETTINGDIALOG_H
#define GRIDCREATINGCONDITIONGRIDGENERATORSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class GridCreatingConditionGridGeneratorSettingDialog;
}

class GridCreatingConditionGridGeneratorSettingDialog : public QDialog
{
	Q_OBJECT
public:
	GridCreatingConditionGridGeneratorSettingDialog(QWidget* parent = 0);
	~GridCreatingConditionGridGeneratorSettingDialog();

	enum ChannelShape {SingleSection, SimpleCompound, ComplexCompound};
	enum FloodChannel {Fixed, Movable};
	enum BedShape {Flat, AlternateBar, Parabolic};
	enum WidthChange {Constant, Change};
	enum WidthChangeType {Both, Left, Right};

	void setChannelShape(ChannelShape c);
	void setFloodChannelProperty(FloodChannel f);
	void setBedShape(BedShape b);
	void setAmplitude(double amp);
	void setLag(double lag);
	void setWaveLength(double length);
	void setWaveNumber(int waveNum);
	void setAngle(double angle);
	void setSlope(double slope);
	void setLongitudinalGridNumber(int gridNum);
	void setSimpleChannelWidth(double width);
	void setSimpleChannelGridNumber(int gridNum);
	void setSimpleCompoundLeftWidth(double width);
	void setSimpleCompoundRightWidth(double width);
	void setSimpleCompoundLowWidth(double width);
	void setComplexCompoundTotalWidth(double width);
	void setComplexCompoundLowWidth(double width);
	void setLeftGridNumber(int gridNum);
	void setRightGridNumber(int gridNum);
	void setLowGridNumber(int gridNum);
	void setLowDepth(double depth);
	void setLowSlope(double slope);
	void setBankGridNumber(int gridNum);
	void setWidthChange(WidthChange wc);
	void setWidthChangeType(WidthChangeType wct);
	void setWidthVariation(double var);

	ChannelShape channelShape();
	FloodChannel floodChannel();
	BedShape bedShape();
	double amplitude();
	double lag();
	double waveLength();
	int waveNumber();
	double angle();
	double slope();
	int longitudinalGridNumber();
	double simpleChannelWidth();
	int simpleChannelGridNumber();
	double simpleCompoundLeftWidth();
	double simpleCompoundRightWidth();
	double simpleCompoundLowWidth();
	double complexCompoundTotalWidth();
	double complexCompoundLowWidth();
	int leftGridNumber();
	int rightGridNumber();
	int lowGridNumber();
	double lowDepth();
	double lowSlope();
	int bankGridNumber();
	WidthChange widthChange();
	WidthChangeType widthChangeType();
	double widthVariation();

	void setupConnections();
	void setupDisability();
private:
	Ui::GridCreatingConditionGridGeneratorSettingDialog* ui;
};

#endif // GRIDCREATINGCONDITIONGRIDGENERATORSETTINGDIALOG_H
