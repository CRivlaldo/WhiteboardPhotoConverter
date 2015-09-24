#pragma once

namespace WPC
{
	class LineRecognizer
	{
	private:
		//struct PointWeight
		//{
		//	long index;
		//	double value;

		//	PointWeight(long index, double value);
		//};

		//class PointWeightComparator
		//{
		//public:
		//	static bool PointWeightPredicate(const PointWeight& weight0, const PointWeight& weight1)
		//	{
		//		return weight0.value < weight1.value;
		//	}

		//	bool operator()(PointWeight x, PointWeight y) const
		//	{     
		//		return x.value < y.value;
		//	}
		//};

	private:
		const BinaryBitmap* pBitmap;
		const Chunk *pChunk;
		//std::vector<PointWeight> weights;
		std::vector<double> weights;
		long startPointIndex;
		long endPointIndex;

		bool TryToRecognizeLineFromChunk(Line*& pLine);
		void CalcPointWeights();
		bool FindBestPairOfWeights();
		void ConstructLine(Line*& pLine);

	public:
		LineRecognizer(const BinaryBitmap* pBitmap, const Chunk* pChunk);
		~LineRecognizer(void);

		static bool TryToRecognizeLineFromChunk(BinaryBitmap* pBitmap, const Chunk& chunk, Line*& pLine);
	};
}

