#include "colorConverter.h"




ofPoint convertRgbToHsv(ofPoint rgbColor){
	
	const double &x = rgbColor.x / 255.0f;
    const double &y = rgbColor.y / 255.0f;
    const double &z = rgbColor.z / 255.0f;
	
    double max	 = (x > y) ? ((x > z) ? x : z) : ((y > z) ? y : z);
    double min	 = (x < y) ? ((x < z) ? x : z) : ((y < z) ? y : z);
    double range = max - min;
    double val	 = max;
    double sat   = 0;
    double hue   = 0;
    
    if (max != 0)   sat = range/max;
    
    if (sat != 0) 
    {
		double h;
		
		if      (x == max)	h =     (y - z) / range;
		else if (y == max)	h = 2 + (z - x) / range;
		else		h = 4 + (x - y) / range;
		
		hue = h/6.;
	    
		if (hue < 0.)
			hue += 1.0;
    }
    return ofPoint(hue * 255.0f, sat * 255.0f, val * 255.0f);
}

ofPoint convertHsvToRgb(ofPoint hsvColor){
	
	double hue = hsvColor.x / 255.0f;
    double sat = hsvColor.y / 255.0f;
    double val = hsvColor.z / 255.0f;
	
    double x = 0.0, y = 0.0, z = 0.0;
    
    if (hue == 1) hue = 0;
    else hue *= 6;
	
    int i = int(floor(hue));
    double f = hue-i;
    double p = val*(1-sat);
    double q = val*(1-(sat*f));
    double t = val*(1-(sat*(1-f)));
	
    switch (i) 
    {
		case 0: x = val; y = t; z = p; break;
		case 1: x = q; y = val; z = p; break;
		case 2: x = p; y = val; z = t; break;
		case 3: x = p; y = q; z = val; break;
		case 4: x = t; y = p; z = val; break;
		case 5: x = val; y = p; z = q; break;
    }
	
    return ofPoint(x*255.0f,y*255.0f,z*255.0f);
	
}
	
