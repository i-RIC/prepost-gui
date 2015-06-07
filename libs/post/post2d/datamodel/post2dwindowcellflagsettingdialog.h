#ifndef POST2DWINDOWCELLFLAGSETTINGDIALOG_H
#define POST2DWINDOWCELLFLAGSETTINGDIALOG_H

#include "post2dwindowcellflaggroupdataitem.h"
#include <QDialog>
#include <QList>

class SolverDefinitionGridType;
class QTableWidgetItem;
class QItemSelection;

namespace Ui
{
	class Post2dWindowCellFlagSettingDialog;
}

class Post2dWindowCellFlagSettingDialog : public QDialog
{
	Q_OBJECT

public:
	static const int ATTRWIDTH = 170;
	static const int COLORWIDTH = 50;
	explicit Post2dWindowCellFlagSettingDialog(QWidget* parent = nullptr);
	~Post2dWindowCellFlagSettingDialog();
	void setGridType(SolverDefinitionGridType* gt) {m_gridType = gt;}
	void setSettings(const QList<Post2dWindowCellFlagSetting>& settings) {
		m_settings = settings;
		setupDialog();
	}
	void setOpacityPercent(int o);
	const QList<Post2dWindowCellFlagSetting>& settings();
	int opacityPercent();
	void setupDialog();

private slots:
	void moveUp();
	void moveDown();
	void handleItemClick(QTableWidgetItem*);
	void handleItemEdit(QTableWidgetItem*);
	void handleShownSelectionChange(const QItemSelection& selected, const QItemSelection& deselected);

private:
	Ui::Post2dWindowCellFlagSettingDialog* ui;
	SolverDefinitionGridType* m_gridType;
	QList<Post2dWindowCellFlagSetting> m_settings;
};

#endif // POST2DWINDOWCELLFLAGSETTINGDIALOG_H
