#include "gdalutil.h"

#include <string>
#include <vector>

namespace {

class Node {
public:
	Node();
	~Node();

	std::string name;
	std::vector<Node*> childNodes;
};

Node::Node()
{}

Node::~Node()
{
	for (auto n : childNodes) {
		delete n;
	}
}

void buildNodeTree(char*& c, Node* node) {
	bool inQuat = false;
	bool childMode = false;

	for (;;) {
		if (childMode) {
			if (*c == ']') {
				++ c;
				return;
			} else if (*c == ',') {
				auto childNode = new Node();
				node->childNodes.push_back(childNode);
				++ c;
				buildNodeTree(c, childNode);
			}
		} else {
			if (*c == '\0') {
				return;
			} else if (*c == ',') {
				return;
			} else if (*c == '[') {
				auto childNode = new Node();
				node->childNodes.push_back(childNode);
				++ c;
				buildNodeTree(c, childNode);

				childMode = true;
				continue;
			} else if (*c == ']') {
				return;
			} else if (*c == '\"') {
				inQuat = ! inQuat;
			} else {
				char buffer[2];
				buffer[0] = *c;
				buffer[1] = '\0';
				node->name.append(&(buffer[0]));
			}
			++ c;
		}
	}
}

Node* buildNodeTree(char* text)
{
	Node* node = new Node();
	buildNodeTree(text, node);
	return node;
}

} // namespace

int GdalUtil::wkt2Epsg(const char* wkt)
{
	char* text = const_cast<char*>(wkt);
	Node* node = buildNodeTree(text);
	for (Node* c : node->childNodes) {
		if (c->name == "AUTHORITY") {
			return atoi(c->childNodes[1]->name.c_str());
		}
	}

	return 0;
}
