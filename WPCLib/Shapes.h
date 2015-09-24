namespace WPC
{
	struct TextGlyph
	{
		Rect rect;
		std::string text;
		int confidence;

		TextGlyph(const Rect& rect, const std::string& text, const double confidence);
	};

	inline bool CompareConfidences(const TextGlyph& textA, const TextGlyph& textB)
	{
		return textA.confidence < textB.confidence;
	}

	inline bool CompareHeights(const TextGlyph& textA, const TextGlyph& textB)
	{
		return textA.rect.GetMin().y > textB.rect.GetMin().y;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////

	class Shape
	{
	public:
		enum ShapeType
		{
			ShapeType_None,
			ShapeType_Rect,
			ShapeType_FreeText,
			ShapeType_Ellipse,
			ShapeType_Count
		};

	protected:
		ShapeType shapeType;
		Point origin;
		long blockSize;

	public:
		Shape(ShapeType shapeType, const Point& origin, long blockSize);

		Point GetOrigin() const { return origin; }
		Point GetDIBOrigin() const { return origin * blockSize; }
		double GetMatch(BinaryBitmap* pBinaryBitmap) const ;
		ShapeType GetShapeType() const { return shapeType; }
		
		virtual void FillBinaryBitmap(BinaryBitmap& binaryBitmap) const = 0;
		virtual Rect GetRegion() const = 0;
		//virtual bool FindIntersection(const std::vector<double>& curveEquationCoefficients, 
			//const std::pair<long, long>& endPoint, std::pair<long, long>& intersectionPoint) const = 0;
		virtual bool HasIntersection(const Point& point) const = 0;
		virtual bool Contains(const Point& point) const = 0;
		virtual bool FindNearestPoint(const Point& fromPoint, Point& intersectionPoint) const = 0; 
		virtual Shape* Extract() const = 0;

		static bool IsShapeForMatching(ShapeType shapeType);
	};

	class RectShape: public Shape
	{
	private:
		long width;
		long height;

	public:
		RectShape(const Point& origin, long width, long height, long blockSize);

		long GetWidth() const { return width; }
		long GetHeight() const { return height; }
		long GetDIBWidth() const { return width * blockSize; }
		long GetDIBHeight() const { return height * blockSize; }
		void FillBinaryBitmap(BinaryBitmap& binaryBitmap) const;
		Rect GetRegion() const;
		bool FindNearestPoint(const Point& fromPoint, Point& intersectionPoint) const; 
		/*bool FindIntersection(const std::vector<double>& curveEquationCoefficients, 
			const std::pair<long, long>& endPoint, std::pair<long, long>& intersectionPoint) const;*/
		bool HasIntersection(const Point& point) const;
		bool Contains(const Point& point) const;
		Shape* Extract() const;
	};

	class FreeText: public RectShape
	{
	private:
		std::string text;

	public:
		FreeText(const Point& origin, long width, long height, long blockSize, const std::string& text);
		FreeText(const TextGlyph& textGlyph);

		std::string GetText() { return text; }
	};

	class EllipseShape: public Shape
	{
	private:
		double radiusX;
		double radiusY;

		void UpdateMinLength(const Point& fromPoint, const Point& localPoint, long& minLength,
			Point& intersectionPoint) const;

	public:
		EllipseShape(const Point& origin, double radiusX, double radiusY, long blockSize);

		double GetRadiusX() const { return radiusX; }
		double GetRadiusY() const { return radiusY; }
		double GetDIBRadiusX() const { return radiusX * blockSize; }
		double GetDIBRadiusY() const { return radiusY * blockSize; }
		void FillBinaryBitmap(BinaryBitmap& binaryBitmap) const;
		Rect GetRegion() const;
		bool FindNearestPoint(const Point& fromPoint, Point& intersectionPoint) const;
		/*bool FindIntersection(const std::vector<double>& curveEquationCoefficients, 
			const std::pair<long, long>& endPoint, std::pair<long, long>& intersectionPoint) const; */		
		bool HasIntersection(const Point& point) const;
		bool Contains(const Point& point) const;
		Shape* Extract() const;
	};
}