#ifndef GRIDCOMPLEXCONDITIONWIDGET_H
#define GRIDCOMPLEXCONDITIONWIDGET_H

#include "../../guicore_global.h"
#include <QWidget>
#include "../../project/inputcond/inputconditioncontainerstring.h"
#include "../../project/inputcond/inputconditioncontainerinteger.h"

class QAbstractButton;
class SolverDefinition;
class InputConditionContainerSet;
class QDomElement;
class QLocale;

namespace Ui
{
	class GridComplexConditionWidget;
}

class InputConditionWidgetSet;
class iRICMainWindowInterface;
class PreProcessorGridRelatedComplexConditionDataItem;

class GUICOREDLL_EXPORT GridComplexConditionWidget : public QWidget
{
	Q_OBJECT

public:
	class Setting
	{

	public:
		Setting() {
			containerSet = nullptr;
		}
		~Setting() {}
		InputConditionContainerSet* containerSet;
		InputConditionContainerString captionContainer;
		InputConditionContainerString colorContainer;
		InputConditionContainerInteger isDefaultContainer;
	};

	/// Constructor
	GridComplexConditionWidget(iRICMainWindowInterface* mw, QWidget* parent = nullptr);
	/// Destructor
	~GridComplexConditionWidget();
	void setNameAndNumber(const std::string& name, int number);
	/// Load data fron CGNS file.
	void load(const int fn);
	/// Save data into CGNS file.
	void save(const int fn);
	/// Setup the widget.
	void setup(SolverDefinition* def, const QDomElement& elem, const QLocale& locale);

	void setCaption(const QString& caption);
	QString caption() const;
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

	Ui::GridComplexConditionWidget* ui;
	InputConditionContainerSet* m_containerSet;
	InputConditionWidgetSet* m_widgetSet;
	bool m_modified;

	QString m_caption;

	InputConditionContainerString m_captionContainer;
	InputConditionContainerString m_colorContainer;
	InputConditionContainerInteger m_isDefaultContainer;
};

#endif // GRIDCOMPLEXCONDITIONWIDGET_H
