#include "kernel.h"





__kernel void toGrey(__global unsigned char *restrict in)
{

	unsigned const x = (get_global_id(0)+borderSize)*3;
	unsigned const y = (get_global_id(1)+borderSize)*3 * W;
	
	in[x+y] = ((ushort)in[x+y] + in[x+y+1] + in[x+y+2]) / 3;
}



__kernel void gaussianBlur(__global const unsigned char * in, __global unsigned char * out)
{

	unsigned const xmid = (get_global_id(0)+borderSize)*3;
	unsigned const ymid = (get_global_id(1)+borderSize)*3 * W;
 	// printf("doing w %d h %d.\n", w, h);

	ushort total = 0;

	unsigned ylow = ymid - W*3;
	unsigned yhigh = ymid + W*3;
	unsigned xlow = xmid - 3;
	unsigned xhigh = xmid + 3;
	if (ylow > ymid) 
		ylow = 0;
	if (yhigh > (H-1) * (W-1) * 3)
		yhigh = (H-1) * (W-1) * 3;
	if (xlow > xmid) 
		xlow = 0;
	if (xhigh > (W-1) * 3)
		xhigh = (W-1) * 3;
	
	out[xmid+ymid] = (((ushort)in[ylow+xlow] + in[ylow+xhigh] + in[yhigh+xlow] + in[yhigh+xhigh])
					+ ((ushort)in[ymid+xlow] + in[ymid+xhigh] + in[ylow+xmid] + in[yhigh+xmid]) * 2
					+  (ushort)in[ymid+xmid] * 4) / 16;


	// out[xmid+ymid] = in[xmid+ymid];







	// out[w + h * W] = ((ushort)(in[w - 1 + (h - 1) * W] * corner) + in[w + (h - 1) * W] * edge   + in[w + 1 + (h - 1) * W] * corner
	// 			   			 + in[w - 1 +  h      * W] * edge   + in[w +  h      * W] * center + in[w + 1 +  h      * W] * edge
	// 			   			 + in[w - 1 + (h + 1) * W] * corner + in[w + (h + 1) * W] * edge   + in[w + 1 + (h + 1) * W] * corner) / normalised;
	
	// ushort total = 0;
	// int test = 0;

	// // unsigned char gaussKernelIndex = 0;
	// // unsigned inputIndex = w - blurIterations + (h - blurIterations) * W;
	// // for (int i = -blurIterations; i <= blurIterations; i++) {
	
	// // 	for (int j = -blurIterations; j <= blurIterations; j++)
	// // 		total += (ushort)in[inputIndex++] * gaussKernel[gaussKernelIndex++];
	// // 		// total += (ushort)in[w + i + (h + j) * W] * gaussKernel[i + blurIterations + (j + blurIterations) * ksize];

	// // 	inputIndex += W - ksize - 1;

	// // }
	// //printf("%d\n", test);

	// out[w + h * W] = total >> 8;
}





__kernel void edgeDetect(__global const unsigned char *restrict in, __global unsigned char *restrict out)
{


	unsigned const xmid = (get_global_id(0)+borderSize)*3;
	unsigned const ymid = (get_global_id(1)+borderSize)*3 * W;

	unsigned ylow = ymid - W*3;
	unsigned yhigh = ymid + W*3;
	unsigned xlow = xmid - 3;
	unsigned xhigh = xmid + 3;
	if (ylow > ymid) 
		ylow = 0;
	if (yhigh > (H-1) * (W-1) * 3)
		yhigh = (H-1) * (W-1) * 3;
	if (xlow > xmid) 
		xlow = 0;
	if (xhigh > (W-1) * 3)
		xhigh = (W-1) * 3;



	short Gx = ((short)in[xhigh+ylow] + (short)in[xhigh+yhigh] - (short)in[xlow+ylow] - (short)in[xlow+yhigh]) * 3 + ((short)in[xhigh+ymid] - (short)in[xlow+ymid]) * 10;
	short Gy = ((short)in[xlow+yhigh] + (short)in[xhigh+yhigh] - (short)in[xlow+ylow] - (short)in[xhigh+ylow]) * 3 + ((short)in[xmid+yhigh] - (short)in[xmid+ylow]) * 10;

	Gx = Gx > 0 ? Gx : 0;
	Gy = Gy > 0 ? Gy : 0;

	out[xmid+ymid] = out[xmid+ymid+1] = out[xmid+ymid+2] = Gx + Gy > 160 ? 0 : in[xmid+ymid];

}

