#ifndef GRIDCOMPLEXCONDITIONWIDGET_H
#define GRIDCOMPLEXCONDITIONWIDGET_H

#include "../../guicore_global.h"
#include <QWidget>
#include "../../project/inputcond/cgnsfileinputconditioncontainerset.h"
#include "../../project/inputcond/cgnsfileinputconditioncontainerstring.h"
#include "../../project/inputcond/cgnsfileinputconditioncontainerinteger.h"

class QAbstractButton;
class SolverDefinition;
class QDomElement;
class QLocale;

namespace Ui {
	class GridComplexConditionWidget;
}

class CgnsFileInputConditionWidgetSet;
class iRICMainWindowInterface;
class PreProcessorGridRelatedComplexConditionDataItem;

class GUICOREDLL_EXPORT GridComplexConditionWidget : public QWidget {
	Q_OBJECT
public:
	class Setting {
	public:
		Setting(){
			containerSet = nullptr;
		}
		~Setting(){}
		CgnsFileInputConditionContainerSet* containerSet;
		CgnsFileInputConditionContainerString captionContainer;
		CgnsFileInputConditionContainerString colorContainer;
		CgnsFileInputConditionContainerInteger isDefaultContainer;
	};

	/// Constructor
	GridComplexConditionWidget(iRICMainWindowInterface* mw, QWidget *parent = nullptr);
	/// Destructor
	~GridComplexConditionWidget();
	void setNameAndNumber(QString name, int number);
	/// Load data fron CGNS file.
	void load(const int fn);
	/// Save data into CGNS file.
	void save(const int fn);
	/// Setup the widget.
	void setup(SolverDefinition* def, const QDomElement& elem, const QLocale& locale);

	void setCaption(const QString& caption);
	const QString caption() const;
	void setColor(const QColor& color);
	QColor color() const;
	void setIsDefault(bool def);
	bool isDefault() const;
	Setting setting();
	void setSetting(const Setting& setting);

signals:
	void captionChanged(const QString& caption);
	void defaultChecked(bool checked);

private slots:
	void setModified();
	void handleDefaultCheck(bool checked);

private:

	Ui::GridComplexConditionWidget *ui;
	CgnsFileInputConditionContainerSet* m_containerSet;
	CgnsFileInputConditionWidgetSet* m_widgetSet;
	bool m_modified;

	QString m_caption;

	CgnsFileInputConditionContainerString m_captionContainer;
	CgnsFileInputConditionContainerString m_colorContainer;
	CgnsFileInputConditionContainerInteger m_isDefaultContainer;
};

#endif // GRIDCOMPLEXCONDITIONWIDGET_H
