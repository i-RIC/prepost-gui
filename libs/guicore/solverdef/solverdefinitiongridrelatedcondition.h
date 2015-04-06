#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

#include <QVariant>
#include <QList>

class QString;
class QDialog;
class QWidget;
class Grid;
class GridRelatedConditionContainer;
class GridRelatedConditionEditWidget;
class GridRelatedConditionEditDialog;
class GridRelatedConditionVariationEditWidget;
class GridRelatedConditionVariationEditDialog;
class RawData;
class ProjectDataItem;
class ScalarsToColorsContainer;
class ColorTransferFunctionContainer;
class LookupTableContainer;
class ScalarsToColorsEditDialog;
class ScalarsToColorsEditWidget;
class SolverDefinitionGridRelatedConditionDimension;

class GUICOREDLL_EXPORT SolverDefinitionGridRelatedCondition : public SolverDefinitionNode
{
public:
	enum Position {
		Node,           ///< Node (vertex)
		CellCenter      ///< Cell center
	};
	/// Constructor
	SolverDefinitionGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionNode(node, translator)
	{
		m_position = Node;
		m_isOption = false;
		load(node, translator);
	}
	virtual ~SolverDefinitionGridRelatedCondition();
	const QString& name() const {return m_name;}
	const QString& englishCaption() const {return m_englishCaption;}
	const QString& caption() const {return m_caption;}
	const QVariant& variantDefaultValue() const {return m_variantDefaultValue;}
	const QVariant& variantMaximumValue() const {return m_variantMaximumValue;}
	const QVariant& variantMinimumValue() const {return m_variantMinimumValue;}
	Position position() const {return m_position;}
	bool isOption() const {return m_isOption;}
	const QList<SolverDefinitionGridRelatedConditionDimension*>& dimensions() const {return m_dimensions;}
	QList<SolverDefinitionGridRelatedConditionDimension*>& dimensions() {return m_dimensions;}

	GridRelatedConditionContainer* container(Grid* grid);
	GridRelatedConditionEditDialog* editDialog(QWidget* parent);
	GridRelatedConditionVariationEditDialog* variationEditDialog(QWidget* parent);
	virtual GridRelatedConditionEditWidget* editWidget(QWidget* parent) = 0;
	virtual GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* parent) = 0;
	virtual RawData* buildBackgroundRawData(ProjectDataItem* parent) = 0;
	virtual ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d);
	virtual ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent);
	ScalarsToColorsEditDialog* createScalarsToColorsEditDialog(QWidget* parent);

protected:
	ColorTransferFunctionContainer* createColorTransferFunctionContainer(ProjectDataItem* d);
	LookupTableContainer* createLookupTableContainer(ProjectDataItem* d);
	ScalarsToColorsEditWidget* createColorTransferFunctionEditWidget(QWidget* parent);
	ScalarsToColorsEditWidget* createLookupTableEditWidget(QWidget* parent);
	virtual GridRelatedConditionContainer* buildContainer(Grid* grid) = 0;
	/// Load settings from solver definition file.
	void load(const QDomElement& node, const SolverDefinitionTranslator& translator);
	QString m_name;
	QString m_caption;
	QString m_englishCaption;
	bool m_isOption;
	QVariant m_variantDefaultValue;
	QVariant m_variantMaximumValue;
	QVariant m_variantMinimumValue;
	Position m_position;
	QList<SolverDefinitionGridRelatedConditionDimension*> m_dimensions;
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITION_H
