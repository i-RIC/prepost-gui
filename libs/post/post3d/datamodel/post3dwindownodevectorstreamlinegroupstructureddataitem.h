#ifndef POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H
#define POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H

#include "post3dwindownodevectorstreamlinegroupdataitem.h"
#include <guibase/vtksubdividegrid.h>
#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>

struct Post3dWindowStructuredStreamlineSetSetting {
	enum SpaceMode {smNormal, smSubdivide, smSkip};
	/// Range to generate
	StructuredGridRegion::Range3d range;
	SpaceMode spaceMode;
	int spaceSamplingRate;
	int spaceDivision;
	/// Streamline color.
	QColor color;
	/// Streamline width on screen. Specify by pixels on screen.
	int width;
};

class Post3dWindowStreamlineStructuredSetProperty;

class Post3dWindowNodeVectorStreamlineGroupStructuredDataItem : public Post3dWindowNodeVectorStreamlineGroupDataItem
{
	Q_OBJECT
public:
	Post3dWindowNodeVectorStreamlineGroupStructuredDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorStreamlineGroupStructuredDataItem();
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

	QList<Post3dWindowStructuredStreamlineSetSetting> m_settings;
public:
	friend class Post3dWindowStreamlineStructuredSetProperty;
};

#endif // POST3DWINDOWNODEVECTORSTREAMLINEGROUPSTRUCTUREDDATAITEM_H
