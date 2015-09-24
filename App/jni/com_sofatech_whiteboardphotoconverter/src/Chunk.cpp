#include "Prerequisites.h"

using namespace WPC;

double* CreateDistanceVector(long pointCount)
{
	//long** pDistance = new long*[pointCount];
	//for(long i = 0; i < pointCount; i++)
	//{
	//	pDistance[i] = new long[pointCount];
	//	for(long j = 0; j < pointCount; j++)
	//		pDistance[i][j] = i != j ? -1 : 0;
	//}

	//return pDistance;

	double* pDistances = new double[pointCount];
	for(long i = 0; i < pointCount; i++)
		pDistances[i] = std::numeric_limits<double>::infinity();

	return pDistances;
}

bool* CreateVisitedNodesVector(long pointCount)
{
	bool* pVisitedNodes = new bool[pointCount];
	for(long i = 0; i < pointCount; i++)
		pVisitedNodes[i] = false;

	return pVisitedNodes;	
}

long* CreatePreviousNodesVector(long pointCount)
{
	long* pPreviousNodes = new long[pointCount];
	for(long i = 0; i < pointCount; i++)
		pPreviousNodes[i] = -1;

	return pPreviousNodes;	
}

//void DeleteDistanceMatrix(long* pDistance);//, long pointCount)
//{
//	//for(long i = 0; i < pointCount; i++)
//	//	delete[] pDistance[i];
//
//	delete[] pDistance;
//}

long GetNodeWithMinValue(double* pDistances, const std::list<long>& queue)
{
	double minValue = std::numeric_limits<double>::infinity();
	long minIndex = -1;

	std::list<long>::const_iterator it;
	for(it = queue.begin(); it != queue.end(); it++)
	{
		long index = *it;
		double distance = pDistances[index];

		if(distance == std::numeric_limits<double>::infinity())
			continue;

		if(distance < minValue)
		{
			minValue = distance;
			minIndex = index;
		}
	}

	return minIndex;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

Chunk::Chunk(BinaryBitmap* pBitmap)
{
	this->pBitmap = pBitmap;

	minPixel = Point(pBitmap->GetWidth(), pBitmap->GetHeight());
	maxPixel = Point(-1, -1);
}


Chunk::~Chunk(void)
{
}

void Chunk::AddAdjacency(long index0, long index1)
{
	long smallestIndex = index0 < index1 ? index0 : index1;
	long largestIndex = index0 > index1 ? index0 : index1;

	//!!!!we need sorting here (x -> y)
	std::pair<long, long> adjacency(smallestIndex, largestIndex);
	adjacencyList.push_back(adjacency);
}
		
bool Chunk::HasAdjacency(long index0, long index1) const
{
	long smallestIndex = index0 < index1 ? index0 : index1;
	long largestIndex = index0 > index1 ? index0 : index1;

	//!!!!we need use sorting here
	std::vector< std::pair<long, long> >::const_iterator it;
	for(it = adjacencyList.begin(); it != adjacencyList.end(); it++)
	{
		if(it->first == smallestIndex && it->second == largestIndex)
			return true;
	}

	return false;
}

bool Chunk::Contains(const Point& pixel) const
{
	return pixels.end() != std::find(pixels.begin(), pixels.end(), pixel);
}

long Chunk::FindPixel(const Point& pixel) const
{
	///!!!!we need sorting here
	long index = 0;

	std::vector<Point>::const_iterator it;
	for(it = pixels.begin(); it != pixels.end(); it++)
	{
		if(*it == pixel)
			return index;

		index++;
	}

	return -1;
}

void Chunk::FindAndCreatePixelAdjacency(const Point& pixel, long pixelIndex)
{
	for(long x = pixel.x - 1; x <= pixel.x + 1; x++)
	{
		for(long y = pixel.y - 1; y <= pixel.y + 1; y++)
		{
			Point neighbor = Point(x, y);

			if(neighbor == pixel)
				continue;

			long index = FindPixel(neighbor);
			if(index >= 0)
				AddAdjacency(pixelIndex, index);
		}
	}
}

void Chunk::AddPixel(long x, long y)
{
	//!!!!we need sorting (clockwise\kd\...) here
	Point pixel(x, y);
	if(pixels.end() == std::find(pixels.begin(), pixels.end(), pixel))//!!!????not so neccessary to check
	{
		pixels.push_back(pixel);

		if(x > maxPixel.x)
			maxPixel.x = x;
		if(x < minPixel.x)
			minPixel.x = x;
		if(y > maxPixel.y)
			maxPixel.y = y;
		if(y < minPixel.y)
			minPixel.y = y;

		FindAndCreatePixelAdjacency(pixel, pixels.size() - 1);
	}
}

std::vector<Point>::const_iterator Chunk::GetBegin() const
{
	return pixels.begin();
}

std::vector<Point>::const_iterator Chunk::GetEnd() const 
{ 
	return pixels.end(); 
}

double Chunk::GetWeight(long index) const
{
	return pBitmap->GetWeight(pixels[index].x, pixels[index].y);
}

const Chunk Chunk::CreateChunkFromUnion(BinaryBitmap* pBitmap, const QuickUnion& chunkUnion, long chunkIndex)
{
	Chunk chunk(pBitmap);

	//!!!!we need Union method here
	for(long x = 0; x < pBitmap->GetWidth(); x++)
	{
		for(long y = 0; y < pBitmap->GetHeight(); y++)
		{
			if(chunkUnion.HasConnection(pBitmap->GetPixelIndex(x, y), chunkIndex))
				chunk.AddPixel(x, y);
		}
	}

	return chunk;
}

void Chunk::EraseFromBitmap() const
{
	std::vector<Point>::const_iterator it;
	for(it = pixels.begin(); it != pixels.end(); it++)
	{
		pBitmap->SetPixel(it->x, it->y, false);
	}
}

std::list<long> Chunk::GetPathFromPixelToPixel(long startIndex, long endIndex) const
{
	////!!!!!here we need algorithm for undirected graph
	if(startIndex == endIndex)
		return std::list<long>();

	std::list<long> path;
	double* pDistances = CreateDistanceVector(pixels.size());
	bool* pVisitedNodes = CreateVisitedNodesVector(pixels.size());	
	long* pPreviousNodes = CreatePreviousNodesVector(pixels.size());
	
	pDistances[startIndex] = 0;
	//pVisitedNodes[startIndex] = true;

	std::list<long> queue;
	queue.push_back(startIndex);

	while(!queue.empty())
	{
		long currentNode = GetNodeWithMinValue(pDistances, queue);
		queue.remove(currentNode);

		pVisitedNodes[currentNode] = true;

		for(unsigned long neighborIndex = 0; neighborIndex < pixels.size(); neighborIndex++)
		{
			if(!HasAdjacency(neighborIndex, currentNode))
				continue;

			double distance = pDistances[currentNode] + GetDistanceBetweenPoints(neighborIndex, currentNode);
			if(distance < pDistances[neighborIndex] && !pVisitedNodes[neighborIndex])
			{
				pDistances[neighborIndex] = distance;
				pPreviousNodes[neighborIndex] = currentNode;
				queue.push_back(neighborIndex);
			}
		}
	}

	path.push_front(endIndex);

	long previousNode = pPreviousNodes[endIndex];
	while(previousNode != -1)
	{
		path.push_front(previousNode);
		previousNode = pPreviousNodes[previousNode];
	}

	delete[] pPreviousNodes;
	delete[] pVisitedNodes;
	delete[] pDistances;
	//DeleteDistanceMatrix(pDistance)//, points.size());

	return path;
}
		
void Chunk::GetPathLengthesFromPixelToPixel(long index, double* pDistances) const
{
	////!!!!!here we need algorithm for undirected graph
	std::list<long> path;
	bool* pVisitedNodes = CreateVisitedNodesVector(pixels.size());	
	
	for(unsigned long i = 0; i < pixels.size(); i++)
		pDistances[i] = std::numeric_limits<double>::infinity();
	pDistances[index] = 0;

	std::list<long> queue;
	queue.push_back(index);

	while(!queue.empty())
	{
		long currentNode = GetNodeWithMinValue(pDistances, queue);
		queue.remove(currentNode);

		pVisitedNodes[currentNode] = true;

		for(unsigned long neighborIndex = 0; neighborIndex < pixels.size(); neighborIndex++)
		{
			if(!HasAdjacency(neighborIndex, currentNode))
				continue;

			double distance = pDistances[currentNode] + GetDistanceBetweenPoints(neighborIndex, currentNode);
			if(distance < pDistances[neighborIndex] && !pVisitedNodes[neighborIndex])
			{
				pDistances[neighborIndex] = distance;
				queue.push_back(neighborIndex);
			}
		}
	}

	for(unsigned long i = 0; i < pixels.size(); i++)
	{
		if(pDistances[i] == std::numeric_limits<double>::infinity())
			pDistances[i] = -1;
	}
}

double Chunk::GetDistanceBetweenPoints(long index0, long index1) const
{
	Point point0 = pixels[index0];
	Point point1 = pixels[index1];

	///!!!!delete
	//if(point0.first = point1.first && point0.second == point1.second)
	//	return 0;

	//if(point0.first == point1.first)
	//	return abs(point0.second - point1.second);

	//if(point0.second == point1.second)
	//	return abs(point0.first - point1.first);

	double dx = (double)(point0.x - point1.x);
	double dy = (double)(point0.y - point1.y);
	
	return std::sqrt(dx * dx + dy * dy);
}

Rect Chunk::GetRegion() const
{
	return Rect(minPixel, maxPixel); 
}
