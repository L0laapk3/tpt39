#include <stdio.h>
#include <stdlib.h>
#include <iostream> // for standard I/O
#include <fstream>
#include <time.h>
#include <CL/cl.h>
#include <CL/cl_ext.h>
#include "opencv2/opencv.hpp"
#define STRING_BUFFER_LEN 1024


#define BILLION  1000000000L

using namespace cv;
using namespace std;
//#define SHOW



void print_clbuild_errors(cl_program program,cl_device_id device)
{
	cout<<"Program Build failed\n";
	size_t length;
	char buffer[2048];
	clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &length);
	cout<<"--- Build log ---\n "<<buffer<<endl;
	exit(1);
}


void callback(const char *buffer, size_t length, size_t final, void *user_data)
{
    fwrite(buffer, 1, length, stdout);
}

const char *getErrorString(cl_int error)
{
switch(error){
    // run-time and JIT compiler errors
    case 0: return "CL_SUCCESS";
    case -1: return "CL_DEVICE_NOT_FOUND";
    case -2: return "CL_DEVICE_NOT_AVAILABLE";
    case -3: return "CL_COMPILER_NOT_AVAILABLE";
    case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
    case -5: return "CL_OUT_OF_RESOURCES";
    case -6: return "CL_OUT_OF_HOST_MEMORY";
    case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
    case -8: return "CL_MEM_COPY_OVERLAP";
    case -9: return "CL_IMAGE_FORMAT_MISMATCH";
    case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
    case -11: return "CL_BUILD_PROGRAM_FAILURE";
    case -12: return "CL_MAP_FAILURE";
    case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
    case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
    case -15: return "CL_COMPILE_PROGRAM_FAILURE";
    case -16: return "CL_LINKER_NOT_AVAILABLE";
    case -17: return "CL_LINK_PROGRAM_FAILURE";
    case -18: return "CL_DEVICE_PARTITION_FAILED";
    case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";

    // compile-time errors
    case -30: return "CL_INVALID_VALUE";
    case -31: return "CL_INVALID_DEVICE_TYPE";
    case -32: return "CL_INVALID_PLATFORM";
    case -33: return "CL_INVALID_DEVICE";
    case -34: return "CL_INVALID_CONTEXT";
    case -35: return "CL_INVALID_QUEUE_PROPERTIES";
    case -36: return "CL_INVALID_COMMAND_QUEUE";
    case -37: return "CL_INVALID_HOST_PTR";
    case -38: return "CL_INVALID_MEM_OBJECT";
    case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
    case -40: return "CL_INVALID_IMAGE_SIZE";
    case -41: return "CL_INVALID_SAMPLER";
    case -42: return "CL_INVALID_BINARY";
    case -43: return "CL_INVALID_BUILD_OPTIONS";
    case -44: return "CL_INVALID_PROGRAM";
    case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
    case -46: return "CL_INVALID_KERNEL_NAME";
    case -47: return "CL_INVALID_KERNEL_DEFINITION";
    case -48: return "CL_INVALID_KERNEL";
    case -49: return "CL_INVALID_ARG_INDEX";
    case -50: return "CL_INVALID_ARG_VALUE";
    case -51: return "CL_INVALID_ARG_SIZE";
    case -52: return "CL_INVALID_KERNEL_ARGS";
    case -53: return "CL_INVALID_WORK_DIMENSION";
    case -54: return "CL_INVALID_WORK_GROUP_SIZE";
    case -55: return "CL_INVALID_WORK_ITEM_SIZE";
    case -56: return "CL_INVALID_GLOBAL_OFFSET";
    case -57: return "CL_INVALID_EVENT_WAIT_LIST";
    case -58: return "CL_INVALID_EVENT";
    case -59: return "CL_INVALID_OPERATION";
    case -60: return "CL_INVALID_GL_OBJECT";
    case -61: return "CL_INVALID_BUFFER_SIZE";
    case -62: return "CL_INVALID_MIP_LEVEL";
    case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
    case -64: return "CL_INVALID_PROPERTY";
    case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
    case -66: return "CL_INVALID_COMPILER_OPTIONS";
    case -67: return "CL_INVALID_LINKER_OPTIONS";
    case -68: return "C${GCC}L_INVALID_DEVICE_PARTITION_COUNT";

    // extension errors
    case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
    case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
    case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
    case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
    case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
    case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
    default: return "Unknown OpenCL error";
    }
}
unsigned char ** read_file(const char *name) {
	size_t size;
	unsigned char **output=(unsigned char **)malloc(sizeof(unsigned char *));
	FILE* fp = fopen(name, "rb");
	if (!fp) {
		printf("no such file:%s",name);
		exit(-1);
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	*output = (unsigned char *)malloc(size);
	unsigned char **outputstr=(unsigned char **)malloc(sizeof(unsigned char *));
	*outputstr= (unsigned char *)malloc(size);
	if (!*output) {
		fclose(fp);
		printf("mem allocate failure:%s",name);
		exit(-1);
	}

	if(!fread(*output, size, 1, fp)) printf("failed to read file\n");
		fclose(fp);
	// printf("file size %d\n",size);
	// printf("-------------------------------------------\n");
	snprintf((char *)*outputstr,size,"%s\n",*output);
	// printf("%s\n",*outputstr);
	// printf("-------------------------------------------\n");
	return outputstr;
}


void checkError(int status, const char *msg) {
    if(status!=CL_SUCCESS)
        printf("%s: %s\n",msg,getErrorString(status));
}



#define blurIterations 1
void generateGaussianKernelFile(Size resolution) {
	unsigned int ksize = 1 + blurIterations * 2;
	Mat kernelTest(ksize+2, ksize+2, CV_16UC1, Scalar(0));
	kernelTest.at<ushort>(blurIterations+1, blurIterations+1) = 0xFFFF;

	imwrite("beforeblur.jpg", kernelTest);
	for (int i = 0; i < blurIterations; i++)
		GaussianBlur(kernelTest, kernelTest, Size(3,3),0,0);

	cv::Rect cropRect;
	cropRect.x = 1;
	cropRect.y = 1;
	cropRect.width = ksize;
	cropRect.height = ksize;

	kernelTest = kernelTest(cropRect);

	imwrite("afterblur.jpg", kernelTest);
	kernelTest.convertTo(kernelTest, CV_8UC1, 1/256.0);

	ofstream kernelFile("kernel.h");

	kernelFile << "#define blurIterations " << blurIterations << "\n";
	kernelFile << "#define borderSize " << borderSize << "\n";
	kernelFile << "#define W " << resolution.width << "\n";
	kernelFile << "#define H " << resolution.height << "\n";
	kernelFile << "#define ksize " << ksize << "\n";
	//kernelFile << "const static unsigned H = " << resolution.height << ";\n";
	//kernelFile << "const static unsigned BW = W + 2 * blurIterations;\n";
	//kernelFile << "const static unsigned BH = H + 2 * blurIterations;\n\n";

	kernelFile << "#define gaussKernelValues ";

	
	uint sum = 0;
	for (uint i = 0; i < ksize*ksize; i += ksize) {
		for (uint j = 0; j < ksize; j++) {
			kernelFile << (uint)kernelTest.data[i+j] << ", ";
			printf("%02d ", kernelTest.data[i+j]);
			sum += kernelTest.data[i+j];
		}
		cout << "\n";
	}
	printf("sum %d\n", sum);

	kernelFile << "\n";
	//kernelFile << "const static unsigned short normalisationFactor = " << sum << ";\n";

	kernelFile.close();

}





int main(int, char**)
{



    VideoCapture camera("./bourne.mp4");
    if(!camera.isOpened())  // check if we succeeded
        return -1;

    const string NAME = "./output.avi";   // Form the new name with container
    int ex = static_cast<int>(CV_FOURCC('M','J','P','G'));
    Size smallerSize = Size((int) camera.get(CV_CAP_PROP_FRAME_WIDTH),    // Acquire input size
                  (int) camera.get(CV_CAP_PROP_FRAME_HEIGHT));
	Size biggerSize = Size(smallerSize.width + 2 * blurIterations, smallerSize.height + 2 * blurIterations);

	generateGaussianKernelFile(smallerSize);

	unsigned bufferSize = biggerSize.width * biggerSize.height * 3 * sizeof(char);
	//Size smallerSize =Size(1280,720);
	cout << "SIZE:" << smallerSize << endl;
	
    VideoWriter outputVideo;                                        // Open the output
        outputVideo.open(NAME, ex, 25, smallerSize, true);

    if (!outputVideo.isOpened())
    {
        cout  << "Could not open the output video for write: " << NAME << endl;
        return -1;
    }







    cl_event write_event[1];
	cl_event kernel_event;

	char char_buffer[STRING_BUFFER_LEN];
	cl_platform_id platform;
	cl_device_id device;
	cl_context context;
	cl_context_properties context_properties[] =
	{ 
		CL_CONTEXT_PLATFORM, 0,
		CL_PRINTF_CALLBACK_ARM, (cl_context_properties)callback,
		CL_PRINTF_BUFFERSIZE_ARM, 0x1000,
		0
	};
	cl_command_queue queue;
	cl_program program;

	int status;
	clGetPlatformIDs(1, &platform, NULL);
	clGetPlatformInfo(platform, CL_PLATFORM_NAME, STRING_BUFFER_LEN, char_buffer, NULL);
	printf("%-40s = %s\n", "CL_PLATFORM_NAME", char_buffer);
	clGetPlatformInfo(platform, CL_PLATFORM_VENDOR, STRING_BUFFER_LEN, char_buffer, NULL);
	printf("%-40s = %s\n", "CL_PLATFORM_VENDOR ", char_buffer);
	clGetPlatformInfo(platform, CL_PLATFORM_VERSION, STRING_BUFFER_LEN, char_buffer, NULL);
	printf("%-40s = %s\n\n", "CL_PLATFORM_VERSION ", char_buffer);

	context_properties[1] = (cl_context_properties)platform;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
	context = clCreateContext(context_properties, 1, &device, NULL, NULL, NULL);
	queue = clCreateCommandQueue(context, device, 0, NULL);

	unsigned char **opencl_program=read_file("videofilter.cl");
	program = clCreateProgramWithSource(context, 1, (const char **)opencl_program, NULL, NULL);
	if (program == NULL) {
		printf("Program creation failed\n");
		return 1;
	}	
	int success=clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if(success!=CL_SUCCESS) print_clbuild_errors(program,device);
	cl_kernel gaussianBlurKernel = clCreateKernel(program, "gaussianBlur", NULL);
	cl_kernel greyKernel = clCreateKernel(program, "toGrey", NULL);
	
	//buffers 
	cl_mem input_buf = clCreateBuffer(context, CL_MEM_ALLOC_HOST_PTR, bufferSize, NULL, &status);
    checkError(status, "Failed to create buffer for input");
    cl_mem output_buf  = clCreateBuffer(context, CL_MEM_ALLOC_HOST_PTR, bufferSize, NULL, &status);
    checkError(status, "Failed to create buffer for output");

	cl_int errcode;

    const size_t global_work_size[] = { (size_t)smallerSize.width, (size_t)smallerSize.height };
	const size_t local_work_size[] = { 16, 15 };


    status = clSetKernelArg(gaussianBlurKernel, 0, sizeof(cl_mem), &input_buf);
    checkError(status, "Failed to set input argument");

    status = clSetKernelArg(gaussianBlurKernel, 1, sizeof(cl_mem), &output_buf);
    checkError(status, "Failed to set output argument");


    status = clSetKernelArg(greyKernel, 0, sizeof(cl_mem), &input_buf);
    checkError(status, "Failed to set input argument");



	struct timespec bigstart, bigend;
	double tot = 0;
	int count=0;
    
	#define logspeeds
	#ifdef logspeeds
	struct timespec start, end;
	#endif

    while (true) {
        Mat inputFrame;
		count=count+1;
		if(count > 299) break;
        camera >> inputFrame;
		#ifdef logspeeds
		clock_gettime( CLOCK_REALTIME, &start);
		#endif
		clock_gettime( CLOCK_REALTIME, &bigstart);


        // Mat grayframe,edge_x,edge_y,edge,edge_inv;
    	// cvtColor(inputFrame, grayframe, CV_BGR2GRAY);


		copyMakeBorder(inputFrame, inputFrame, blurIterations, blurIterations, blurIterations, blurIterations, BORDER_REPLICATE);

		float* input = (float*)clEnqueueMapBuffer(queue, input_buf, CL_TRUE, CL_MAP_WRITE, 0, bufferSize, 0, NULL, &write_event[0], &errcode);
		checkError(errcode, "Failed to map input");
		memcpy(input, inputFrame.data, bufferSize);
		clEnqueueUnmapMemObject(queue, input_buf, input, 0, NULL,NULL);


		#ifdef logspeeds
		clock_gettime( CLOCK_REALTIME, &end);
		cout << "memcpy " << (double)( end.tv_sec - start.tv_sec ) + (double)( end.tv_nsec - start.tv_nsec ) / BILLION << "\n";
		clock_gettime( CLOCK_REALTIME, &start);
		#endif



		status = clEnqueueNDRangeKernel(queue, greyKernel, 2, NULL,
			global_work_size, local_work_size, 1, write_event, &kernel_event);
    	checkError(status, "Failed to launch kernel");

		#ifdef logspeeds
		clWaitForEvents(1, &kernel_event);
		clock_gettime( CLOCK_REALTIME, &end);
		cout << "grey   " << (double)( end.tv_sec - start.tv_sec ) + (double)( end.tv_nsec - start.tv_nsec ) / BILLION << "\n";
		clock_gettime( CLOCK_REALTIME, &start);
		#endif

		status = clEnqueueNDRangeKernel(queue, gaussianBlurKernel, 2, NULL,
			global_work_size, local_work_size, 1, write_event, &kernel_event);
    	checkError(status, "Failed to launch kernel");

		status = clSetKernelArg(gaussianBlurKernel, 0, sizeof(cl_mem), &output_buf);
		checkError(status, "Failed to set input argument");
		status = clSetKernelArg(gaussianBlurKernel, 1, sizeof(cl_mem), &input_buf);
		checkError(status, "Failed to set output argument");

		status = clEnqueueNDRangeKernel(queue, gaussianBlurKernel, 2, NULL,
			global_work_size, local_work_size, 1, write_event, &kernel_event);
    	checkError(status, "Failed to launch kernel");
		
		status = clSetKernelArg(gaussianBlurKernel, 0, sizeof(cl_mem), &input_buf);
		checkError(status, "Failed to set input argument");
		status = clSetKernelArg(gaussianBlurKernel, 1, sizeof(cl_mem), &output_buf);
		checkError(status, "Failed to set output argument");

		status = clEnqueueNDRangeKernel(queue, gaussianBlurKernel, 2, NULL,
			global_work_size, local_work_size, 1, write_event, &kernel_event);
    	checkError(status, "Failed to launch kernel");




		clWaitForEvents(1, &kernel_event);


		#ifdef logspeeds
		clock_gettime( CLOCK_REALTIME, &end);
		cout << "gauss  " << (double)( end.tv_sec - start.tv_sec ) + (double)( end.tv_nsec - start.tv_nsec ) / BILLION << "\n";
		clock_gettime( CLOCK_REALTIME, &start);
		#endif

		uchar* output  = (uchar*)clEnqueueMapBuffer(queue, output_buf
		, CL_TRUE, CL_MAP_READ, 0, bufferSize, 0, NULL, NULL, &errcode);
		checkError(errcode, "Failed to map output");

		Mat outputFrame = Mat(biggerSize.height, biggerSize.width, CV_8UC3, output);

		clEnqueueUnmapMemObject(queue, output_buf, output, 0, NULL, NULL);

		#ifdef logspeeds
		clock_gettime( CLOCK_REALTIME, &end);
		cout << "output " << (double)( end.tv_sec - start.tv_sec ) + (double)( end.tv_nsec - start.tv_nsec ) / BILLION << "\n";
		clock_gettime( CLOCK_REALTIME, &start);
		#endif
		
    	memset((unsigned char*)outputFrame.data, 0, outputFrame.step * outputFrame.rows);
		cv::Rect cropRect;
		cropRect.x = blurIterations;
		cropRect.y = blurIterations;
		cropRect.width = smallerSize.width;
		cropRect.height = smallerSize.height;

		outputFrame = outputFrame(cropRect);







		clock_gettime( CLOCK_REALTIME, &bigend);
		tot += (double)( bigend.tv_sec - bigstart.tv_sec ) + (double)( bigend.tv_nsec - bigstart.tv_nsec ) / BILLION;



		// grayframe.copyTo(outputFrame,edge);
        // cvtColor(outputFrame, outputFrame, CV_GRAY2BGR);
		
		outputVideo << outputFrame;

	#ifdef SHOW
        imshow(windowName, outputFrame);
	#endif
	}

	
	outputVideo.release();
	camera.release();
  	printf ("FPS %.2lf .\n", 299.0/tot );

    return EXIT_SUCCESS;

}
