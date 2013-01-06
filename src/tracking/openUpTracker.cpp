#include "openUpTracker.h"


//--------------------------------------------------------------
OpenUpTracker::OpenUpTracker(){

	myName = "Open Up Tracker";
}


OpenUpTracker::~OpenUpTracker(){
	delete [] mhiImageFloat;

}



//--------------------------------------------------------------
void OpenUpTracker::setup(int width, int height){

	minSize = 10;
	maxSize = width*height*.75;
	w = width;
	h = height;
	threshold = 60;
    bSaveBgImage = false;

	//--- allocate all
	colorImg.allocate(w, h);
	grayImgPreModification.allocate(w, h);
	grayImg.allocate(w, h);
	threshImg.allocate(w, h);
	grayBgImg.allocate(w, h);
	prevFrame.allocate(w, h);
	motionImg.allocate(w, h);
	mhiDisp.allocate(w,h);
	colorMask.allocate(w,h);
	grayBgImgMasked.allocate(w,h);

	int totalWH = w*h;
	
	//--learn background
	pixelsScore = new int[totalWH];
	
	mhiImageFloat = new float[totalWH];
	memset(mhiImageFloat,0,totalWH*sizeof(float));
	mhiImageByte = new unsigned char[totalWH];
	memset(mhiImageByte,0,totalWH*sizeof(char));

	//--- setup gui
	setupGui();

	//-- set up mask
	masker.setup(w,h,10,10,1);
	masker.setConstraints( ofRectangle(10,10,w,h) );
    masker.enable();

	//--- find out how many bgs we have capped, so we don't overwrite
	bgCapDirectory = "bgCaps";
	ofxDirList lister;
	tBgs = lister.listDir(bgCapDirectory);

	if( tBgs > 0 )
	{
		ofImage img;
		cout << bgCapDirectory+"/"+lister.getName(tBgs-1) << endl;
		img.loadImage( bgCapDirectory+"/bgCap_"+ofToString(tBgs-1)+".jpg" );
		grayBgImg.setFromPixels(img.getPixels(), grayBgImg.width, grayBgImg.height);
		for (int i = 0; i < w*h; i++){
			pixelsScore[i]==100;
		}
	}

	//--- set up color tracking
	hsvColorTracker.setup(w,h);
	int tColorPix = w*h*3;
	colorPixels = new unsigned char[tColorPix];
	convColor.allocate(w,h);

	//--- set up osc
	ofxXmlSettings xml;
	xml.loadFile("settings/oscSettings.xml");
	port = xml.getValue("osc:port",3008);
	setupOscSender("localhost");

	//--- setup tuio
	TS.setup(w, h);     //start tuio server, if configured

    lastTime = 0;
    waitTime = 100;
    tBgsM = lister.listDir("bgCapsMasked/");
}

void OpenUpTracker::learnBackground(){
	int forget = panel.getValueI("FORGET");
	int dec = panel.getValueI("SCORE_DEC");
	int inc = panel.getValueI("SCORE_INC");
	int lt = panel.getValueI("LEARN_THRESHOLD");
	int ms = panel.getValueI("MAX_SCORE");
	int ds = panel.getValueI("DIF_SENSIBILITY");
							 
	unsigned char * gray = grayImg.getPixels();
	unsigned char * previousFrame = prevFrame.getPixels();
	unsigned char * bg = grayBgImg.getPixels();
	for (int i = 0; i < w*h*3; i+=3){
		int change = abs(gray[i] - previousFrame[i]);
		if(change>ds){
			if(pixelsScore[i/3]==ms){
			    pixelsScore[i/3]=forget;
			}else if(pixelsScore[i/3]>dec){
				pixelsScore[i/3]-=dec;
			}else{
				pixelsScore[i/3]=0;
			}
		}else{
				pixelsScore[i/3]++;
				if(pixelsScore[i/3]==lt){
					
					bg[i] = gray[i];
					bg[i+1] = gray[i+1];
					bg[i+2] = gray[i+2];
				}
				if(pixelsScore[i/3]>ms){
					pixelsScore[i/3] = ms;
					bg[i] = gray[i];
					bg[i+1] = gray[i+1];
					bg[i+2] = gray[i+2];
				}
			
		}
		
	}
	grayBgImg.setFromPixels(bg, w, h);
	//
}

//--------------------------------------------------------------
void OpenUpTracker::update(ofxCvColorImage & colorImgFromCam){


	// update vars from control panel
	updateGui();

	// maybe not needed, slow?
	colorImg = colorImgFromCam;

	// preserve original
	grayImgPreModification = colorImgFromCam;

	// copy original
	grayImg = grayImgPreModification;

	// mask
	grayImg -= masker.mask;


	// bg  cap
	if( panel.getValueB("BG_CAPTURE") ){

		// cap bg
		grayBgImg = grayImgPreModification;
		for (int i = 0; i < w*h; i++){
			pixelsScore[i]==100;
		}
		bSaveBgImage = true;

		// reset gui
		panel.setValueB("BG_CAPTURE", false);
	}

	if(bSaveBgImage)
	{
	    // save to dir
		ofImage img;
		img.setFromPixels(grayImgPreModification.getPixels(), grayImgPreModification.width, grayImgPreModification.height, OF_IMAGE_GRAYSCALE);
		img.saveImage(bgCapDirectory+"/bgCap_"+ofToString(tBgs)+".jpg");
		tBgs++;
		bSaveBgImage = false;

	}



	//------------- background subtraction
	if( panel.getValueB("B_BG_SUB") )
	{
			grayBgImgMasked = grayBgImg;
			grayBgImgMasked -= masker.mask;
		
		learnBackground();
		
		prevFrame = grayImgPreModification;
		prevFrame -= masker.mask;

			threshImg.absDiff(grayImg,grayBgImgMasked);
			threshImg.threshold(threshold, false);
			threshImg.erode();
			threshImg.dilate();

			int blurVal = panel.getValueI( "BLUR" );
			if(  blurVal > 0 )
			{
				threshImg.blur(blurVal);
				threshImg.threshold(11, false);
			}

			int num = contourFinder.findContours(threshImg, minSize, maxSize, MAX_USERS_HARDLIMIT, false, false);

			if( num > 0 && panel.getValueB("B_BLOB_TRACK") ) blobTracker.trackBlobs( contourFinder.blobs );

	}

	// TEMP
	float dt = ofGetElapsedTimef() - lastTime;
	if( dt > waitTime )
	{
	   /* grayBgImgMasked = grayBgImg;
	    grayBgImgMasked -= threshImg;
	    lastTime = ofGetElapsedTimef();
	    cout << "bgmask cap " << endl;
        ofImage img;
		img.setFromPixels(grayBgImgMasked.getPixels(), grayBgImgMasked.width, grayBgImgMasked.height, OF_IMAGE_GRAYSCALE);
		img.saveImage("bgCapsMasked/bgCapM_"+ofToString(tBgsM)+".jpg");
		tBgsM++;*/

	}

	//------------- motion detection
	if( panel.getValueB("B_MOTION") )
	{
		grayImg = grayImgPreModification;
		grayImg -= masker.mask;

		motionImg.absDiff(grayImg, prevFrame);   // motionImg is the difference between current and previous frame
		motionImg.blur(3);
		motionImg.threshold(panel.getValueI("MOT_THRESHOLD") );
		motionImg.dilate();

		int blurVal = panel.getValueI( "MOT_BLUR" );
		if(  blurVal > 0 )
		{
			motionImg.blurGaussian(blurVal);//(blurVal);
			motionImg.threshold(10, false);
		}


		int num = contourFinderMotion.findContours(motionImg, panel.getValueI("MOT_MIN_BLOB"), panel.getValueI("MOT_MAX_BLOB"), MAX_USERS_HARDLIMIT, false, false);

		prevFrame = grayImgPreModification;
		prevFrame -= masker.mask;

		generateHistory(motionImg.getPixels(),.95);


	}

	//--------- color finding
	if( panel.getValueB("B_COLOR_TRACK") )
	{
		hsvColorTracker.updateColorImage();
		convColor = colorImg;
		colorMask = masker.mask;
		convColor -= colorMask;
		convColor.convertRgbToHsv();
		colorPixels = convColor.getPixels();

		if(contourFinderMotion.nBlobs > 0 )
			hsvColorTracker.findBlobColor(convColor,contourFinderMotion.blobs[0]);


		ofPoint hsvColor = hsvColorTracker.rgbColor;//ofPoint(panel.getValueI("HUE_TESTER"),255,255);
		hsvColor = convertRgbToHsv(hsvColor);
		colorPicker.pickColorSimple(hsvColor.x);

		// for viewing
		hsvColorTracker.colorFindImg.convertHsvToRgb();
	}


	//---- mask
	masker.makeMask();

	//--- tuio
	if (TS.mode == CONNECTED && panel.getValueB("B_COLOR_TRACK") ){

        TS.update(blobTracker.blobs);

    }
}
//--------------------------------------------------------------
//--------------------------------------------------------------
void OpenUpTracker::generateHistory(unsigned char * input,float decreaseRate){

	for (int i = 0; i < w*h; i++){
		mhiImageFloat[i] += input[i];

		if (mhiImageFloat[i] > 255){
			mhiImageFloat[i] = 255;
		}

		mhiImageFloat[i] *= decreaseRate;
		mhiImageByte[i] = (unsigned char)(mhiImageFloat[i]);
	}

	mhiDisp.setFromPixels(mhiImageByte,w,h);

}

//--------------------------------------------------------------
void OpenUpTracker::draw(float x, float y){


	ofSetColor(255,255,255);

	//---- live video or movie file
	colorImg.draw(10,10,320,240);

	if( panel.getSelectedPanelName() == "presence")
	{
		grayImg.draw(320+20, 10, 320,240);
		grayBgImg.draw(10,20+240,320,240);

		ofSetColor(255,255,255, 255);
		threshImg.draw(10 + 10 + 320, 10+10 + 240,320,240);
		contourFinder.draw(10 + 10 + 320,10+10+240,320,240);

		ofSetColor(255,255,255);

		grayImg.draw(10 + 10 + 320,10,320,240);
		//contourFinder.draw(10 + 10 + 320,10,320,240);
		blobTracker.draw( 10 + 10 + 320,10,1,1 );  // draws all blobs
	}
	else if( panel.getSelectedPanelName() == "motion detection" )
	{
		grayImg.draw(320+20, 10, 320,240);
		grayBgImg.draw(10,20+240,320,240);

		ofSetColor(255,255,255, 255);
		motionImg.draw(10 + 10 + 320, 10+10 + 240,320,240);
		//contourFinderMotion.draw(10 + 10 + 320,10+10+240,320,240);

		ofSetColor(255,255,255);
		//motionImg.draw(10 + 10 + 320,10,320,240);
		mhiDisp.draw(10 + 10 + 320,10,320,240);
		contourFinderMotion.draw(10 + 10 + 320,10,320,240);
	}
	else if( panel.getSelectedPanelName() == "color tracking")
	{
		convColor.draw(10+10+320,10,320,240);
		//hsvColorTracker.colorTrackerImg.draw(10,10+10+240,320,240);
		hsvColorTracker.colorFindImg.draw(10+10+320,10+10+240,320,240);
		//colorFindImg.draw(10+10+320,10+10+240,320,240);
		ofFill();
		ofSetColor(hsvColorTracker.rgbColor.x,hsvColorTracker.rgbColor.y,hsvColorTracker.rgbColor.z);
		ofRect(10,10+10+240,320,240);
	}else if( panel.getSelectedPanelName() == "blob tracking")
	{
        grayImg.draw(10, 10, 640,480);
        blobTracker.drawWData( 10,10,2,2 );
	}
	else if( panel.getSelectedPanelName() == "learn bg")
	{
        //grayImg.draw(10, 10, 640,480);
        grayBgImg.draw(10,20+240,320,240);
	}



	panel.draw();

	if( panel.getSelectedPanelName() == "presence") masker.draw();
}

void OpenUpTracker::setupGui()
{

	// create the gui panel
	panel.setup("cv panel", 700, 10, 300, 500);

	// add some tabs
	panel.addPanel("presence", 1, false);
	panel.addPanel("motion detection", 1, false);
	panel.addPanel("color tracking", 1, false);
	panel.addPanel("blob tracking", 1, false);
	panel.addPanel("options", 1, false);
	panel.addPanel("learn bg", 1, false);

	//---- set which tab we are on and add some controls
	panel.setWhichPanel("presence");

	panel.addToggle("capture bg ", "BG_CAPTURE", false);

	panel.addSlider("threshold ", "THRESHOLD_GAZE", threshold, 0, 255, true);
	panel.addSlider("min blob","MIN_BLOB",10,0,500,true);
	panel.addSlider("max blob","MAX_BLOB",100*100,0,50500,true);
	panel.addSlider("blur","BLUR",11,0,11,true);
	panel.addToggle("clear mask ", "CLEAR_MASK", false);
	panel.addToggle("save mask ", "SAVE_MASK", false);
	panel.addToggle("load mask ", "LOAD_MASK", false);
	panel.addToggle("save current image as bg", "SAVE_BG", false);
	
	panel.setWhichPanel("learn bg");
	panel.addSlider("forget BG","FORGET",150,119,300,true);
	panel.addSlider("score dec","SCORE_DEC",4,1,100,true);
	panel.addSlider("score inc","SCORE_INC",4,1,100,true);
	panel.addSlider("learn threshold","LEARN_THRESHOLD",120,0,300,true);
	panel.addSlider("max score","MAX_SCORE",300,50,500,true);
	panel.addSlider("difference sensibility","DIF_SENSIBILITY",10,0,254,true);


	panel.setWhichPanel("options");
	panel.addToggle("Use Background Subtraction ", "B_BG_SUB", true);
	panel.addToggle("Use Motion Detection ", "B_MOTION", false);
	panel.addToggle("Use Blob Tracking ", "B_BLOB_TRACK", true);
	panel.addToggle("Use Color Tracking ", "B_COLOR_TRACK", true);

	panel.setWhichPanel("color tracking");
	panel.addSlider("hue range ", "HUE", hsvColorTracker.hueRange, 0, 255, true);
	panel.addSlider("sat range ", "SAT", hsvColorTracker.satRange, 0, 255, true);
	panel.addSlider("val range ", "VAL", hsvColorTracker.valRange, 0, 255, true);
	//panel.addSlider("hue tester ", "HUE_TESTER", 0, 0, 180, true);

	panel.setWhichPanel("motion detection");
	panel.addSlider("mot_threshold ", "MOT_THRESHOLD", 80, 0, 255, true);
	panel.addSlider("min blob","MOT_MIN_BLOB",10,0,500,true);
	panel.addSlider("max blob","MOT_MAX_BLOB",100*100,0,50500,true);
	panel.addSlider("blur","MOT_BLUR",11,0,11,true);
	panel.addSlider("fade rate", "FADE_RATE", .99, 0, 1, false);

	// load our saved settings
	panel.loadSettings("settings/"+myName+"Settings.xml");


}



void OpenUpTracker::updateGui(){

	panel.update();

	minSize = panel.getValueI("MIN_BLOB");
	maxSize = panel.getValueI("MAX_BLOB");

	threshold				= panel.getValueI("THRESHOLD_GAZE");

	hsvColorTracker.setHueSatBri( panel.getValueI("HUE"),  panel.getValueI("SAT"),  panel.getValueI("VAL"));

	if(panel.getValueB("CLEAR_MASK") )
	{
		panel.setValueB("CLEAR_MASK",false);
		masker.clear();
	}

	if(panel.getValueB("SAVE_MASK") )
	{
		panel.setValueB("SAVE_MASK",false);
		masker.saveMask("settings/mask_"+myName+".xml");
	}

	if(panel.getValueB("LOAD_MASK") )
	{
		panel.setValueB("LOAD_MASK",false);
		masker.loadMask("settings/mask_"+myName+".xml");
	}

	if(panel.getValueB("SAVE_BG") )
	{
	    panel.setValueB("SAVE_BG",false);
	    bSaveBgImage = true;
	}
}


void OpenUpTracker::setupOscSender( string host )
{
	ofxOscSender sender;
	oscSender.push_back(sender);
	oscSender[ oscSender.size()-1 ].setup( host,port);
}

void OpenUpTracker::sendOsc()
{
	for( int i = 0; i < oscSender.size(); i++)
	{
		ofxOscMessage m;
		m.setAddress("/tracker");
		m.addStringArg(myName);
		oscSender[i].sendMessage(m);
	}
}
