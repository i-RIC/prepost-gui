#ifndef POST3DWINDOWNODEVECTORARROWDATAITEM_H
#define POST3DWINDOWNODEVECTORARROWDATAITEM_H

#include "post3dwindowfacedataitem.h"
#include "post3dwindownodevectorarrowsettingdialog.h"
#include "../post3dwindowdataitem.h"
#include "../post3dfacesettingcontainer.h"
#include "../post3dsamplingratesettingcontainer.h"

#include <guibase/arrowsettingcontainer.h>
#include <guibase/vtktool/vtkarrowsactor.h>

class vtkExtractGrid;
class vtkTransformFilter;

class Post3dWindowNodeVectorArrowDataItem : public Post3dWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowNodeVectorArrowDataItem(Post3dWindowDataItem* parent);
	~Post3dWindowNodeVectorArrowDataItem();

	Post3dWindowNodeVectorArrowSettingDialog::FaceSetting setting() const;
	void setSetting(const Post3dWindowNodeVectorArrowSettingDialog::FaceSetting& faceSetting);

	void update();

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	void innerUpdateZScale(double scale) override;
	void innerUpdate2Ds() override;

	void updateActorSettings();
	void updatePolyData();
	void updateColorSetting();

	vtkArrowsActor m_arrowsActor;
	vtkExtractGrid* m_extractGrid;
	vtkTransformFilter* m_transformFilter;

	ArrowSettingContainer m_arrowSetting;
	Post3dFaceSettingContainer m_faceSetting;
	Post3dSamplingRateSettingContainer m_samplingRateSetting;
};

#endif // POST3DWINDOWNODEVECTORARROWDATAITEM_H
