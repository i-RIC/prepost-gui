#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorstreamlinegroupdataitem.h"
#include <guibase/vtksubdividegrid.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/intcontainer.h>
#include <misc/colorcontainer.h>
#include <vtkExtractGrid.h>

class Post2dWindowStreamlineStructuredSettingDialog;

class Post2dWindowNodeVectorStreamlineGroupStructuredDataItem : public Post2dWindowNodeVectorStreamlineGroupDataItem
{
	Q_OBJECT

public:
	struct Setting : public CompositeContainer
	{
		enum SpaceMode {smNormal, smSubdivide, smSkip};

		/// Constructor
		Setting();

		/// Range to generate
		StructuredGridRegion::Range2d range;
		EnumContainerT<SpaceMode> spaceMode;
		IntContainer spaceSamplingRate;
		IntContainer spaceDivision;
		/// Streamline color.
		ColorContainer color;
		/// Streamline width on screen. Specify by pixels on screen.
		IntContainer width;
	};

	Post2dWindowNodeVectorStreamlineGroupStructuredDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorStreamlineGroupStructuredDataItem();

protected:
	vtkPointSet* getSource(int i);
	void setupActors() override;
	QDialog* propertyDialog(QWidget* parent) override;
	void handlePropertyDialogAccepted(QDialog* propDialog) override;
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

private:
	void setupDefaultValues();

	QList<vtkExtractGrid*> m_extractGrids;
	QList<vtkSubdivideGrid*> m_subdivideGrids;

	QList<Setting> m_stSettings;

	class SetSettingCommand;

public:
	friend class Post2dWindowStreamlineStructuredSettingDialog;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H
