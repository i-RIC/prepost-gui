#ifndef SCALARBARDIALOG_H
#define SCALARBARDIALOG_H

#include "guibase_global.h"
#include "scalarbarsetting.h"

#include <QDialog>

class vtkTextPropertySettingContainer;

namespace Ui
{
	class ScalarBarDialog;
}

/// Dialog to edit scalar bar setting
class GUIBASEDLL_EXPORT ScalarBarDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ScalarBarDialog(QWidget* parent = nullptr);
	~ScalarBarDialog();

	ScalarBarSetting setting() const;
	void setSetting(const ScalarBarSetting& setting);

	QString title() const;
	void setTitle(const QString& title);

private:
	Ui::ScalarBarDialog* ui;
};

#endif // SCALARBARDIALOG_H
