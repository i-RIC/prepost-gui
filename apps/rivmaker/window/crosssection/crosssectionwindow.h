#ifndef CROSSSECTIONWINDOW_H
#define CROSSSECTIONWINDOW_H

#include "../../widgets/chartwindow.h"

class CrossSection;
class CrossSectionWindowGraphicsView;
class Project;

class QComboBox;
class QPushButton;

class CrossSectionWindow : public ChartWindow
{
	Q_OBJECT

public:
	explicit CrossSectionWindow(QWidget *parent = nullptr);
	~CrossSectionWindow();

	Project* project() const;
	void setProject(Project* project);

	CrossSection* currentCrossSection() const;

	bool showWSE() const;
	bool showLBHWM() const;
	bool showRBHWM() const;

signals:
	void positionChangedForStatusBar(const QPointF& position);

private slots:
	void exportSVG();
	void exportCSV();
	void editDisplaySetting();
	void updateView();
	void handleSelectionChange(int selected);

private:
	void setupToolBars();
	void updateWindowTitle();

	CrossSectionWindowGraphicsView* graphicsView() const;

	CrossSection* m_currentCrossSection;
	Project* m_project;

	QComboBox* m_crossSectionComboBox;

	QPushButton* m_exportSVGButton;
	QPushButton* m_exportCSVButton;

	QCheckBox* m_WSECheckBox;
	QCheckBox* m_LBHWMCheckBox;
	QCheckBox* m_RBHWMCheckBox;
	QPushButton* m_displaySettingButton;
};

#endif // CROSSSECTIONWINDOW_H
