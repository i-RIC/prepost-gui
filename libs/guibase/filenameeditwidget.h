#ifndef FILENAMEEDITWIDGET_H
#define FILENAMEEDITWIDGET_H

#include "guibase_global.h"

#include <QWidget>

namespace Ui
{
	class FilenameEditWidget;
}

/**
 * @brief Widget to edit file name
 *
 * The Widget consists of a text box and a push button.
 * User can input the file name (full path) into the text box, or click on the push button
 * to select the file from the dialog open.
 */
class GUIBASEDLL_EXPORT FilenameEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit FilenameEditWidget(QWidget* parent = nullptr);
	~FilenameEditWidget();
	/// Set the file name
	void setFilename(const QString& filename);
	/**
	 * @brief Set the filter for files to select
	 * @param filter Filter string. For example, "CSV files (*.csv)"
	 */
	void setFilter(const QString& filter);
	/// The file name
	const QString filename() const;
	/// Set the base directory, that is used when file selecting dialog is open by calling openDialog()
	void setBaseDir(const QString& basedir) {
		m_baseDir = basedir;
	}

private slots:
	void openDialog();

private:
	Ui::FilenameEditWidget* ui;
	QString m_filter;
	QString m_baseDir;
};

#endif // FILENAMEEDITWIDGET_H
