#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef ANIMATE
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define X_SIZE 800
#define Y_SIZE 800
#endif

#define prec float
#define PI 3.14159265359


static prec gdt = 0.0001;

typedef struct {

  prec mass; /* mass */
  prec cordX; /* X cordinate */
  prec cordY; /* Y cordinate */
  prec veloX; /* X velocity */
  prec veloY; /* Y velocity */
  prec accX; /* X acceleration */
  prec accY; /* Y acceleration */
  prec forceX; /* X force */
  prec forceY;/* Y force */

} body;



static void update(body* a, prec dt)
{ 

  /*calculation of new acceleartion for X and Y*/
  a->accX = a->forceX / a->mass;
  a->accY = a->forceY / a->mass;
  
  
  /*calculation of new velocity for X and Y*/
  a->veloX += dt*a->accX;
  a->veloY += dt*a->accY;
  
  
  /*calculation of new cordinations for X and Y components*/
  a->cordX +=  (dt*a->veloX) + 0.5*dt*dt*a->accX;
  a->cordY +=  (dt*a->veloY) + 0.5*dt*dt*a->accY;
    
}


static void addForce(body* a, body* b)
{
  prec dx = b->cordX - a->cordX;
  prec dy = b->cordY - a->cordY;

  prec dist = sqrt(dx*dx + dy*dy);
  prec inv = 1.0/dist;
  prec inv2 = inv*inv;

  prec F = b->mass*inv2;
  a->forceX += F * dx;
  a->forceY += F * dy;
}



static void resetForce(body* b) 
{
  /*resets force X and Y to zero*/
  b->forceX = 0;
  b->forceY = 0;
} 


static prec newRand() 
{
  prec r = (prec)((double)rand()/(double)RAND_MAX);
  return r;
}


void init(int N, body* star)
{


  for (int i = 0; i <= N; i++) 
    {
    star[i].mass = newRand();
    star[i].cordX = newRand();
    star[i].cordY = newRand();
    star[i].veloX = newRand();
    star[i].veloY = newRand();

    }
}

static void updateForces(int N, body* star)

/*
första loopen går ingeom alla stjärnor
andra loopen går igenom alla stjärnans krafter
*/

{
  for( int i = 0; i < N; i++ ) {
    resetForce(&star[i]);
    for( int j = 0; j < N; j++ ) {
	  if( i != j ) {
	   
	  
	    addForce(&star[i], &star[j]);
	    addForce(&star[i], &star[j]);

	  }
    } update(&star[i], gdt);
    
  } 
}

// Manually copy coordinates from stars into points (to be drawn).
// Look at the manual file for XPoint to see which 
// format XPoint accepts its coordinates in.
#ifdef ANIMATE
static void copyToXBuffer(body* star, XPoint* points, int N)

{


    for( int i=0; i < N; i++)
      {
	points[i].x = star[i].cordX + (X_SIZE / 2);
	points[i].y = star[i].cordY + (Y_SIZE / 2);
      }
}

#endif

int main(int argc, char* argv[]) {

  int N = 200;
  int iter = 1000;

  if(argc == 
    {
      N = atoi(argv[1]);
      iter = atoi(argv[2]);
    
    }

    body *star = star[N];

    init(N, star);

    


#ifdef ANIMATE
  XPoint* points = malloc(sizeof(XPoint)*N);
  Display* disp;
  Window window, rootwin;
  int screen;
  

  

  disp = XOpenDisplay(NULL);
  screen = DefaultScreen(disp);
  rootwin = RootWindow(disp,screen);
  window = XCreateSimpleWindow(disp,rootwin,
                               0,0,X_SIZE,Y_SIZE,1,0,0);
  GC gc = XCreateGC(disp, window, 0, 0);
  XSetForeground(disp, gc, WhitePixel(disp, screen));
  XSetBackground(disp, gc, BlackPixel(disp, screen));
  XMapWindow(disp,window);

  XClearWindow(disp,window);	
	
  copyToXBuffer(star, points, N);
  XDrawPoints(disp, window, gc, points, N, 0);

  XFlush(disp);

#endif

  clock_t start = clock();
  for(int i = 0; i < iter; i++)
    {

      updateForces(N, &star[i]);

#ifdef ANIMATE
      copyToXBuffer(star, points, N);
      XDrawPoints(disp, window, gc, points, N, CoordModeOrigin);
      XClearWindow(disp,window);	
#endif
    }
  clock_t stop = clock();
  float diff = (float)(stop - start)/CLOCKS_PER_SEC;
  printf("Total: %lf seconds\n",diff);
  printf("Bodies: %d\n",N);
  printf("Iterations: %d\n", iter);

#ifdef ANIMATE
  XCloseDisplay(disp);
#endif

  return 0;
}
