#ifndef FILENAMEEDITWIDGET_H
#define FILENAMEEDITWIDGET_H

#include "guibase_global.h"

#include <QWidget>

namespace Ui
{
	class FilenameEditWidget;
}

class GUIBASEDLL_EXPORT FilenameEditWidget : public QWidget
{
	Q_OBJECT

public:
	explicit FilenameEditWidget(QWidget* parent = nullptr);
	~FilenameEditWidget();
	void setFilename(const QString& filename);
	void setFilter(const QString& filter);
	const QString filename() const;
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
