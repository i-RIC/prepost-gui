#ifndef GRIDCREATINGCONDITIONGRIDGENERATOR_H
#define GRIDCREATINGCONDITIONGRIDGENERATOR_H

#include <guicore/pre/gridcreatingcondition/gridcreatingcondition.h>
#include "gridcreatingconditiongridgeneratorsettingdialog.h"

class GridCreatingConditionGridGenerator : public GridCreatingCondition
{
	Q_OBJECT

public:
	GridCreatingConditionGridGenerator(ProjectDataItem* parent, GridCreatingConditionCreator* creator);
	~GridCreatingConditionGridGenerator();

	bool create(QWidget* parent) override;
	void showCondition(QWidget* parent) override;
	bool showConditionAvailable() override;
	bool ready() const override;
	void setupMenu() override;
	bool setupParameters();
	void showInitialDialog() override;
	void clear() override;

protected:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
	void loadGridGeneratorFromProjectMainFile(const QDomNode& node);
	void saveGridGeneratorToProjectMainFile(QXmlStreamWriter& writer);

	int m_iMax;
	int m_jMax;

	double** xp;
	double** yp;
	double** zp;
	int** obst;
	int ndy[6];
	double wid[6];

	// General Parameters
	GridCreatingConditionGridGeneratorSettingDialog::ChannelShape j_chn;
	GridCreatingConditionGridGeneratorSettingDialog::FloodChannel j_fix;
	// Parameters for bed shape
	GridCreatingConditionGridGeneratorSettingDialog::BedShape j_bar;
	double amp;
	double delta;
	// Common Parameters
	int nt;
	int irns;
	double sl;
	double phi0;
	double slope;
	// Simple Section
	double width;
	int ny;
	// Compound Channel
	double height_l;
	double alpha_l;
	int ny_m;
	double width_t;
	double width_ct;
	// Width Variation
	GridCreatingConditionGridGeneratorSettingDialog::WidthChange j_width;
	GridCreatingConditionGridGeneratorSettingDialog::WidthChangeType j_width_type;
	double delta_b;

	void mousePressEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;
	void mouseReleaseEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/) override;

	QPoint m_dragStartPoint;
	QMenu* m_rightClickingMenu;
};

#endif // GRIDCREATINGCONDITIONGRIDGENERATOR_H
