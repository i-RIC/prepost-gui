#ifndef POSTSTRINGRESULTSETTINGCONTAINER_H
#define POSTSTRINGRESULTSETTINGCONTAINER_H

#include "poststringresultargumentcontainer.h"

#include <guicore/image/imagesettingcontainer.h>
#include <misc/colorcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/qfontcontainer.h>
#include <misc/stringcontainer.h>

class PostStringResultSettingContainer : public CompositeContainer
{
public:
	PostStringResultSettingContainer();
	PostStringResultSettingContainer(const PostStringResultSettingContainer& s);
	~PostStringResultSettingContainer();

	PostStringResultSettingContainer& operator=(const PostStringResultSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;
	void copyValue(const XmlAttributeContainer& c) override;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;

	bool importData(const QString& fileName);
	bool exportData(const QString& fileName);

	StringContainer script;
	std::vector<PostStringResultArgumentContainer> arguments;

	ColorContainer fontColor;
	QFontContainer font;
	ColorContainer backgroundColor;

	ImageSettingContainer imageSetting;

	class ImageBuilder;
	ImageBuilder* imageBuilder;
};

#endif // POSTSTRINGRESULTSETTINGCONTAINER_H
