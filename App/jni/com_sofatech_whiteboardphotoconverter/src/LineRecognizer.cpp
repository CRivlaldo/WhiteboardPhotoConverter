#include "Prerequisites.h"

using namespace WPC;

//LineRecognizer::PointWeight::PointWeight(long index, double value)
//{
//	this->index = index;
//	this->value = value;
//}
	
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

LineRecognizer::LineRecognizer(const BinaryBitmap* pBitmap, const Chunk* pChunk)
{
	this->pBitmap = pBitmap;
	this->pChunk = pChunk;

	startPointIndex = -1;
	endPointIndex = -1;
}

LineRecognizer::~LineRecognizer(void)
{
}
		
void LineRecognizer::CalcPointWeights()
{
	for(long index = 0; index < pChunk->GetCount(); index++)
	{
		//PointWeight weight(index, pChunk->GetWeight(index));
		//weights.push_back(weight);
		weights.push_back(pChunk->GetWeight(index));
	}
}
		
bool LineRecognizer::FindBestPairOfWeights()
{
	//long maxIndex0 = -1;
	//long maxIndex1 = -1;
	double maxCoefficient = -1;
	double* pDistances = new double[pChunk->GetCount()];

	for(long i = 0; i < pChunk->GetCount(); i++)
	{
		pChunk->GetPathLengthesFromPixelToPixel(i, pDistances);

		for(long j = i + 1; j < pChunk->GetCount(); j++)
		{
			double length = pDistances[j];
			if(length == -1)
				continue;

			double coefficient = length / (weights[i] + weights[j]);

			if(coefficient > maxCoefficient)
			{
				maxCoefficient = coefficient;
				startPointIndex = i;
				endPointIndex = j;
			}
		}

	}
		
	delete[] pDistances;

	if(maxCoefficient == -1)
		return false;

	return true;
}
		
void LineRecognizer::ConstructLine(Line*& pLine)
{
	pLine = new Line();

	std::list<long> path = pChunk->GetPathFromPixelToPixel(startPointIndex, endPointIndex);

	std::list<long>::const_iterator it;
	for(it = path.begin(); it != path.end(); it++)
	{
		pLine->AddControlPoint(pChunk->GetPixel(*it));
	}
}

bool LineRecognizer::TryToRecognizeLineFromChunk(Line*& pLine)
{
	if(pChunk->GetCount() <= 1)
		return false;

	CalcPointWeights();
	if(!FindBestPairOfWeights())
		return false;

	ConstructLine(pLine);
	///!!!!
	//need to recongize arrows

	return true;
}

bool LineRecognizer::TryToRecognizeLineFromChunk(BinaryBitmap* pBitmap, const Chunk& chunk, Line*& pLine)
{
	LineRecognizer lineRecognizer(pBitmap, &chunk);
	return lineRecognizer.TryToRecognizeLineFromChunk(pLine);
}
