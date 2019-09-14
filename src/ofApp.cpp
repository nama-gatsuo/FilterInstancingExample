#include "ofApp.h"



void ofApp::setup(){

	detectShader.loadCompute("shader/detect.glsl");
	instancingShader.load("shader/instancing");

	paramPanel.setup();
	paramPanel.add(lumThres.set("lumThres", 0.5, 0., 1.));
	paramPanel.add(voxelSize.set("voxelSize", 20., 0., 50.));

	box = ofMesh::box(1, 1, 300, 1, 1, 1);
	
	rs.enableFlags(
		ofxRealSenseUtil::USE_DEPTH_MESH_POINTCLOUD |
		ofxRealSenseUtil::USE_COLOR_TEXTURE
	);
	rsPanel.setup(rs.getParameters());

	const glm::vec2 s(ofxRealSenseUtil::rsColorRes);
	
	posTex.allocate(s.x, s.y, GL_RGBA32F);
	posTex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	colorTex.allocate(s.x, s.y, GL_RGBA8);
	colorTex.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	acb.setMaxCount(s.x * s.y);

	helper.init();
	cam.setFarClip(1500);
}

void ofApp::update(){
	rs.update();
	hasTex = rs.getColorImage().isAllocated() && rs.getPointCloud().hasVertices();
	const auto pointCloud = rs.getPointCloud().getVbo();
	
	const auto& vertexBuffer = pointCloud.getVertexBuffer();
	const auto& texCoordBuffer = pointCloud.getTexCoordBuffer();

	if (hasTex) {
		/*float t = ofGetElapsedTimef() * 0.6;
		cam.setPosition(300. * cos(t), 0., 600. * sin(t));
		cam.lookAt(glm::vec3(0));*/

		vertexBuffer.bind(GL_SHADER_STORAGE_BUFFER);
		vertexBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
		texCoordBuffer.bind(GL_SHADER_STORAGE_BUFFER);
		texCoordBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 1);
		posTex.bindAsImage(0, GL_WRITE_ONLY);
		colorTex.bindAsImage(1, GL_WRITE_ONLY);
		acb.bind();

		detectShader.begin();
		detectShader.setUniform1f("lumThres", lumThres.get());
		detectShader.setUniformTexture("colorImage", rs.getColorImage().getTexture(), 0);
		detectShader.dispatchCompute(pointCloud.getVertexBuffer().size() / 3, 1, 1);
		detectShader.end();

		acb.unbind();
		colorTex.unbind();
		posTex.unbind();
		texCoordBuffer.unbindBase(GL_SHADER_STORAGE_BUFFER, 1);
		texCoordBuffer.unbind(GL_SHADER_STORAGE_BUFFER);
		vertexBuffer.unbindBase(GL_SHADER_STORAGE_BUFFER, 0);
		vertexBuffer.unbind(GL_SHADER_STORAGE_BUFFER);

		helper.render([&](float lds, bool isShadow) {
			ofTranslate(0, 0, 600.);
			instancingShader.begin();
			instancingShader.setUniform1f("lds", lds);
			instancingShader.setUniform1i("isShadow", isShadow ? 1 : 0);
			instancingShader.setUniform1f("voxelSize", voxelSize);
			instancingShader.setUniform1f("time", ofGetElapsedTimef());
			instancingShader.setUniformTexture("posTex", posTex, 1);
			instancingShader.setUniformTexture("colorTex", colorTex, 2);
			acb.drawIndirect(box.getVbo(), GL_TRIANGLE_STRIP);
			instancingShader.end();
		}, cam);
		
	}
	
}

void ofApp::draw(){
	if (hasTex) helper.getRenderedImage().draw(0, 0, ofGetWidth(), ofGetHeight());

	if (isDebug) {
		//helper.debugDraw();
		helper.drawGui();
		rsPanel.draw();
		paramPanel.draw();

		float w = ofGetViewportWidth() * 0.25;
		float h = ofGetViewportHeight() * 0.25;
		float y = ofGetHeight() - h;
		float x = 0;
		//posTex.draw(x, y, w, h); x += w;
		//colorTex.draw(x, y, w, h); x += w;
		//rs.getColorImage().draw(x + w * 3., y, w, h); // x += w;
		//rs.getDepthImage().draw(x, y, w, h);
	}
}

void ofApp::exit() {
	rs.stopThread();
}

void ofApp::keyPressed(int key){
	if (key == 's') {
		isDebug = !isDebug;
	}
}

