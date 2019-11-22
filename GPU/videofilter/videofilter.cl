#include "kernel.h"





__kernel void videoFilter(__global uchar *restrict inOut, __global uchar *restrict temp)
{

	unsigned const xmid = get_global_id(0);
	unsigned const ymid = get_global_id(1) * W;
	
	unsigned inIndex = (xmid+ymid)*3;
	temp[xmid+ymid] = ((ushort)inOut[inIndex] + inOut[inIndex+1] + inOut[inIndex+2]) / 3;



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
	
	inOut[xmid+ymid] = (((ushort)temp[ylow+xlow] + temp[ylow+xhigh] + temp[yhigh+xlow] + temp[yhigh+xhigh])
				      + ((ushort)temp[ymid+xlow] + temp[ymid+xhigh] + temp[ylow+xmid] + temp[yhigh+xmid]) * 2
				      +  (ushort)temp[ymid+xmid] * 4) / 16;


	mem_fence(CLK_GLOBAL_MEM_FENCE);	

	temp[xmid+ymid] = (((ushort)inOut[ylow+xlow] + inOut[ylow+xhigh] + inOut[yhigh+xlow] + inOut[yhigh+xhigh])
					 + ((ushort)inOut[ymid+xlow] + inOut[ymid+xhigh] + inOut[ylow+xmid] + inOut[yhigh+xmid]) * 2
					 +  (ushort)inOut[ymid+xmid] * 4) / 16;


	mem_fence(CLK_GLOBAL_MEM_FENCE);

	inOut[(xmid+ymid)] = inOut[(xmid+ymid)*3+1] = inOut[(xmid+ymid)*3+2] = 255 - temp[xmid+ymid];


	// inOut[(xmid+ymid)*3] = inOut[(xmid+ymid)*3+1] = inOut[(xmid+ymid)*3+2] = (((ushort)temp[ylow+xlow] + temp[ylow+xhigh] + temp[yhigh+xlow] + temp[yhigh+xhigh])
	// 				  + ((ushort)temp[ymid+xlow] + temp[ymid+xhigh] + temp[ylow+xmid] + temp[yhigh+xmid]) * 2
	// 				  +  (ushort)temp[ymid+xmid] * 4) / 16;


	// mem_fence(CLK_GLOBAL_MEM_FENCE);


	// short Gx = ((short)temp[xhigh+ylow] + (short)temp[xhigh+yhigh] - (short)temp[xlow+ylow] - (short)temp[xlow+yhigh]) * 3 + ((short)temp[xhigh+ymid] - (short)temp[xlow+ymid]) * 10;
	// short Gy = ((short)temp[xlow+yhigh] + (short)temp[xhigh+yhigh] - (short)temp[xlow+ylow] - (short)temp[xhigh+ylow]) * 3 + ((short)temp[xmid+yhigh] - (short)temp[xmid+ylow]) * 10;

	// Gx = Gx > 0 ? Gx : 0;
	// Gy = Gy > 0 ? Gy : 0;

	// unsigned outIndex = (xmid+ymid) * 3;
	// inOut[outIndex] = inOut[outIndex+1] = inOut[outIndex+2] = Gx + Gy > 160 ? 0 : temp[xmid+ymid];
}

