#pragma once

namespace WPC
{
	class QuickUnion
	{
	private:
		long size;
		long* pElementIDs;
		long unionCount;
		
		static void Copy(const QuickUnion& srcBitmap, QuickUnion& dstBitmap);
	public:
		QuickUnion(long size);
		~QuickUnion(void);

		long GetSize() const { return size; }
		long GetUnionCount() const { return unionCount; }

		long FindUnionIndex(int index) const;
		void Union(long index0, long index1);
		void Disconnect(long index);
		bool HasConnection(long index0, long index1) const;
		void Reset();

		QuickUnion& operator=(const QuickUnion& srcUnion);
	};
}