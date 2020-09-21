
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: Class implemets a contour search by Evgeny Nikulin
TODO:
 * test class (logic() method)
 * defines.h
FIXME:
DANGER:
NOTE:

Sec_145::EllipsesNikulin
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|               |            |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include "DisjointSet.h"  // DisjointSet template class (for inheritance)
#include "Geometry.h"     // Ellipses class

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <omp.h>

#include <omp.h>

#include <xmmintrin.h>
#include <emmintrin.h>
#include <tmmintrin.h>

//-------------------------------------------------------------------------------------------------
namespace Sec_145
{

//-------------------------------------------------------------------------------------------------
#define H 384  // высота изображения для обработки
#define W 512  // ширина для обработки

#define HR 768   // высота исходного изображения
#define WR 1024  // ширина исходного изображения

#define HS 255*8
#define TW 1001

#define __restrict__ __restrict
#define M_PI 3.1415926

#define GH1 0.960789439152323
#define GH2 0.975309912028333
#define GH3 0.980198673306755
#define GH4 0.975309912028333
#define GH5 0.960789439152323

#define GV1 1.0
#define GV2 1.015113065
#define GV3 1.02020134
#define GV4 1.015113065
#define GV5 1.0

#define SIG 5.0

#define E1  0.08
#define E2  0.27
#define E3  0.30

#define P1 88
#define P2 10
#define P3 1000
#define P4 7.38
#define P5 1.4
#define P6 20
#define PO 0.0007

#define CORR_OFFSET 32

//-------------------------------------------------------------------------------------------------
// Class implemets a contour search by Evgeny Nikulin
class EllipsesNikulin : protected DisjointSet<Ellipse>
{
public:

	EllipsesNikulin();

	void applyDetector(const uint8_t* const greyData,
	                   const uint32_t width, const uint32_t height);

private:

	void detect_sse(unsigned char* data);

	unsigned char *prev;
	unsigned char *curr;
	unsigned char *delta;
	int width, height;

	struct obj {
		int   num;
		int   clast;
		float a;
		float b;
		float tan_alpha;
		float cos_alpha;
		float sin_alpha;
		float x;
		float y;
		int   min_x;
		int   min_y;  // ERROR !!!!!!!!!!
		int   max_x;
		int   max_y;  // ERROR !!!!!!!!!!
		int summ_x;
		int summ_y;
		int summ_xx;
		int summ_yy;
		int summ_xy;
	};

	// функции для обнаружения объектов
	void detect_resize(unsigned char*,unsigned char*,int,int);
	void detect_char2float(unsigned char*,float*, int, int);
	void detect_gaussb(float*, float*, float*, float*, float*, float*, float*, float*, float*,
	                   float*, float*, int*, int,int);
	void detect_conv(int*, int*, int*, int*, int*, int*, int*, int*, int*, int*, int, int);
	int  detect_binariz_statis(int*,int*,float*,int,int);
	void detect_morph_open(int*, int*, int*, int*, int*, int*, int*, int*, int*, int*, int*,
	                       int*, int*, int*, int*, int*, int*, int*, int h,int w);
	void detect_morph_bridge(int*, int*, int*, int*, int, int);
	int  detect_form_line(int*,int*,int*,int,int,int);
	void detect_form_obj(int*,struct obj*,int,int);
	void detect_obj_params(struct obj*, float,float,float);
	int detect_finding_objects(unsigned char*, unsigned char*, float*, float*, float*, float*,
	                           int*, int*, int*, int*, int*, struct obj*);
	void detect_ell_drow(unsigned char* , struct obj*, float*, int, int, int, int);

	void detect_copter(unsigned char*, struct obj*, float*, int, int, int, int);

	// буферы под служебные файлы для панорамы
	// для обнаружения объектов
	unsigned char /*image[HR*WR],simage[H*W],*/tmp[WR];
	unsigned char *image, *simage;

	float /*image_float[H*W],*/weight_b[512];
	float *image_float;
	float dp, pi;
	float image_gauss[HR*WR],t[TW];
	float shist[HS];
	int   A[H*W],B[H*W],current_line[2*P3],hist[HS];
	struct obj obj_list[P3];
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
