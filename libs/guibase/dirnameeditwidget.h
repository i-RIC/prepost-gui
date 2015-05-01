#ifndef DIRNAMEEDITWIDGET_H
#define DIRNAMEEDITWIDGET_H

#include "guibase_global.h"

#include <QWidget>

namespace Ui
{
	class DirnameEditWidget;
}

class GUIBASEDLL_EXPORT DirnameEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit DirnameEditWidget(QWidget* parent = nullptr);
	~DirnameEditWidget();
	void setDirname(const QString& dirname);
	const QString dirname() const;

private slots:
	void openDialog();

private:
	Ui::DirnameEditWidget* ui;
};

#endif // DIRNAMEEDITWIDGET_H
