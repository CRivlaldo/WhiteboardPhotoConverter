#pragma once

namespace WPC
{
	class Chart
	{
	public:
		class Node;

		class Link
		{
		public:
			enum LinkType ///!!!!do we really need this?
			{
				LinkType_Sibling,
				LinkType_ChildParent
			};

		private:
			Node* pFirst;
			Node* pSecond;
			LinkType linkType;
			Line* pLine;
						
			Link(Node* pFirst, Node* pSecond, LinkType linkType, Line* pLine);

		public:
			~Link();

			const Line* GetLine() const { return pLine; }

			static Link* CreateSiblingLink(Node* pFirst, Node* pSecond, Line* pLine);
			static Link* CreateChildParentLink(Node* pFirst, Node* pSecond, Line* pLine);
		};

		class Node
		{
		private:
			Shape* pShape;
			std::string text;

		public:
			Node(Shape* pShape, const std::string& text);
			~Node();

			const Shape* GetShape() const { return pShape; }
			const std::string& GetText() const { return text; } 
		};

	private:
		std::vector<Node*> nodes;
		std::vector<Link*> links;
		long width, height;

		Node* GetNodeByShape(Shape* pShape);

	public:
		Chart(long width, long height);
		~Chart(void);

		void AddNode(Shape* pShape, const std::string& text);
		void AddSiblingLink(Shape* pFirstShape, Shape* pSecondShape, Line* pLine);
		void AddChildParentLink(Shape* pFirstShape, Shape* pSecondShape, Line* pLine);

		long GetWidth() const { return width; }
		long GetHeight() const { return height; }

		std::vector<Node*>::const_iterator GetNodesBegin() const { return nodes.begin(); }
		std::vector<Node*>::const_iterator GetNodesEnd() const { return nodes.end(); }

		std::vector<Link*>::const_iterator GetLinksBegin() const { return links.begin(); }
		std::vector<Link*>::const_iterator GetLinksEnd() const { return links.end(); }
	};
}

