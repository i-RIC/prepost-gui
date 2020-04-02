#ifndef INPUTCONDITIONPAGELIST_H
#define INPUTCONDITIONPAGELIST_H

#include "../../guicore_global.h"

#include <QString>
#include <QTreeWidget>
#include <QMap>

class QDomElement;
class SolverDefinitionTranslator;
class QTreeWidgetItem;

/// This class handles the list of pages displayed in the left side of
/// InputConditionDialog.
class GUICOREDLL_EXPORT InputConditionPageList : public QTreeWidget
{
	Q_OBJECT

private:
	const static int WIDTH_MIN = 150;

public:
	/// Constructor
	InputConditionPageList(QWidget* w = nullptr);
	void setup(const QDomElement& elem, const SolverDefinitionTranslator& translator);
	void selectFirstItem();

signals:
	void selectChanged(const QString& pagename);

private slots:
	void handleSelectionChange();

private:
	QMap<QTreeWidgetItem*, QString> m_nameMap;
};

#endif // INPUTCONDITIONPAGELIST_H
