#ifndef GRIDCOMPLEXCONDITIONGROUP_H
#define GRIDCOMPLEXCONDITIONGROUP_H

#include "../../guicore_global.h"

#include <QColor>
#include <QString>

class SolverDefinition;
class InputConditionContainerSet;

class QDomElement;
class QWidget;

class GUICOREDLL_EXPORT GridComplexConditionGroup
{
public:
	class Setting {
	public:
		Setting();
		~Setting();

		QString caption;
		QColor color;
		bool isDefault;
		InputConditionContainerSet* containerSet;
	};

	GridComplexConditionGroup(SolverDefinition* def, const QDomElement& elem);
	~GridComplexConditionGroup();

	void setNameAndNumber(const std::string& name, int number);

	void load();
	void save();

	QWidget* widget() const;
	InputConditionContainerSet* containerSet() const;

	QString caption() const;
	void setCaption(const QString& caption);

	QColor color() const;
	void setColor(const QColor& color);

	bool isDefault() const;
	void setIsDefault(bool isDefault);

	Setting setting() const;
	void setSetting(const Setting& setting);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/gridcomplexconditiongroup_impl.h"
#endif // _DEBUG

#endif // GRIDCOMPLEXCONDITIONGROUP_H
