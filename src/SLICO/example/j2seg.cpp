#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "jpeglib.h"
#include "SLIC.h"
#include <limits>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define PUT_2B(array,offset,value)  \
         (array[offset] = (char) ((value) & 0xFF), \
          array[offset+1] = (char) (((value) >> 8) & 0xFF))
#define PUT_4B(array,offset,value)  \
         (array[offset] = (char) ((value) & 0xFF), \
          array[offset+1] = (char) (((value) >> 8) & 0xFF), \
          array[offset+2] = (char) (((value) >> 16) & 0xFF), \
          array[offset+3] = (char) (((value) >> 24) & 0xFF))
#define JPEG_QUALITY 90
#define LBP_VECTORS 16
 
void readbmp(char *filename, unsigned char * image_buffer, int h, int w);
void write_bmp_header(j_decompress_ptr cinfo, FILE *output_file);
void write_pixel_data(j_decompress_ptr cinfo, unsigned char *output_buffer, FILE *output_file);
int write_jpeg_file(char * filename, unsigned char * image_buffer, int w, int h, int quality);
int read_jpeg_file(const char *input_filename, const char *output_filename, int & height, int & width);

int main(int argc, char* argv[])
{
	int h(0),w(0);
	int i,j;
	int x,y;	//pixel:x,y
	
	printf("01.jpg -> bmp \n");
	read_jpeg_file("1.jpg","1.bmp", h, w);
	printf("01 ok. w = %d, h = %d.\n",w,h);

	unsigned char * image_buffer = (unsigned char *)malloc(sizeof(char) * w * h * 4)  ; // BGRA  
    unsigned int  * pbuff        = (unsigned int  *)malloc(sizeof(int) * w * h);
	
	//----------------------------------
    // Initialize parameters
    //----------------------------------
    int k = 2000;    //Desired number of superpixels.
	int k_real = 0;
    double m = 10;  //Compactness factor. use a value ranging from 10 to 40 depending on your needs. Default is 10
    int* klabels = (int *)malloc(sizeof(int) * w * h);
    int numlabels(0);
    string filename = "temp.jpg";
    string savepath = "/home/xduser/LiHuan/SLICO/example/result/";
	
	printf("02. read bmp to buffer.\n");
	readbmp("1.bmp", image_buffer, h, w);
	printf("02 ok!\n");
    // unsigned int (32 bits) to hold a pixel in ARGB format as follows:
    // from left to right,
    // the first 8 bits are for the alpha channel (and are ignored)
    // the next 8 bits are for the red channel
    // the next 8 bits are for the green channel
    // the last 8 bits are for the blue channel

	printf("03. convert char to int.\n");
	for(i = j = 0; i < w*h; i++,j+=4 )
	{	
		*(pbuff + i) = *(image_buffer+j+3) + \
					  (*(image_buffer+j+2))*256 + \
					  (*(image_buffer+j+1)) * 256 * 256 + \
				      (*(image_buffer+j+0)) * 256 * 256 * 256;
	}
	for(i=0;i<w*h*4;i++)
	{
		*(image_buffer+i)=0;
	}
	printf("03 ok.\n");
    
	//----------------------------------
    // Perform SLIC on the image buffer
    //----------------------------------
    SLIC segment;
	printf("04. perform SLICO.\n");
    segment.PerformSLICO_ForGivenK(pbuff, w, h, klabels, numlabels, k, m);
    // Alternately one can also use the function PerformSLICO_ForGivenStepSize() for a desired superpixel size
	printf("04 ok.\n");
    //----------------------------------
    // Save the labels to a text file
    //----------------------------------
	printf("05. savelabels.\n");
    segment.SaveSuperpixelLabels(klabels, w, h, filename, savepath);
	printf("05 ok.\n");
	
	// find the number of real superpixels
    for(i = 0; i < w*h; i++)
    {
        if(klabels[i] > k_real)
            k_real = klabels[i];
    }
    printf("\nk_real = %d\n",k_real);
	
	// put into vector
	vector<vector<int> > superpixel;	// 0 ~ n superpixel
	vector<int> i_superpixel;			// the pixel:x,y in the ith superpixel 

	for(i = 0; i < k_real + 1; i++)
	{	
		for(j = 0; j < w * h; j++)
		{
			if(klabels[j] == i)
			{	
				y = j % w ;
				x = (j - y ) / w;
				i_superpixel.push_back(x);
				i_superpixel.push_back(y);
			}
		}
		superpixel.push_back(i_superpixel);	
		i_superpixel.clear();
	}
	printf("read into vector ok!\n");

	/*	
	for(vector<vector<int> >::size_type ix = 0; ix < superpixel.size(); ix++) 	//superpixel label number
		for(vector<int>::size_type iy = 0; iy < superpixel[ix].size(); iy += 2)	//get all x,y pixel in ith super
			printf("(%d, %d)",superpixel[ix][iy], superpixel[ix][iy+1]);
	*/
	/*	
	int n20,n30,n40,n50,n60,n70,n80,n90,n100,n110,n120,n130,n140;
	n20 = n30 = n40 = n50 = n60 = n70 = n80 = n90 = n100 = n110 = n120 = n130 = n140 = 0;
	for(vector<vector<int> >::size_type ix = 0; ix < superpixel.size(); ix++) 	//superpixel label number
	{
		int p = int(superpixel[ix].size()) / 2;
		if( p>=20 && p<30 ) n20++;
		if( p>=30 && p<40 ) n30++;
		if( p>=40 && p<50 ) n40++;
		if( p>=50 && p<60 ) n50++;
		if( p>=60 && p<70 ) n60++;
		if( p>=70 && p<80 ) n70++;
		if( p>=80 && p<90 ) n80++;
		if( p>=90 && p<100 ) n90++;
		if( p>=100 && p<110 ) n100++;
		if( p>=110 && p<120 ) n110++;
		if( p>=120 && p<130 ) n120++;
		if( p>=130 && p<140 ) n130++;
		if( p>=140 && p<200 ) n140++;
	}
	printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", n20,n30,n40,n50,n60,n70,n80,n90,n100,n110,n120,n130,n140);
	printf("%d,%d,%d",7/2,8/2,9/2);
	*/

	/*****************************************************
	** Extract Feature 
	******************************************************/
/*
	// 01.LBP feature [16]
	vector<vector<float> > lbp_superpixel;
	vector<vector<float> > labxy_superpixel;
	vector<float> temp;
	int max_x, min_x, max_y, min_y;
	max_x = max_y = 0;
	min_x = min_y = 65536;	
	float color_l, color_a, color_b; 
	color_l = color_a = color_b = 0;
    Mat image_lbp = imread("/home/xduser/LiHuan/SLICO/build/example/1.jpg", 0);		// 0:¿¿¿
	printf("%d-%d\n", image_lbp.rows, image_lbp.cols);
	if(image_lbp.empty())
		printf("imread fail.\n");
	Mat image_lab = imread("1.jpg");		// BGR
	printf("%d-%d\n", image_lab.rows, image_lab.cols);
//	Mat img_lbp, 
	Mat img_lab;					// ¿¿
	//Rect rect;
	int radius = 1;
	int neighbors = 8;
	Mat lbp_feature, lab_feature;	
	MatND hist;
	int bins = LBP_VECTORS;     //default = 16
	int hist_size[] = {bins};   
	float range[]   = {0, 255};
	const float * ranges[] = {range};
	int channels[] = {0};
	int sizeofrect;
	
	// ¿¿¿¿¿   
	for(vector<vector<int> >::size_type ix = 0; ix < superpixel.size(); ix++) 	//superpixel label number
	{	
		for(vector<int>::size_type iy = 0; iy < superpixel[ix].size(); iy += 2)	//get all x,y pixel in ith super
		{	
			// ¿¿i¿¿¿¿¿¿¿¿	
			if(superpixel[ix][iy  ] > max_x)	max_x = superpixel[ix][iy];
			if(superpixel[ix][iy+1] > max_y)	max_y = superpixel[ix][iy+1];
			if(superpixel[ix][iy]   < min_x)	min_x = superpixel[ix][iy];
			if(superpixel[ix][iy+1] < min_y)	min_y = superpixel[ix][iy+1];
		}
		Rect rect(min_y, min_x, max_y-min_y+1, max_x-min_x+1);	//(1,3)>rect(3,1)
		sizeofrect = (max_y-min_y+1) * (max_x-min_x+1);
		printf("%d,%d,%d,%d\n",min_y,min_x,max_y,max_x);

		// lbp¿¿
    	Mat img_lbp = image_lbp(rect);
		printf("img_lbp=image_lbp(rect). ok \n");
    	lbp_feature = Mat(img_lbp.rows-2*radius, img_lbp.cols-2*radius,CV_8UC1, Scalar(0));
   	 	elbp(img_lbp, lbp_feature, 1, 8);
    	calcHist(&lbp_feature, 1, channels, Mat(), hist, 1, hist_size, ranges, true, false);
		for(i=0; i < bins; i++)
			temp.push_back( hist.at<float>(i) );
		lbp_superpixel.push_back(temp);
		img_lab = image_lab(rect);    	//imshow("image",img);
		temp.clear();    	
		
		// LAB,xy ¿¿	
		img_lab = image_lab(rect);    	//imshow("image",img);
		cvtColor(img_lab, lab_feature, COLOR_BGR2Lab);
		for(i=0; i<(max_x-min_x+1); i++)
		{	
			for(j=0; j<(max_y-min_y+1); j++)
			{
				color_l += lab_feature.at<Vec3b>(i,j).val[0];				
				color_a += lab_feature.at<Vec3b>(i,j).val[1];				
				color_b += lab_feature.at<Vec3b>(i,j).val[2];				
			}
		}
		temp.push_back( color_l / sizeofrect );
		temp.push_back( color_a / sizeofrect );
		temp.push_back( color_b / sizeofrect );
		temp.push_back( (max_x-min_x+1)/2 );
		temp.push_back( (max_y-min_y+1)/2 );
		labxy_superpixel.push_back(temp);
		temp.clear();    	
	}//for 0~2016

	printf("lbp_superpixel = %d.", int(lbp_superpixel.size()));
	printf("labxy_superpixel = %d.", int(labxy_superpixel.size()));
	for(vector<int>::size_type ix = 0; ix < lbp_superpixel[0].size(); ix++) 	//superpixel label number
		printf("%d-", int(lbp_superpixel[0][ix]) );
	printf("\n");

	for(vector<int>::size_type ix = 0; ix < labxy_superpixel[0].size(); ix++) 	//superpixel label number
		printf("%d-", int(labxy_superpixel[0][ix]) );
	printf("\n");
	
*/
	
	//----------------------------------
    // Draw boundaries around segments
    //----------------------------------
	printf("\n06. draw contour to buffer.\n");
    segment.DrawContoursAroundSegments(pbuff, klabels, w, h, 0xff0000);
	printf("06 ok.\n");

    //----------------------------------
    // Save the image with segment boundaries.
    //----------------------------------
	for(i = j = 0; i < w*h; i++,j+=3 )
    {
        *(image_buffer+j+2) = ( (*(pbuff + i)) & 0xff);
        *(image_buffer+j+1) = ( ( (*(pbuff + i)) >> 8) & 0xff);
        *(image_buffer+j+0) = ( ( (*(pbuff + i)) >> 16) & 0xff);
    } 
    write_jpeg_file("/home/xduser/LiHuan/SLICO/example/result/temp_seg.jpg", image_buffer, w, h, JPEG_QUALITY);
    printf("savejpeg ok!\n");
	
    //----------------------------------
    // Clean up
    //----------------------------------
	free(klabels);
	free(image_buffer);
	free(pbuff);

	return 0;
}

/********************************
 ** func:open jpeg, write to bmp 
*********************************/
int read_jpeg_file(const char *input_filename, const char *output_filename, int & height, int & width)
 {
         struct jpeg_decompress_struct cinfo;
         struct jpeg_error_mgr jerr;
         FILE *input_file;
         FILE *output_file;
         JSAMPARRAY buffer;
         int row_width;

         unsigned char *output_buffer;
         unsigned char *tmp = NULL;

         cinfo.err = jpeg_std_error(&jerr);
		
		 //open jpeg file
         if ((input_file = fopen(input_filename, "rb")) == NULL) {
                 fprintf(stderr, "can't open %s\n", input_filename);
                 return -1;
         }
	     //open bmp file
         if ((output_file = fopen(output_filename, "wb")) == NULL) {
                fprintf(stderr, "can't open %s\n", output_filename);
                return -1;
         }

         jpeg_create_decompress(&cinfo);

         /* Specify data source for decompression */
         jpeg_stdio_src(&cinfo, input_file);

         /* Read file header, set default decompression parameters */
         (void) jpeg_read_header(&cinfo, TRUE);

         /* Start decompressor */
         (void) jpeg_start_decompress(&cinfo);

         row_width = cinfo.output_width * cinfo.output_components;
		
		 height = cinfo.output_height;
		 width  = cinfo.output_width;
		//printf("color_space = %d.\n",cinfo.out_color_space);
		//printf("quantize_color = %d.\n",cinfo.quantize_colors);			
         buffer = (*cinfo.mem->alloc_sarray)
                 ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_width, 1);

         write_bmp_header(&cinfo, output_file);

         output_buffer = (unsigned char *)malloc(row_width * cinfo.output_height);
         memset(output_buffer, 0, row_width * cinfo.output_height);
         tmp = output_buffer;

         /* Process data */
         while (cinfo.output_scanline < cinfo.output_height) {
                 (void) jpeg_read_scanlines(&cinfo, buffer, 1);
                 memcpy(tmp, *buffer, row_width);
                 tmp += row_width;
         }

         write_pixel_data(&cinfo, output_buffer, output_file);

         free(output_buffer);

         (void) jpeg_finish_decompress(&cinfo);

         jpeg_destroy_decompress(&cinfo);

         /* Close files, if we opened them */
         fclose(input_file);
         fclose(output_file);

         return 0;
}

/*******************************
** func:open bmp,read to buffer
********************************/
void readbmp(char *filename, unsigned char * image_buffer, int h, int w)
{
	int temp,j,i;
	FILE * fpbmp;
	fpbmp = fopen(filename, "rb");
	if (fpbmp == NULL)
	{
		printf("Open bmp failed!!!\n");
		return ;
	}
	
	unsigned char * bits24_buffer = (unsigned char *)malloc(sizeof(char) * w * h * 3);
	temp = fseek(fpbmp, 54, SEEK_SET);  // bmp24 or 32bits headersize:54
	fread (bits24_buffer, 1, w*h*3, fpbmp);
	/*
	printf("R:24bits_buffer[3002]=%x.\n",bits24_buffer[3002]);
	printf("G:24bits_buffer[3001]=%x.\n",bits24_buffer[3001]);
	printf("B:24bits_buffer[3000]=%x.\n",bits24_buffer[3000]);
	*/

	//copy [BGR] to [ARGB]
	for (i=0,j=0; i < w * h * 3 ; i+=3,j+=4)
	{
		*(image_buffer + j + 0) = 0;
		*(image_buffer + j + 1) = *(bits24_buffer + i + 2);
		*(image_buffer + j + 2) = *(bits24_buffer + i + 1);
		*(image_buffer + j + 3) = *(bits24_buffer + i + 0);
	}
	/*
	printf("R:image_buffer[4001]=%x.\n",image_buffer[4001]);
	printf("G:image_buffer[4002]=%x.\n",image_buffer[4002]);
	printf("B:image_buffer[4003]=%x.\n",image_buffer[4003]);
	printf("BGRA TO ARGB OK!\n ");
	*/
	free(bits24_buffer);
}


void write_bmp_header(j_decompress_ptr cinfo, FILE *output_file)
{
	char bmpfileheader[14];
	char bmpinfoheader[40];
	long headersize, bfSize;
	int bits_per_pixel, cmap_entries;
	int step;

         /* Compute colormap size and total file size */
         if (cinfo->out_color_space == JCS_RGB) {
                 if (cinfo->quantize_colors) {
                         /* Colormapped RGB */
                         bits_per_pixel = 8;
                         cmap_entries = 256;
                 } else {
                         /* Unquantized, full color RGB */
                         bits_per_pixel = 24;
                         cmap_entries = 0;
                 }
         } else {
                 /* Grayscale output.  We need to fake a 256-entry colormap. */
                 bits_per_pixel = 8;
                 cmap_entries = 256;
         }

         step = cinfo->output_width * cinfo->output_components;

         while ((step & 3) != 0) step++;

         /* File size */
         headersize = 14 + 40 + cmap_entries * 4; /* Header and colormap */

        bfSize = headersize + (long) step * (long) cinfo->output_height;

         /* Set unused fields of header to 0 */
         memset(bmpfileheader, 0, sizeof(bmpfileheader));
         memset(bmpinfoheader, 0 ,sizeof(bmpinfoheader));

         /* Fill the file header */
         bmpfileheader[0] = 0x42;/* first 2 bytes are ASCII 'B', 'M' */
         bmpfileheader[1] = 0x4D;
         PUT_4B(bmpfileheader, 2, bfSize); /* bfSize */
         /* we leave bfReserved1 & bfReserved2 = 0 */
         PUT_4B(bmpfileheader, 10, headersize); /* bfOffBits */

         /* Fill the info header (Microsoft calls this a BITMAPINFOHEADER) */
         PUT_2B(bmpinfoheader, 0, 40);   /* biSize */
         PUT_4B(bmpinfoheader, 4, cinfo->output_width); /* biWidth */
         PUT_4B(bmpinfoheader, 8, cinfo->output_height); /* biHeight */
         PUT_2B(bmpinfoheader, 12, 1);   /* biPlanes - must be 1 */
         PUT_2B(bmpinfoheader, 14, bits_per_pixel); /* biBitCount */
         /* we leave biCompression = 0, for none */
         /* we leave biSizeImage = 0; this is correct for uncompressed data */
         if (cinfo->density_unit == 2) { /* if have density in dots/cm, then */
                 PUT_4B(bmpinfoheader, 24, (INT32) (cinfo->X_density*100)); /* XPels/M */
                 PUT_4B(bmpinfoheader, 28, (INT32) (cinfo->Y_density*100)); /* XPels/M */
         }
         PUT_2B(bmpinfoheader, 32, cmap_entries); /* biClrUsed */
         /* we leave biClrImportant = 0 */

         if (fwrite(bmpfileheader, 1, 14, output_file) != (size_t) 14) {
                 printf("write bmpfileheader error\n");
         }
         if (fwrite(bmpinfoheader, 1, 40, output_file) != (size_t) 40) {
                 printf("write bmpinfoheader error\n");
         }

         if (cmap_entries > 0) {
         }
 }

 void write_pixel_data(j_decompress_ptr cinfo, unsigned char *output_buffer, FILE *output_file)
 {
         int rows, cols;
         int row_width;
         int step;
         unsigned char *tmp = NULL;

        unsigned char *pdata;

         row_width = cinfo->output_width * cinfo->output_components;
         step = row_width;
         while ((step & 3) != 0) step++;

         pdata = (unsigned char *)malloc(step);
         memset(pdata, 0, step);

         tmp = output_buffer + row_width * (cinfo->output_height - 1);
         for (rows = 0; rows < cinfo->output_height; rows++) {
                 for (cols = 0; cols < row_width; cols += 3) {
                         pdata[cols + 2] = tmp[cols + 0];
                         pdata[cols + 1] = tmp[cols + 1];
                         pdata[cols + 0] = tmp[cols + 2];
                 }
                 tmp -= row_width;
                 fwrite(pdata, 1, step, output_file);
         }

         free(pdata);
 }

/***************************************
** func: buffer[RGB] write to jpeg file
****************************************/
int write_jpeg_file(char * filename, unsigned char * image_buffer, int w, int h, int quality)
{

  struct jpeg_compress_struct cinfo;
//  unsigned char  * image_buffer;
  int i = 0;
  struct jpeg_error_mgr jerr;
  /* More stuff */
  FILE * outfile;  /* target file */
  JSAMPROW row_pointer[1]; /* pointer to JSAMPLE row[s] */
  int row_stride;  /* physical row width in image buffer */

  /* Step 1: allocate and initialize JPEG compression object */

  /* We have to set up the error handler first, in case the initialization
   * step fails.  (Unlikely, but it could happen if you are out of memory.)
   * This routine fills in the contents of struct jerr, and returns jerr's
   * address which we place into the link field in cinfo.
   */
   /*¿¿¿¿¿jpeg compress ¿¿*/
  cinfo.err = jpeg_std_error(&jerr);
  /* Now we can initialize the JPEG compression object. */
  jpeg_create_compress(&cinfo);

  /* Step 2: specify data destination (eg, a file) */
  /* Note: steps 2 and 3 can be done in either order. */

  /* Here we use the library-supplied code to send compressed data to a
   * stdio stream. You can also write your own code to do something else.
   * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
   * requires it in order to write binary files.
   */
   /*¿¿¿¿¿¿¿¿*/
  if ((outfile = fopen(filename, "wb")) == NULL) {
 	fprintf(stderr, "can't open %s\n", filename);
 	exit(1);
  }
  jpeg_stdio_dest(&cinfo, outfile);

  /* Step 3: set parameters for compression */

  /* First we supply a description of the input image.
   * Four fields of the cinfo struct must be filled in:
   */
   /*¿¿ ¿¿¿¿ libjpeg¿¿¿¿¿¿¿¿¿¿¿¿¿
  ¿¿¿¿¿¿¿640 480¿¿¿demo¿¿¿¿color_space¿¿
  ¿¿¿*/
  cinfo.image_width = w; /* image width and height, in pixels */
  cinfo.image_height = h;
  cinfo.input_components = 3;  /* # of color components per pixel */
  cinfo.in_color_space = JCS_RGB; /* colorspace of input image */
  /* Now use the library's routine to set default compression parameters.
   * (You must set at least cinfo.in_color_space before calling this,
   * since the defaults depend on the source color space.)
   */
  jpeg_set_defaults(&cinfo);
  /* Now you can set any non-default parameters you wish to.
   * Here we just illustrate the use of quality (quantization table) scaling:
   */
   /*¿¿quality¿2*/
  jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

  /* Step 4: Start compressor */

  /* TRUE ensures that we will write a complete interchange-JPEG file.
   * Pass TRUE unless you are very sure of what you're doing.
   */
   /*¿¿¿¿*/
  jpeg_start_compress(&cinfo, TRUE);

  /* Step 5: while (scan lines remain to be written) */
  /*     jpeg_write_scanlines(...); */

  /* Here we use the library's state variable cinfo.next_scanline as the
   * loop counter, so that we don't have to keep track ourselves.
   * To keep things simple, we pass one scanline per call; you can pass
   * more if you wish, though.
   */
  row_stride = w * 3; /* JSAMPLEs per row in image_buffer */
// image_buffer = (unsigned char*)malloc(640*480*3);

 if (NULL == image_buffer)
 {
  return -1;
 }
/*
 for(i=0; i< 640*480; i++)
   {
     image_buffer[i*3] = i*255;
     image_buffer[i*3+1] = 128-(i*255)&0x7f;
     image_buffer[i*3+2] = 255-(i*255)&0xff;
   }
*/
  while (cinfo.next_scanline < cinfo.image_height) {
 /* jpeg_write_scanlines expects an array of pointers to scanlines.
  * Here the array is only one element long, but you could pass
  * more than one scanline at a time if that's more convenient.
  */
// row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
	row_pointer[0] = & image_buffer[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];
	 (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
  }

  /* Step 6: Finish compression */

  jpeg_finish_compress(&cinfo);
  /* After finish_compress, we can close the output file. */
  fclose(outfile);

  /* Step 7: release JPEG compression object */

  /* This is an important step since it will release a good deal of memory. */
  jpeg_destroy_compress(&cinfo);

  /* And we're done! */
}

/*
int savejpeg(char *filename, unsigned char *bits, int width, int height, int depth)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE * outfile;                 //target file   
	JSAMPROW row_pointer[1];        //pointer to JSAMPLE row[s]   
	int     row_stride;             //physical row width in image buffer   
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(filename, "wb")) == NULL)
	{
		fprintf(stderr, "can't open %s/n", filename);
		return -1;
	}
	jpeg_stdio_dest(&cinfo, outfile);
	cinfo.image_width = width;      //image width and height, in pixels   
	cinfo.image_height = height;
	cinfo.input_components = 3;         //# of color components per pixel   
	cinfo.in_color_space = JCS_RGB;         //colorspace of input image   
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, JPEG_QUALITY, TRUE);//limit to baseline-JPEG values   
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = width * depth; // JSAMPLEs per row in image_buffer   
	while (cinfo.next_scanline < cinfo.image_height)
	{
		//¿¿¿¿¿¿¿¿¿¿jpg¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿¿  
		//¿¿¿¿¿¿  
		//row_pointer[0] = & bits[cinfo.next_scanline * row_stride];  
		row_pointer[0] = &bits[(cinfo.image_height - cinfo.next_scanline - 1) * row_stride];
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);
	return 0;
}
*/

