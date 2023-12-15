#ifndef POSTSTRINGRESULTENGINE_H
#define POSTSTRINGRESULTENGINE_H

class PostStringResultSettingContainer;
class v4PostZoneDataContainer;

class QJSValue;

class PostStringResultEngine : public QObject
{
	Q_OBJECT

public:
	PostStringResultEngine();
	~PostStringResultEngine();

	bool test(const PostStringResultSettingContainer& c, QString* result, QWidget* w);

	void setSetting(const PostStringResultSettingContainer& c);
	QString getValue(v4PostZoneDataContainer* container) const;

private:
	class Argument;

	bool build(const PostStringResultSettingContainer& c, QJSValue* func, std::vector<Argument>* args, QWidget* w);

	class Impl;
	Impl* impl;
};

#endif // POSTSTRINGRESULTENGINE_H
