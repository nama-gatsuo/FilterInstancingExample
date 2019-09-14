#pragma once
#include "ofMain.h"
#include "ofxRealSenseUtil.h"
#include "ofxDeferredHelper.h"

class ofApp : public ofBaseApp{
public:
	ofApp() : ofBaseApp(), acb(36), isDebug(false) {}
	void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
private:
	ofxPanel paramPanel;
	ofParameter<float> depthThres;
	ofParameter<float> lumThres;
	ofParameter<float> voxelSize;

	ofxRealSenseUtil::Interface rs;
	ofxPanel rsPanel;
	
	ofxDeferred::AtomicCounterBuffer acb;
	ofShader detectShader;
	ofShader instancingShader;
	ofTexture posTex;
	ofTexture colorTex;
	ofVboMesh box;
	ofEasyCam cam;

	ofxDeferred::Helper helper;
	bool isDebug;
	bool hasTex;
};
