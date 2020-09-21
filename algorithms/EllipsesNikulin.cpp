
#include "EllipsesNikulin.h"

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
EllipsesNikulin::EllipsesNikulin()
{
#define TV_WIDTH 1280
#define TV_HEIGHT 1280

	prev = new unsigned char[TV_WIDTH * TV_HEIGHT];
	memset(prev, 0, TV_WIDTH * TV_HEIGHT);
	curr = new unsigned char[TV_WIDTH * TV_HEIGHT];
	memset(curr, 0, TV_WIDTH * TV_HEIGHT);
	delta = new unsigned char[TV_WIDTH * TV_HEIGHT];
	memset(delta, 0, TV_WIDTH * TV_HEIGHT);

	width = height = 0;

	dp=float(M_PI/500);
	pi=float(0.0);

	int ii, jj;
	for(ii=0; ii<TW; ii++) // это нужно раз заполнить раз
	{
		t[ii]=pi;
		pi+=dp;
	}

	for(ii=-255,jj=0; ii<256; ii++,jj++) // это нужно раз заполнить раз
		weight_b[jj]=exp(-ii*ii/(2*SIG*SIG));

	image = new unsigned char[HR * WR];
	simage = new unsigned char[H * W];
	image_float = new float[H * W];

#undef TV_HEIGHT
#undef TV_WIDTH
}

//-------------------------------------------------------------------------------------------------
void EllipsesNikulin::applyDetector(const uint8_t* const greyData,
                                    const uint32_t width, const uint32_t height)
{
	this->width = width;
	this->height = height;

	memcpy(curr, greyData, width * height);

	/*
	// разность кадров
	for (uint32_t i = 0; i < width * height; i++)
	{
		delta[i] = abs(curr[i] - prev[i]);
	}
	*/

	delta = curr;

	detect_sse(delta);

	// текущий кадр становится предыдущим
	memcpy(prev, curr, width * height);
}

//-------------------------------------------------------------------------------------------------
void EllipsesNikulin::detect_sse(unsigned char *data)
{
	int step_line[]={0,-1,6,-1,-1,6,-1,0,0,-1,1,
	                 0,0,1,2,1,1,2,1,0,4,1,-1,4,
	                 0,-1,6,-1,-1,6,-1,0,0,-1,1,
	                 0,0,1,2,1,1,2,1,0,4,1,-1,4};

	int num_obj=detect_finding_objects(data,simage,image_float,image_gauss,weight_b,shist,A,
	                                   B,hist,step_line,current_line,obj_list);
	detect_copter(data, obj_list, t, num_obj, HR, WR, TW);
}

//-------------------------------------------------------------------------------------------------
int EllipsesNikulin::detect_finding_objects(unsigned char* image,unsigned char* simage,
     float* image_float, float* image_gauss,float * weight_b,float *shist ,int *A,
     int *B,int *hist,int *step_line,int *current_line,struct obj *obj_list)
{
	int ii,thr,num_obj; // ,jj;

	detect_resize(image,simage,HR,WR);
	detect_char2float(simage,image_float, H, W);

	detect_gaussb(image_float-2,image_float-1,image_float,image_float+1,image_float+2,
	          image_gauss-2*W,image_gauss-W,image_gauss,image_gauss+W,image_gauss+W*2,
	          weight_b,A,H,W);
	detect_conv(A-6*W-6,A-6*W,A-6*W+6,
	              A-6,A,A+6,A+6*W-6,A+6*W,A+6*W+6,B,H,W);
	thr=detect_binariz_statis(B,hist,shist,H,W);
	memset(A,0,H*W*sizeof(int));
	detect_morph_open(B-W-1,B-W,B-W+1,B-1,B,B+1,
	               B+W-1,B+W,B+W+1,
	               A-W-1,A-W,A-W+1,A-1,A,A+1,
	               A+W-1,A+W,A+W+1,H,W);
	detect_morph_bridge(A,A+1,A+W,A+W+1,H,W);


	num_obj=detect_form_line(A,current_line,step_line,H,W,P3);

	for(ii=0; ii<num_obj; ii++){
		obj_list[ii].summ_x=0;
		obj_list[ii].summ_y=0;
		obj_list[ii].summ_xx=0;
		obj_list[ii].summ_yy=0;
		obj_list[ii].summ_xy=0;
		obj_list[ii].num=0;
		obj_list[ii].clast=0;
	}

	detect_form_obj(A,obj_list,H,W);


	for(ii=0; ii<num_obj; ii++){
		if(obj_list[ii].num>=P2){
			detect_obj_params(&obj_list[ii],float(P4),2,2);
		}
	}

	return num_obj;
}

//-------------------------------------------------------------------------------------------------
void	EllipsesNikulin::detect_gaussb(float* __restrict__  a11,float* __restrict__  a12,
                                       float* __restrict__  a13,
              float* __restrict__  a14,float* __restrict__  a15,
              float* __restrict__  b11,float* __restrict__  b21,float* __restrict__  b31,
              float* __restrict__  b41,float* __restrict__  b51, float* __restrict__ wb,
              int * __restrict__  out, int h,int w){
	int ii,jj;
	float p1,p2,p3,p4,p5,rezult,weightsb,weights_full;
	for(ii=0; ii<h-1; ii++){
		for(jj=2; jj<w-2; jj++){
			p1=a11[ii*w+jj];
		   p2=a12[ii*w+jj];
		   p3=a13[ii*w+jj];
		   p4=a14[ii*w+jj];
		   p5=a15[ii*w+jj];

		   weightsb=wb[(int)(p3-p1)+255]*GH1;
		   rezult=p1*weightsb;
		   weights_full=weightsb;

		   weightsb=wb[(int)(p3-p2)+255]*GH2;
		   rezult+=p2*weightsb;
		   weights_full+=weightsb;

		   rezult+=p3*GH3;
		   weights_full+=float(GH3);

		   weightsb=wb[(int)(p3-p4)+255]*GH4;
		   rezult+=p4*weightsb;
		   weights_full+=weightsb;

		   weightsb=wb[(int)(p3-p5)+255]*GH5;
		   rezult+=p5*weightsb;
		   weights_full+=weightsb;

		   b31[ii*w+jj]=rezult/weights_full;
		   //out[ii*w+jj]=round(rezult/weights_full);
		}
	}

	for(ii=2; ii<h-3; ii++){
		for(jj=2; jj<w-3; jj++){

			p1=b11[ii*w+jj];
		   p2=b21[ii*w+jj];
		   p3=b31[ii*w+jj];
		   p4=b41[ii*w+jj];
		   p5=b51[ii*w+jj];

		    weightsb=wb[(int)(p3-p1)+255]*GV1;
			rezult=p1*weightsb;
		   weights_full=weightsb;

		   weightsb=wb[(int)(p3-p2)+255]*GV2;
		   rezult+=p2*weightsb;
		   weights_full+=weightsb;

		   rezult+=p3*GV3;
		   weights_full+=float(GV3);

		   weightsb=wb[(int)(p4-p1)+255]*GV4;
		   rezult+=p4*weightsb;
		   weights_full+=weightsb;

		   weightsb=wb[(int)(p5-p2)+255]*GV5;
		   rezult+=p5*weightsb;
		   weights_full+=weightsb;

#ifdef __linux__
		   out[ii*w+jj]=round(rezult/weights_full);
#elif _WIN32
		   out[ii*w+jj]=ceil(rezult/weights_full);
#endif
		}
	}
}

//-------------------------------------------------------------------------------------------------
void    EllipsesNikulin::detect_conv(int* __restrict__  a11,int* __restrict__  a12,
                                     int* __restrict__  a13,
             int* __restrict__  a21,int* __restrict__  a22,int* __restrict__  a23,
             int* __restrict__  a31,int* __restrict__  a32,int* __restrict__  a33,
             int* __restrict__  out,int h,int w){

	int ii,jj;
	for(ii=10; ii<h-11; ii++){
		for(jj=10; jj<w-11; jj++){
			out[ii*w+jj]=8*a22[ii*w+jj]-a11[ii*w+jj]-a12[ii*w+jj]-a13[ii*w+jj]-
			        a21[ii*w+jj]-a23[ii*w+jj]-a31[ii*w+jj]-
			        a32[ii*w+jj]-a33[ii*w+jj];
		}
	}
}

//-------------------------------------------------------------------------------------------------
void    EllipsesNikulin::detect_morph_open(int* __restrict__  a11,int* __restrict__  a12,
                                           int* __restrict__  a13,
                   int* __restrict__  a21,int* __restrict__  a22,int* __restrict__  a23,
                   int* __restrict__  a31,int* __restrict__  a32,int* __restrict__  a33,
                   int*  __restrict__ out11,int*  __restrict__ out12,int*  __restrict__ out13,
                   int*  __restrict__ out21,int*  __restrict__ out22,int*  __restrict__ out23,
                   int*  __restrict__ out31,int*  __restrict__ out32,int*  __restrict__ out33,
                   int h,int w){
	int ii,jj,kk;
	for(ii=11; ii<h-12; ii++){
		for(jj=11; jj<w-12; jj++){
			kk=a11[ii*w+jj]&a12[ii*w+jj]&a13[ii*w+jj]&a21[ii*w+jj]&
			        a22[ii*w+jj]&a23[ii*w+jj]&a31[ii*w+jj]&a32[ii*w+jj]&
			        a33[ii*w+jj];
			if (kk==1){
				out11[ii*w+jj]=1;
				out12[ii*w+jj]=1;
				out13[ii*w+jj]=1;
				out21[ii*w+jj]=1;
				out22[ii*w+jj]=1;
				out23[ii*w+jj]=1;
				out31[ii*w+jj]=1;
				out32[ii*w+jj]=1;
				out33[ii*w+jj]=1;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
void    EllipsesNikulin::detect_morph_bridge(int* __restrict__  a11,int* __restrict__  a12,
                                             int* __restrict__  a21,
                     int* __restrict__  a22,int h,int w)
{
	int ii,jj,kk,ll;
	for(ii=9; ii<h-11; ii++){
		for(jj=9; jj<w-11; jj++){
			kk=a11[ii*w+jj]&a22[ii*w+jj]&(a12[ii*w+jj]+1)&(a21[ii*w+jj]+1);
			ll=a12[ii*w+jj]&a21[ii*w+jj]&(a11[ii*w+jj]+1)&(a22[ii*w+jj]+1);
			if(kk==1){
				a12[ii*w+jj]=1;
				a21[ii*w+jj]=1;
			}
			else if(ll==1){
				a11[ii*w+jj]=1;
				a22[ii*w+jj]=1;
			}
		}
	}
}

//-------------------------------------------------------------------------------------------------
int    EllipsesNikulin::detect_form_line(int* __restrict__ in,int* __restrict__  current_line,
                 int* __restrict__ step_line,int h,int w, int P)
{
	int ii,jj,kk,iii,jjj,kkk,num=2,line,cur_pos,dx,dy,ll,r;

	for(ii=10; ii<h-11; ii++){
		for(jj=10; jj<w-11; jj++){
			if (in[ii*w+jj]==1){
				if(in[ii*w+jj-1]!=0 || in[(ii-1)*w+jj]!=0 || in[(ii-1)*w+jj-1]!=0){
					continue;
				}
				in[ii*w+jj]=num;
				line=1;
				cur_pos=0;
				current_line[0]=ii;
				current_line[1]=jj;
				iii=ii;
				jjj=jj;
				kkk=2;
				while (line==1){
					kk=0;
					while(kk<24){
						r=kk+cur_pos*3;
						dy=step_line[r++];
						dx=step_line[r++];
						ll=in[(iii+dy)*w+jjj+dx];
						if(ll==1){
							iii+=dy;
							jjj+=dx;
							current_line[kkk++]=iii;
							current_line[kkk++]=jjj;
							in[iii*w+jjj]=num;
							cur_pos=step_line[r];
							break;

						}
						else if(ll>1){
							if((iii+dy)==current_line[0] && (jjj+dx)==current_line[1]){
								line=2;
								num++;
								if( num> P){
									return (num-2);
								}
								break;
							}
							else{
								line=0;
								for(iii=0; iii<kkk; iii+=2){
									in[current_line[iii]*w+current_line[iii+1]]=0;
								}
								break;
							}
						}
						kk+=3;

					}
					if(kk>24){
						in[iii*w+jj]=0;
						line=0;
					}
				}
			}
		}
	}
	return num-2;
}

//-------------------------------------------------------------------------------------------------
void EllipsesNikulin::detect_form_obj(int* __restrict__ in,struct obj* obj_list,int h,int w)
{
	int ii,jj,flag=0,p,a,b,cobj; // ,n;
	for(ii=10; ii<h-11; ii++)
	{
		for(jj=10; jj<w-11; jj++){
			p=in[ii*w+jj];
			a=in[ii*w+jj-1];
			b=in[ii*w+jj+1];
			if(p>1){
				if(a==0 && b==0){
					continue;
				}
				else if(a==0){
					flag=1;
					cobj=p;
				}
				else if(b==0 && cobj==p){
					flag=0;
				}
				else{
					in[ii*w+jj]=cobj;
				}

			}
			else if(flag==1){
				obj_list[cobj-2].num++;
				obj_list[cobj-2].summ_y+=ii;
				obj_list[cobj-2].summ_x+=jj;
				obj_list[cobj-2].summ_yy+=ii*ii;
				obj_list[cobj-2].summ_xx+=jj*jj;
				obj_list[cobj-2].summ_xy+=ii*jj;
				in[ii*w+jj]=cobj;
			}
		}
	}

}

//-------------------------------------------------------------------------------------------------
void EllipsesNikulin::detect_obj_params(struct obj* current_obj, float P,float kh,float kw)
{

	float a,b,c,angl_max=16331239480885248.0f,delta_min=0.00001f;
	float mean_x,mean_y,k,n,lam_max,lam_min,alpha1,alpha2;
	float cos_alpha,sin_alpha,max_x,max_y,min_x,min_y,e1,e2;
	n=(float)current_obj->num;
	mean_x=((float)current_obj->summ_x)/n;
	mean_y=((float)current_obj->summ_y)/n;
	a=((float)current_obj->summ_xx)/(n-1);
	b=((float)current_obj->summ_yy)/(n-1);
	c=((float)current_obj->summ_xy)/(n-1);
	k=n/(n-1);
	a=P*(a-mean_x*mean_x*k);
	b=P*(b-mean_y*mean_y*k);
	c=P*(c-mean_y*mean_x*k);

	if(fabs(a)<delta_min){
		a=1.0;
	}
	if(fabs(b)<delta_min){
		b=1.0;
	}
	if(fabs(c)<delta_min){
		if(a>b){
			lam_max=a;
			lam_min=b;
			alpha1=0.0;
		}
		else{
			lam_max=b;
			lam_min=a;
			alpha1=angl_max;
		}
	}
	else{
		k=sqrt((a-b)*(a-b)+4*c*c);
		lam_max=(a+b+k)/2;
		lam_min=(a+b-k)/2;
		if(lam_min>lam_max){
			n=lam_max;
			lam_max=lam_min;
			lam_min=n;
		}
		if(fabs(lam_max-b)<delta_min){
			 alpha1=(c<0?-1:1)*angl_max;
		}
		else{
			 alpha1=-(a-lam_max)/c;
			 alpha2=c/(lam_max-b);
			 if(fabs(alpha1-alpha2)>0.001){
				alpha1=0;
				lam_max=0;
				lam_min=0;
			 }
		}
	}
	cos_alpha=1.0/sqrt(alpha1*alpha1+1);
	sin_alpha=alpha1*cos_alpha;
	a=sqrt(lam_max);
	b=sqrt(lam_min);

	current_obj->tan_alpha=alpha1;
	current_obj->sin_alpha=sin_alpha;
	current_obj->cos_alpha=cos_alpha;
	current_obj->a=a*kw;
	current_obj->b=b*kw;
	current_obj->x=mean_x*kw;
	current_obj->y=mean_y*kh;

	min_y=a*sin_alpha;
	max_y=min_y;
	min_x=a*cos_alpha;
	max_x=min_x;

	min_y=min_y<0?min_y:-min_y;
	max_y=max_y>0?max_y:-max_y;
	min_x=min_x<0?min_x:-min_x;
	max_x=max_x>0?max_x:-max_x;

	e1=b*cos_alpha;
	e2=b*sin_alpha;

	min_y=min_y<e1?min_y:e1;
	max_y=max_y>e1?max_y:e1;
	min_x=min_x<e2?min_x:e2;
	max_x=max_x>e2?max_x:e2;

	e1=-e1;
	e2=-e2;

	min_y=min_y<e1?min_y:e1;
	max_y=max_y>e1?max_y:e1;
	min_x=min_x<e2?min_x:e2;
	max_x=max_x>e2?max_x:e2;

	current_obj->max_x=(int)((max_x+mean_x)*kw+0.5);
	current_obj->max_y=(int)((max_y+mean_y)*kh+0.5);
	current_obj->min_x=(int)((min_x+mean_x)*kw+0.5);
	current_obj->min_y=(int)((min_y+mean_y)*kh+0.5);

}

//-------------------------------------------------------------------------------------------------
void EllipsesNikulin::detect_resize(unsigned char* src, unsigned char *dst,int h,int w)
{
	int ii,jj,ll,kk,wd=w>>1;
	for(ii=0,ll=0; ii<h; ii+=2,ll++){
		for(jj=0,kk=0; jj<w; jj+=2,kk++){
			    dst[wd*ll+kk]=src[w*ii+jj];
		    }
	    }
}

//-------------------------------------------------------------------------------------------------
int EllipsesNikulin::detect_binariz_statis(int* in,int* hist,float* shist,int h,int w)
{
	int ii,jj,kk,min,max,sum,pz,sum_jjjj=0,p;
	float summ_hist=0.0,lambda,inv_sum;

	memset((unsigned char*)shist,0,HS*sizeof(float));

	for(ii=10; ii<h-11; ii++){
		for(jj=10; jj<w-11; jj++){
			kk=in[ii*w+jj]-1;
			if (kk<0) continue;
			shist[kk]++;
		}
	}
	for(min=0; min<HS && shist[min]==0; min++);
	for(max=HS-1; max>=0 && shist[max]==0; max--);

	hist[min]=(int)(shist[min]+0.5);
	if(max-min>=3){
		hist[min+1]=(int)(E1*shist[min]+E2*shist[min]+E3*shist[min+1]+E2*shist[min+2]+
		         E1*shist[min+3]+0.5);
		for(ii=min+2; ii<=max-2; ii++){
			     hist[ii]=(int)(E1*shist[ii-2]+E2*shist[ii-1]+E3*shist[ii]+E2*shist[ii+1]+
				 E1*shist[ii+2]+0.5);
		}
		hist[max-1]=(int)(shist[max-1]+0.5);
		hist[max]=(int)(shist[max]+0.5);
	}
	else{
		for(ii=min+1; ii<=max; ii++){
			 hist[ii]=(int)(shist[ii]+0.5);
		}
	}
	sum=hist[min];
	for(ii=min+1; ii<=max; ii++){
		sum+=hist[ii];
		hist[ii]=sum;

	}
	pz=(int)((float)sum*0.7+0.5);
	for(ii=min; ii<=max; ii++){
		if(hist[ii]>pz) break;
	}
	inv_sum=1.0/(float)(sum);
	for(jj=min; jj<=ii; jj++){
		sum_jjjj+=jj*jj;
		summ_hist+=log(1-(float)hist[jj]*inv_sum)*jj;
	}
	lambda=-(float)sum_jjjj/summ_hist;
	p=(int)(0.5-log(PO*lambda)*lambda)+1;
	for(ii=10; ii<h-11; ii++){
		for(jj=10; jj<w-11; jj++){
			in[ii*w+jj]=in[ii*w+jj]<p?0:1;
		}
	}
	return p;

}

//-------------------------------------------------------------------------------------------------
void EllipsesNikulin::detect_ell_drow(unsigned char* image,struct obj* obj_list,float* t,
                                      int num_obj,int hr,int wr,int wt)
{
	int ii,jj,x,y;
	float a,b,cos_alpha,sin_alpha,xc,yc,xp,yp;
	for(ii=0; ii<num_obj; ii++){
		if(obj_list[ii].num<P2) continue;
		a=obj_list[ii].a+3.0;
		b=obj_list[ii].b+3.0;
		cos_alpha=obj_list[ii].cos_alpha;
		sin_alpha=obj_list[ii].sin_alpha;
		xc=obj_list[ii].x;
		yc=obj_list[ii].y;
		for(jj=0; jj<wt; jj++){
			xp=a*cos(t[jj]);
			yp=b*sin(t[jj]);
			x=(int)(xp*cos_alpha-yp*sin_alpha+xc+0.5);
			y=(int)(xp*sin_alpha+yp*cos_alpha+yc+0.5);
			if(x<0 || x>=wr || y<0 || y>=hr) continue;
			image[y*wr+x]=122;
		}

	}
}

//-------------------------------------------------------------------------------------------------
void EllipsesNikulin::detect_copter(unsigned char*, struct obj* obj_list,
                                    float* t, int num_obj, int hr, int wr, int wt)
{
	int ii, jj, x, y;
	float a, b, cos_alpha, sin_alpha, xc, yc, xp, yp;

	for(ii = 0; ii < num_obj; ii++) {

		if(obj_list[ii].num < P2) {
			continue;
		}

		a = obj_list[ii].a + 3.0;
		b = obj_list[ii].b + 3.0;

		cos_alpha = obj_list[ii].cos_alpha;
		sin_alpha = obj_list[ii].sin_alpha;

		xc = obj_list[ii].x;
		yc = obj_list[ii].y;

		Sec_145::Ellipse e;
		Sec_145::Point p;

		e.id = ii;

		for(jj = 0; jj < wt; jj += wt / 8) {

			xp = a * cos(t[jj]);
			yp = b * sin(t[jj]);

			x = (int)(xp * cos_alpha - yp * sin_alpha + xc + 0.5);
			y = (int)(xp * sin_alpha + yp * cos_alpha + yc + 0.5);
			if(x < 0 || x >= wr || y < 0 || y >= hr) {
				continue;
			}

			p.x = x;
			p.y = y;
			e.points.push_back(p);

			if (0 == jj) {
				e.p_1.x = x;
				e.p_1.y = y;
			} else if (4 * wt / 8 == jj) {
				e.p_2.x = x;
				e.p_2.y = y;
			}
		}

		if (e.points.size() > 0) {
			e.points.pop_back(); // first element repeating
			DisjointSet<Ellipse>::addMember(e);
		}
	}
}

//-------------------------------------------------------------------------------------------------
void EllipsesNikulin::detect_char2float(unsigned char* src,float* dst, int h, int w )
{
	int ii;

	__m128i m1,upack1,upack2,upack3,upack4,m2;
	__m128  fm1;

	uint8_t num_255 = 255;
//	upack1=_mm_set_epi8(255,255,255,3,255,255,255,2,255,255,255,1,255,255,255,0);
//	upack2=_mm_set_epi8(255,255,255,7,255,255,255,6,255,255,255,5,255,255,255,4);
//	upack3=_mm_set_epi8(255,255,255,11,255,255,255,10,255,255,255,9,255,255,255,8);
//	upack4=_mm_set_epi8(255,255,255,15,255,255,255,14,255,255,255,13,255,255,255,12);
	upack1=_mm_set_epi8(num_255,num_255,num_255,3,num_255,num_255,num_255,2,num_255,
	                    num_255,num_255,1,num_255,num_255,num_255,0);
	upack2=_mm_set_epi8(num_255,num_255,num_255,7,num_255,num_255,num_255,6,num_255,num_255
	                    ,num_255,5,num_255,num_255,num_255,4);
	upack3=_mm_set_epi8(num_255,num_255,num_255,11,num_255,num_255,num_255,10,num_255,num_255,
	                    num_255,9,num_255,num_255,num_255,8);
	upack4=_mm_set_epi8(num_255,num_255,num_255,15,num_255,num_255,num_255,14,num_255,num_255,
	                    num_255,13,num_255,num_255,num_255,12);

	for (ii=0; ii<h*w; ii+=16){

		m1=_mm_load_si128((__m128i*)&src[ii]);

		m2=_mm_shuffle_epi8(m1,upack1);
		fm1=_mm_cvtepi32_ps(m2);
		_mm_store_ps(dst,fm1);
		dst+=4;

		m2=_mm_shuffle_epi8(m1,upack2);
		fm1=_mm_cvtepi32_ps(m2);
		_mm_store_ps(dst,fm1);
		dst+=4;

		m2=_mm_shuffle_epi8(m1,upack3);
		fm1=_mm_cvtepi32_ps(m2);
		_mm_store_ps(dst,fm1);
		dst+=4;

		m2=_mm_shuffle_epi8(m1,upack4);
		fm1=_mm_cvtepi32_ps(m2);
		_mm_store_ps(dst,fm1);
		dst+=4;
	}
}
