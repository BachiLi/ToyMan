#ifndef __CROP_WINDOW_H__
#define __CROP_WINDOW_H__

struct CropWindow {
	CropWindow() {}
	CropWindow(int xs, int xe, int ys, int ye) {
		xStart = xs; xEnd = xe;
		yStart = ys; yEnd = ye;
	}

	//pixel range: [xStart, xEnd), [yStart, yEnd)
	int xStart, xEnd;
	int yStart, yEnd;

	int GetWidth() const {return xEnd-xStart;}
	int GetHeight() const {return yEnd-yStart;}
};

#endif //#ifndef __CROP_WINDOW_H__