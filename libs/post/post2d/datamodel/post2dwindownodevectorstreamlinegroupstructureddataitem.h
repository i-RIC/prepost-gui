#ifndef POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H
#define POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H

#include "post2dwindownodevectorstreamlinegroupdataitem.h"
#include <guibase/vtksubdividegrid.h>
#include <vtkExtractGrid.h>

struct Post2dWindowStructuredStreamlineSetSetting
{
	enum SpaceMode {smNormal, smSubdivide, smSkip};
	/// Range to generate
	StructuredGridRegion::Range2d range;
	SpaceMode spaceMode;
	int spaceSamplingRate;
	int spaceDivision;
	/// Streamline color.
	QColor color;
	/// Streamline width on screen. Specify by pixels on screen.
	int width;
};

class Post2dWindowStreamlineStructuredSetProperty;

class Post2dWindowNodeVectorStreamlineGroupStructuredDataItem : public Post2dWindowNodeVectorStreamlineGroupDataItem
{
	Q_OBJECT
public:
	Post2dWindowNodeVectorStreamlineGroupStructuredDataItem(Post2dWindowDataItem* parent);
	~Post2dWindowNodeVectorStreamlineGroupStructuredDataItem();
protected:
	vtkPointSet* getSource(int i);
	void setupActors();
	QDialog* propertyDialog(QWidget* parent);
	void handlePropertyDialogAccepted(QDialog* propDialog);
	void doLoadFromProjectMainFile(const QDomNode& node);
	void doSaveToProjectMainFile(QXmlStreamWriter& writer);
private:
	void setupDefaultValues();

	QList<vtkExtractGrid*> m_extractGrids;
	QList<vtkSubdivideGrid*> m_subdivideGrids;

	QList<Post2dWindowStructuredStreamlineSetSetting> m_settings;
public:
	friend class Post2dWindowStreamlineStructuredSetProperty;
};

#endif // POST2DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H
