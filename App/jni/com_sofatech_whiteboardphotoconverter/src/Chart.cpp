#include "Prerequisites.h"

using namespace WPC;

Chart::Link::Link(Node* pFirst, Node* pSecond, LinkType linkType, Line* pLine)
{
	this->pFirst = pFirst;
	this->pSecond = pSecond;
	this->linkType = linkType;
	this->pLine = pLine;
}
			
Chart::Link* Chart::Link::CreateSiblingLink(Node* pFirst, Node* pSecond, Line* pLine)
{
	return new Link(pFirst, pSecond, LinkType_Sibling, pLine);
}
			
Chart::Link* Chart::Link::CreateChildParentLink(Node* pFirst, Node* pSecond, Line* pLine)
{
	return new Link(pFirst, pSecond, LinkType_Sibling, pLine);
}

Chart::Link::~Link()
{
	delete pLine;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

Chart::Node::Node(Shape* pShape, const std::string& text)
{
	this->pShape = pShape;
	this->text = text;
}

Chart::Node::~Node()
{
	delete pShape;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

Chart::Chart(long width, long height)
{
	this->width = width;
	this->height = height;
}


Chart::~Chart(void)
{
	std::vector<Link*>::iterator linkterator;
	for(linkterator = links.begin(); linkterator != links.end(); linkterator++)
	{
		delete *linkterator;
	}

	std::vector<Node*>::iterator nodeIterator;
	for(nodeIterator = nodes.begin(); nodeIterator != nodes.end(); nodeIterator++)
	{
		delete *nodeIterator;
	}
}
		
void Chart::AddNode(Shape* pShape, const std::string& text)
{
	Node* pNode = new Node(pShape, text);
	nodes.push_back(pNode);
}
		
void Chart::AddSiblingLink(Shape* pFirstShape, Shape* pSecondShape, Line* pLine)
{
	Node* pFirstNode = GetNodeByShape(pFirstShape);
	Node* pSecondNode = GetNodeByShape(pSecondShape);

	Link* pLink = Link::CreateSiblingLink(pFirstNode, pSecondNode, pLine);
	links.push_back(pLink);
}
		
void Chart::AddChildParentLink(Shape* pFirstShape, Shape* pSecondShape, Line* pLine)
{
	Node* pFirstNode = GetNodeByShape(pFirstShape);
	Node* pSecondNode = GetNodeByShape(pSecondShape);

	Link* pLink = Link::CreateChildParentLink(pFirstNode, pSecondNode, pLine);
	links.push_back(pLink);
}

Chart::Node* Chart::GetNodeByShape(Shape* pShape)
{
	std::vector<Node*>::iterator nodeIterator;
	for(nodeIterator = nodes.begin(); nodeIterator != nodes.end(); nodeIterator++)
	{
		Node* pNode = *nodeIterator;
		if(pShape == pNode->GetShape())
			return pNode;
	}

	return NULL;
}