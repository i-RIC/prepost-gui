#ifndef WINDOWWITHOBJECTBROWSER_H
#define WINDOWWITHOBJECTBROWSER_H

class ObjectBrowser;

class WindowWithObjectBrowser
{
public:
	virtual ObjectBrowser* objectBrowser() = 0;
};

#endif // WINDOWWITHOBJECTBROWSER_H
