#include "StdAfx.h"
#include "VisualFilter.h"


typedef	unsigned char Pixel;

typedef struct {
	int	xsize;		/* horizontal size of the image in Pixels */
	int	ysize;		/* vertical size of the image in Pixels */
	Pixel *	data;		/* pointer to first scanline of image */
	int	span;		/* byte offset between two scanlines */
} Image;

typedef struct {
	int	pixel;
	double	weight;
} CONTRIB;

typedef struct {
	int	n;		/* number of contributors */
	CONTRIB	*p;		/* pointer to list of contributions */
} CLIST;

Image * new_image(int xsize, int ysize)	/* create a blank image */
{
	Image *image;

	if((image = (Image *)malloc(sizeof(Image)))
	&& (image->data = (Pixel *)VirtualAlloc( NULL, ysize*xsize*sizeof(Pixel), MEM_COMMIT, PAGE_READWRITE))) {
		image->xsize = xsize;
		image->ysize = ysize;
		image->span = xsize;
		return(image);
	}
	else{
		free( image);
		return NULL;
	}
}

void free_image(Image *image)
{
	if( image!=NULL){
		if( image->data!=NULL)
			VirtualFree( image->data, 0, MEM_RELEASE);
		free(image);
	}
}


Pixel get_pixel(Image *image, int x, int y)
{
	static Image *im = NULL;
	static int yy = -1;
	static Pixel *p = NULL;

	if((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize)) {
		return(0);
	}
	if((im != image) || (yy != y)) {
		im = image;
		yy = y;
		p = image->data + (y * image->span);
	}
	return(p[x]);
}

void get_row(Pixel *row, Image *image, int y)
{
	if((y < 0) || (y >= image->ysize)) {
		return;
	}
	memcpy(row,
		image->data + (y * image->span),
		(sizeof(Pixel) * image->xsize));
}

void get_column(Pixel *column, Image *image, int x)
{
	int i, d;
	Pixel *p;

	if((x < 0) || (x >= image->xsize)) {
		return;
	}
	d = image->span;
	for(i = image->ysize, p = image->data + x; i-- > 0; p += d) {
		*column++ = *p;
	}
}

Pixel put_pixel(Image *image, int x, int y, Pixel data)
{
	static Image *im = NULL;
	static int yy = -1;
	static Pixel *p = NULL;

	if((x < 0) || (x >= image->xsize) || (y < 0) || (y >= image->ysize)) {
		return(0);
	}
	if((im != image) || (yy != y)) {
		im = image;
		yy = y;
		p = image->data + (y * image->span);
	}
	return(p[x] = data);
}


/*
 *	image rescaling routine
 */


int zoom(Image *dst, Image *src)
{

	Image *tmp;			/* intermediate image */
	double xscale, yscale;		/* zoom scale factors */
	int i, j, k;			/* loop variables */
	double center, left, right;	/* filter calculation variables */
	double width, weight;	/* filter calculation variables */
	Pixel *raster;			/* a row or column of pixels */
	CLIST	*contrib;		/* array of contribution lists */
	
	/* create intermediate image to hold horizontal zoom */
	tmp = new_image(dst->xsize, src->ysize);
	if( tmp==NULL) return 0;

	xscale = (double) (dst->xsize-1) / (double) (src->xsize-1);
	yscale = (double) (dst->ysize-1) / (double) (src->ysize-1);

	/* pre-calculate filter contributions for a row */
	contrib = (CLIST *)calloc(dst->xsize, sizeof(CLIST));
	if( contrib==NULL) return 0;

	if(xscale < 1.0) {
		width = 0.5 / xscale;
		for(i = 0; i < dst->xsize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (width * 2 + 1),
					sizeof(CONTRIB));
			if( contrib[i].p==NULL) return 0;
			center = (double) i / xscale;
			left = ceil(center - width);
			right = floor(center + width);
			if( left<0) left =0;
			if( right>= src->xsize) right =src->xsize-1;
			weight = 1 / (right-left+1);
			for(j = left; j <= right; ++j) {
				k = contrib[i].n++;
				contrib[i].p[k].pixel = j;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->xsize; ++i) {
			contrib[i].n = 2;
			contrib[i].p = (CONTRIB *)calloc(2,	sizeof(CONTRIB));
			if( contrib[i].p==NULL) return 0;
			center = (double) (i) / xscale;
			right = ceil(center);
			left = floor(center);
			if( left<0) left =0;
			if( right>= src->xsize) right =src->xsize-1;
			weight =right-center;
			contrib[i].p[0].pixel = left;
			contrib[i].p[0].weight = weight;
			contrib[i].p[1].pixel = right;
			contrib[i].p[1].weight = 1-weight;
		}
	}


	/* apply filter to zoom horizontally from src to tmp */
	raster = (Pixel *)calloc(src->xsize, sizeof(Pixel));
	if( raster==NULL) return 0;

	for(k = 0; k < tmp->ysize; ++k) {
		get_row(raster, src, k);
		for(i = 0; i < tmp->xsize; ++i) {
			weight = 0.0;
			for(j = 0; j < contrib[i].n; ++j) {
				weight += raster[contrib[i].p[j].pixel]
					* contrib[i].p[j].weight;
			}
			put_pixel(tmp, i, k,(Pixel)weight);
		}
	}

	free(raster);
	

	/* free the memory allocated for horizontal filter weights */
	for(i = 0; i < tmp->xsize; ++i) {
		free(contrib[i].p);
	}
	free(contrib);

	/* pre-calculate filter contributions for a column */
	contrib = (CLIST *)calloc(dst->ysize, sizeof(CLIST));
	if( contrib==NULL) return 0;
	if(yscale < 1.0) {
		width = 0.5 / yscale;
		for(i = 0; i < dst->ysize; ++i) {
			contrib[i].n = 0;
			contrib[i].p = (CONTRIB *)calloc((int) (width * 2 + 1),
					sizeof(CONTRIB));
			if( contrib[i].p==NULL) return 0;
			center = (double) i / yscale;
			left = ceil(center - width);
			right = floor(center + width);
			if( left<0) left =0;
			if( right>= src->ysize) right =src->ysize-1;
			weight = 1 / (right-left+1);
			for(j = left; j <= right; ++j) {
				k = contrib[i].n++;
				contrib[i].p[k].pixel = j;
				contrib[i].p[k].weight = weight;
			}
		}
	} else {
		for(i = 0; i < dst->ysize; ++i) {
			contrib[i].n = 2;
			contrib[i].p = (CONTRIB *)calloc(2,	sizeof(CONTRIB));
			if( contrib[i].p==NULL) return 0;
			center = (double) (i) / yscale;
			right = ceil(center);
			left = floor(center);
			if( left<0) left =0;
			if( right>= src->ysize) right =src->ysize-1;
			weight =right-center;
			contrib[i].p[0].pixel = left;
			contrib[i].p[0].weight = weight;
			contrib[i].p[1].pixel = right;
			contrib[i].p[1].weight = 1-weight;
		}
	}

	/* apply filter to zoom vertically from tmp to dst */
	raster = (Pixel *)calloc(tmp->ysize, sizeof(Pixel));
	if( raster==NULL) return 0;
	for(k = 0; k < dst->xsize; ++k) {
		get_column(raster, tmp, k);
		for(i = 0; i < dst->ysize; ++i) {
			weight = 0.0;
			for(j = 0; j < contrib[i].n; ++j) {
				weight += raster[contrib[i].p[j].pixel]
					* contrib[i].p[j].weight;
			}
			put_pixel(dst, k, i,(Pixel)weight);
		}
	}
	free(raster);

	/* free the memory allocated for vertical filter weights */
	for(i = 0; i < dst->ysize; ++i) {
		free(contrib[i].p);
	}
	free(contrib);

	free_image(tmp);
	
	return 1;
}


CVisualFilter::CVisualFilter(void)
{
}


CVisualFilter::~CVisualFilter(void)
{
}
//BYTE CVisualFilter:: Convert2Grayscale(double intensity,double upper,double lower)
//{
//	if( lower == upper) return 0;
//	if( intensity < lower) return 0;
//	if( intensity > upper) return 255;
//	return(int(255*(intensity-lower)/(upper-lower)));
//}
BYTE CVisualFilter:: Convert2Grayscale(double intensity,double lower,double upper,double factor)
{
	
	if( intensity < lower) return 0;
	if( intensity > upper) return 255;
	return(int(factor*(intensity-lower)));
}


BYTE* CVisualFilter::Zoom(BYTE* data,CSize srcSize,CSize dstSize)
{
		if(data==NULL) return NULL;
		BYTE* idata=new BYTE[dstSize.cx*dstSize.cy];
		Image src,dst;
		src.data=data;
		src.xsize=srcSize.cx;
		src.ysize=srcSize.cy;
		src.span=src.xsize;

		dst.data=idata;
		dst.xsize=dstSize.cx;
		dst.ysize=dstSize.cy;
		dst.span=dst.xsize;
		zoom(&dst,&src);
		
		return data;
}
BYTE* CVisualFilter::Zoom(BYTE* data,CSize srcSize,double scale)
{
	return Zoom(data,srcSize,CSize(srcSize.cx*scale,srcSize.cy*scale));
}


