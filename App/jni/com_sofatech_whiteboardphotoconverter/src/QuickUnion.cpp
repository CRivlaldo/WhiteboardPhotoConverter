#include "Prerequisites.h"

using namespace WPC;

QuickUnion::QuickUnion(long size)
{
	this->size = size;

	pElementIDs = new long[size];
	for(long i = 0; i < size; i++)
	{
		pElementIDs[i] = i;
	}

	unionCount = size;
}

QuickUnion::~QuickUnion(void)
{
	delete pElementIDs;
}

long QuickUnion::FindUnionIndex(int index) const
{
	while(index != pElementIDs[index])
		index = pElementIDs[index];

	return index;
}

void QuickUnion::Union(long index0, long index1)
{
	long rootIndex0 = FindUnionIndex(index0);
	long rootIndex1 = FindUnionIndex(index1);
	if(rootIndex0 == rootIndex1)
		return;

	pElementIDs[rootIndex0] = rootIndex1;
	unionCount--;
}

void QuickUnion::Disconnect(long index)
{
	long rootIndex = FindUnionIndex(index);

	if(rootIndex == index)
	{
		for(long i = 0; i < size; i++)
		{
			if(pElementIDs[i] == index && i != index)
			{
				rootIndex = i;
				break;
			}
		}
	}
	
	pElementIDs[index] = index;
	for(long i = 0; i < size; i++)
	{
		if(pElementIDs[i] == index && i != index)
			pElementIDs[i] = rootIndex;
	}

	unionCount++;
}

bool QuickUnion::HasConnection(long index0, long index1) const
{
	return FindUnionIndex(index0) == FindUnionIndex(index1);
}

void QuickUnion::Copy(const QuickUnion& srcUnion, QuickUnion& dstUnion)
{
	dstUnion.size = srcUnion.size;
	dstUnion.unionCount =  srcUnion.unionCount;

	dstUnion.pElementIDs = new long[srcUnion.size];

	for(long i = 0; i < srcUnion.size; i++)
	{
		dstUnion.pElementIDs[i] = srcUnion.pElementIDs[i];
	}
}

QuickUnion& QuickUnion::operator=(const QuickUnion& srcUnion)
{
	Copy(srcUnion, *this);
	return *this;
}

void QuickUnion::Reset()
{
	for(long i = 0; i < size; i++)
	{
		pElementIDs[i] = i;
	}

	unionCount = size;
}