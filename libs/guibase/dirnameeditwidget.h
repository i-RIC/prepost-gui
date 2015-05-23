#ifndef DIRNAMEEDITWIDGET_H
#define DIRNAMEEDITWIDGET_H

#include "guibase_global.h"

#include <QWidget>

namespace Ui
{
	class DirnameEditWidget;
}

/**
 * @brief Widget to edit directory (folder) name
 *
 * This widget consists of a text box and a push button.
 * Users can input the full path of directory name in the text box, or
 * click on the push button to open dialog and select the folder from that dialog.
 */
class GUIBASEDLL_EXPORT DirnameEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit DirnameEditWidget(QWidget* parent = nullptr);
	~DirnameEditWidget();
	/// Set the directory name (in full path)
	void setDirname(const QString& dirname);
	/// The directory name (in full path)
	const QString dirname() const;

private slots:
	/// Open folder select dialog (The dialog shown is operating system dependent)
	void openDialog();

private:
	Ui::DirnameEditWidget* ui;
};

#endif // DIRNAMEEDITWIDGET_H
