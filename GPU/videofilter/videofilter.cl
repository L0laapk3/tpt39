#include "kernel.h"





__kernel void toGrey(__global unsigned char *restrict in, __global unsigned char *restrict out)
{

	unsigned const x = (get_global_id(0)+borderSize);
	unsigned const y = (get_global_id(1)+borderSize) * W;
	
	unsigned inIndex = (x+y)*3;
	out[x+y] = ((ushort)in[inIndex] + in[inIndex+1] + in[inIndex+2]) / 3;
}



__kernel void gaussianBlur(__global const unsigned char *restrict in, __global unsigned char *restrict	 out)
{

	unsigned const xmid = (get_global_id(0)+borderSize);
	unsigned const ymid = (get_global_id(1)+borderSize) * W;
 	// printf("doing w %d h %d.\n", w, h);

	ushort total = 0;

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
	
	out[xmid+ymid] = (((ushort)in[ylow+xlow] + in[ylow+xhigh] + in[yhigh+xlow] + in[yhigh+xhigh])
					+ ((ushort)in[ymid+xlow] + in[ymid+xhigh] + in[ylow+xmid] + in[yhigh+xmid]) * 2
					+  (ushort)in[ymid+xmid] * 4) / 16;
}





__kernel void edgeDetect(__global const unsigned char *restrict in, __global unsigned char *restrict out)
{


	unsigned const xmid = (get_global_id(0)+borderSize);
	unsigned const ymid = (get_global_id(1)+borderSize) * W;

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



	short Gx = ((short)in[xhigh+ylow] + (short)in[xhigh+yhigh] - (short)in[xlow+ylow] - (short)in[xlow+yhigh]) * 3 + ((short)in[xhigh+ymid] - (short)in[xlow+ymid]) * 10;
	short Gy = ((short)in[xlow+yhigh] + (short)in[xhigh+yhigh] - (short)in[xlow+ylow] - (short)in[xhigh+ylow]) * 3 + ((short)in[xmid+yhigh] - (short)in[xmid+ylow]) * 10;

	Gx = Gx > 0 ? Gx : 0;
	Gy = Gy > 0 ? Gy : 0;

	unsigned outIndex = (xmid+ymid) * 3;
	out[outIndex] = out[outIndex+1] = out[outIndex+2] = Gx + Gy > 160 ? 0 : in[xmid+ymid];

}

