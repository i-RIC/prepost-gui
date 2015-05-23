#ifndef COLORMAPCUSTOMSETTINGDIALOG_H
#define COLORMAPCUSTOMSETTINGDIALOG_H

#include "guibase_global.h"
#include "colormapsettingwidget.h"

#include <QDialog>
#include <QList>
#include <QColor>

namespace Ui
{
	class ColorMapCustomSettingDialog;
}

class QTableWidgetItem;
class iRICMainWindow;

/// Dialog to edit custom setting of color map
class GUIBASEDLL_EXPORT ColorMapCustomSettingDialog : public QDialog
{
	Q_OBJECT

private:
	const static int VALUEWIDTH = 120;
	const static int COLORWIDTH = 40;
	const static int ROWHEIGHT = 20;

public:
	explicit ColorMapCustomSettingDialog(QWidget* parent = nullptr);
	~ColorMapCustomSettingDialog();
	/// Set custom setting of color map
	void setCustomSetting(const ColorMapSettingWidget::CustomSetting& setting);
	/// Returns the edited custom setting of color map
	ColorMapSettingWidget::CustomSetting customSetting();

private slots:
	void typeUpdated(int type);
	void addColor();
	void removeColor();
	void importColors();
	void exportColors();
	void handleItemEdit(QTableWidgetItem* item);
	void handleItemClick(QTableWidgetItem* item);

private:
	Ui::ColorMapCustomSettingDialog* ui;
	QList<QColor> m_arbitraryColors;
};

#endif // COLORMAPCUSTOMSETTINGDIALOG_H
