#ifndef POST3DWINDOWCELLRANGELISTSETTINGEDITWIDGET_H
#define POST3DWINDOWCELLRANGELISTSETTINGEDITWIDGET_H

#include <QWidget>

namespace Ui {
class Post3dWindowCellRangeListSettingEditWidget;
}

class Post3dWindowCellRangeSettingContainer;

class Post3dWindowCellRangeListSettingEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit Post3dWindowCellRangeListSettingEditWidget(QWidget *parent = nullptr);
	~Post3dWindowCellRangeListSettingEditWidget();

	void setDimensions(int* dims);

	std::vector<Post3dWindowCellRangeSettingContainer> ranges();
	void setRanges(const std::vector<Post3dWindowCellRangeSettingContainer>& ranges);

private slots:
	void addRange();
	void removeRange();
	void setCurrentRange(int row);

private:
	void updateRangeList();
	void saveCurrentRange();

	std::vector<Post3dWindowCellRangeSettingContainer> m_ranges;
	int m_currentRow;
	int m_gridDimensions[3];

	Ui::Post3dWindowCellRangeListSettingEditWidget *ui;
};

#endif // POST3DWINDOWCELLRANGELISTSETTINGEDITWIDGET_H
