#ifndef FILENAMEEDITWIDGET_H
#define FILENAMEEDITWIDGET_H

#include "../guibase_global.h"

#include <QWidget>

class QString;

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

	QString filename() const;
	void setFilename(const QString& filename);

	/// Filter string like "CSV files (*.csv)"
	void setFilter(const QString& filter);

	/// if a file under baseFolder is selected, file name fill be the relative file name.
	void setBaseFolder(const QString& folder);

	void setSaveMode(bool saveMode);

private slots:
	void openDialog();

private:
	QString m_filter;
	QString m_baseFolder;
	bool m_saveMode;

	Ui::FilenameEditWidget* ui;
};

#endif // FILENAMEEDITWIDGET_H
