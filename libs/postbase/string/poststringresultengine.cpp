#include "poststringresultengine.h"
#include "poststringresultsettingcontainer.h"
#include "private/poststringresultengine_argument.h"
#include "private/poststringresultengine_impl.h"

#include <QMessageBox>

PostStringResultEngine::PostStringResultEngine() :
	impl {new Impl {}}
{}

PostStringResultEngine::~PostStringResultEngine()
{
	delete impl;
}

bool PostStringResultEngine::test(const PostStringResultSettingContainer& c, QString* result, QWidget* w)
{
	QJSValue func;
	std::vector<Argument> args;

	bool ok = build(c, &func, &args, w);
	if (! ok) {
		return false;
	}

	QJSValueList jsArgs;
	for (auto a : args) {
		jsArgs << impl->m_jsEngine.evaluate(a.valueForTest());
	}
	auto value = func.call(jsArgs);

	if (value.isError()) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured: %1").arg(value.toString()));
		return false;
	}

	if (result != nullptr) {
		*result = value.toString();
	}
	return true;
}

void PostStringResultEngine::setSetting(const PostStringResultSettingContainer& container)
{
	impl->m_arguments.clear();

	build(container, &impl->m_function, &impl->m_arguments, nullptr);
}

QString PostStringResultEngine::getValue(v4PostZoneDataContainer* container) const
{
	QJSValueList jsArgs;
	for (auto a : impl->m_arguments) {
		jsArgs << a.value(container);
	}

	return impl->m_function.call(jsArgs).toString();
}

bool PostStringResultEngine::build(const PostStringResultSettingContainer& c, QJSValue* func, std::vector<PostStringResultEngine::Argument>* args, QWidget* w)
{
	QStringList refNames;
	for (const auto& arg : c.arguments) {
		Argument a(arg);
		args->push_back(a);

		refNames.push_back(arg.refName);
	}

	QString funcStr;

	funcStr.append("(function(").append(refNames.join(",").append(") {"));
	funcStr.append(c.script);
	funcStr.append("})");
	*func = impl->m_jsEngine.evaluate(funcStr);

	if (func->isError()) {
		QMessageBox::critical(w, tr("Error"), tr("Error occured: %1").arg(func->toString()));
		return false;
	}

	return true;
}
