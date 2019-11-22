#include "kernel.h"





__kernel void videoFilter(__global uchar *restrict inOut, __global uchar *restrict tempA, __global uchar *restrict tempB)
{

	unsigned const xmid = get_global_id(0);
	unsigned const ymid = get_global_id(1) * W;
	
	unsigned inIndex = (xmid+ymid)*3;
	tempA[xmid+ymid] = ((ushort)inOut[inIndex] + inOut[inIndex+1] + inOut[inIndex+2]) / 3;



	mem_fence(CLK_GLOBAL_MEM_FENCE);

	unsigned ylow = ymid - W;
	unsigned yhigh = ymid + W;
	unsigned xlow = xmid - 1;
	unsigned xhigh = xmid + 1;
	if (ylow > ymid) 
		ylow = 0;
	if (yhigh > (H-1) * (W-1))
		yhigh = (H-1) * (W-1);
	if (xlow > xmid) 
		xlow = 0;
	if (xhigh > (W-1))
		xhigh = (W-1);
	
	tempB[xmid+ymid] = (((ushort)tempA[ylow+xlow] + tempA[ylow+xhigh] + tempA[yhigh+xlow] + tempA[yhigh+xhigh])
				      + ((ushort)tempA[ymid+xlow] + tempA[ymid+xhigh] + tempA[ylow+xmid] + tempA[yhigh+xmid]) * 2
				      +  (ushort)tempA[ymid+xmid] * 4) / 16;


	mem_fence(CLK_GLOBAL_MEM_FENCE);	

	tempA[xmid+ymid] = (((ushort)tempB[ylow+xlow] + tempB[ylow+xhigh] + tempB[yhigh+xlow] + tempB[yhigh+xhigh])
					 + ((ushort)tempB[ymid+xlow] + tempB[ymid+xhigh] + tempB[ylow+xmid] + tempB[yhigh+xmid]) * 2
					 +  (ushort)tempB[ymid+xmid] * 4) / 16;


	mem_fence(CLK_GLOBAL_MEM_FENCE);


	tempB[xmid+ymid] = (((ushort)tempA[ylow+xlow] + tempA[ylow+xhigh] + tempA[yhigh+xlow] + tempA[yhigh+xhigh])
					  + ((ushort)tempA[ymid+xlow] + tempA[ymid+xhigh] + tempA[ylow+xmid] + tempA[yhigh+xmid]) * 2
					  +  (ushort)tempA[ymid+xmid] * 4) / 16;


	mem_fence(CLK_GLOBAL_MEM_FENCE);


	short Gx = ((short)tempB[xhigh+ylow] + (short)tempB[xhigh+yhigh] - (short)tempB[xlow+ylow] - (short)tempB[xlow+yhigh]) * 3 + ((short)tempB[xhigh+ymid] - (short)tempB[xlow+ymid]) * 10;
	short Gy = ((short)tempB[xlow+yhigh] + (short)tempB[xhigh+yhigh] - (short)tempB[xlow+ylow] - (short)tempB[xhigh+ylow]) * 3 + ((short)tempB[xmid+yhigh] - (short)tempB[xmid+ylow]) * 10;

	Gx = Gx > 0 ? Gx : 0;
	Gy = Gy > 0 ? Gy : 0;

	unsigned outIndex = (xmid+ymid) * 3;
	inOut[outIndex] = inOut[outIndex+1] = inOut[outIndex+2] = Gx + Gy > 160 ? 0 : tempB[xmid+ymid];
}

