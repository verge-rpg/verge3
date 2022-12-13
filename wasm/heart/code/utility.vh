/*Tristan "RageCage" Michael's Utilities*/

/*a powerful little function I made for the effects libraries*/
void tGrabRotScale(int x, int y, int width, int height, int destx, int desty, int angle, int scale, int source, int dest){
	int tempImage=newImage(width,height);
	rectFill(0,0,width,height,RGB(255,0,255),tempImage);
	tGrabRegion(x,y,x+width,y+height,0,0,source,tempImage);
	RotScale(destx, desty, angle, scale, tempImage, dest);
	freeImage(tempImage);
}

/*absolute value*/
int abs(int a)
{
 if (a<0) return 0-a;
 else return a;
}

/*FPS*/
int fps_frames;
int fps_lasttimer = timer;
void fps_showrate()
{
	fps_frames ++;
	if(timer - fps_lasttimer >= 100)
	{
		setAppName("FPS: "+str(fps_frames));
		fps_frames = 0;
		fps_lasttimer = timer;
	}
}