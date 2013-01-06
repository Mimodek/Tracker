#include "inputManager.h"




/*
<app>

	<!-- mode, input 0 = live, 1 = video -->
	<mode>1</mode>

	<!-- some per mode settings -->
	<videoFile>inputTestVideos/test1.mov</videoFile>

	<videoGrabber>
		<width>640</width>
		<height>480</height>

		<!-- this is optional -->
		<deviceId></deviceId>
	</videoGrabber>


</app>
*/

//--------------------------------------------------------------
void inputManager::setup(){

	ofxXmlSettings XML;
	XML.loadFile("settings/inputSettings.xml");
	mode = XML.getValue("app:mode", 0);

	cout << "mode " << mode << endl;

	if (mode == INPUT_VIDEO){

		string movieToLoad = XML.getValue("app:videoFile", "movies/cam_color.mov");
		vidPlayer.loadMovie(movieToLoad);
		vidPlayer.play();
		vidPlayer.setLoopState(OF_LOOP_NORMAL);
		width	= XML.getValue("app:width",320);//vidPlayer.width;
		height	= XML.getValue("app:height", 240);//vidPlayer.height;
		cout << movieToLoad << " " << width << " " << height << endl;

		panel_video.setup("video panel", 700, 10+10+530, 300, 180);
        panel_video.addPanel("player", 1, false);
        panel_video.addSlider("video position","VID_POS",0,0,1,false);
        panel_video.addToggle("play","VID_PLAY",true);

	}

	if (mode == INPUT_LIVE_VIDEO){

		width = XML.getValue("app:width", 320);
		height = XML.getValue("app:height", 240);
		int device = XML.getValue("app:videoGrabber:deviceId", 0);

		if (device != 0){
			vidGrabber.setDeviceID(device);
		}

		vidGrabber.initGrabber(width, height, false);	// false = no texture. faster...
		width = vidGrabber.width;
		height = vidGrabber.height;		// in case we are different then what we request... ;)
	}

	if (width != 0 || height != 0){
		colorImg.setUseTexture(false);
		grayImage.setUseTexture(false);
		colorImg.allocate(width,height);
		grayImage.allocate(width,height);
	}

	bIsFrameNew = false;
}

//--------------------------------------------------------------
void inputManager::update(){



	if (mode == INPUT_LIVE_VIDEO){
		vidGrabber.grabFrame();
		bIsFrameNew = vidGrabber.isFrameNew();
	} else {
		vidPlayer.idleMovie();
		bIsFrameNew = vidPlayer.isFrameNew();
	}

	if (bIsFrameNew){

		if (mode == INPUT_LIVE_VIDEO){
			colorImg.setFromPixels(vidGrabber.getPixels(),width,height);
		} else {
			colorImg.setFromPixels(vidPlayer.getPixels(), width,height);
		}

        //grayImage = colorImg;		// TODO: this color to gray conversion is *slow*, since it's using the CV cvt color, we can make this faster !

	}

	if( mode == INPUT_VIDEO )
    {
        panel_video.update();

        if( panel_video.getValueB("VID_PLAY") && vidPlayer.speed != 1 ) vidPlayer.setSpeed(1);
        else if( !panel_video.getValueB("VID_PLAY") && vidPlayer.speed != 0 ) vidPlayer.setSpeed(0);

        float vpos = vidPlayer.getPosition();
        panel_video.setValueF("VID_POS",vpos);

    }
}

void inputManager::draw()
{
    if( mode == INPUT_VIDEO )
    {
     panel_video.draw();
    }

}

void  inputManager::scrubVideo(int x, int y){
    if( isInsideRect(x, y, panel_video.boundingBox))
    {
        float vpos = vidPlayer.getPosition();
        if(vpos!=panel_video.getValueF("VID_POS") ){
        vidPlayer.setPosition( panel_video.getValueF("VID_POS") );
        }
    }

}
