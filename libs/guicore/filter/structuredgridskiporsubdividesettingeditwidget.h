#ifndef STRUCTUREDGRIDSKIPORSUBDIVIDESETTINGEDITWIDGET_H
#define STRUCTUREDGRIDSKIPORSUBDIVIDESETTINGEDITWIDGET_H

#include "../guicore_global.h"

#include <QWidget>

namespace Ui {
class StructuredGridSkipOrSubdivideSettingEditWidget;
}

class StructuredGridSkipOrSubdivideSettingContainer;

class GUICOREDLL_EXPORT StructuredGridSkipOrSubdivideSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit StructuredGridSkipOrSubdivideSettingEditWidget(QWidget *parent = nullptr);
	~StructuredGridSkipOrSubdivideSettingEditWidget();

	void setupNominations(int max);

	StructuredGridSkipOrSubdivideSettingContainer setting() const;
	void setSetting(const StructuredGridSkipOrSubdivideSettingContainer& setting);

private slots:
	void handleSpaceSliderChange(int val);

private:
	std::vector<int> m_subDivideNominations;
	std::vector<int> m_skipNominations;

	Ui::StructuredGridSkipOrSubdivideSettingEditWidget *ui;
};

#endif // STRUCTUREDGRIDSKIPORSUBDIVIDESETTINGEDITWIDGET_H
