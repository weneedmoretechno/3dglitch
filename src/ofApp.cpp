/**
 *
 * OFDevCon Example Code Sprint
 * Model Distort Example
 *
 * This example loads a model and distorts it using noise
 *
 * The model is the open source and freely licensed balloon dog by Rob Myers, commissioned by furtherfield:
 * http://www.furtherfield.org/projects/balloon-dog-rob-myers
 *
 * Created by James George for openFrameworks workshop at Waves Festival Vienna sponsored by Lichterloh and Pratersauna
 * Adapted during ofDevCon on 2/23/2012
 */

#include "ofApp.h"
 
const int N = 256;		//Number of bands in spectrum
float spectrum[ N ];	//Smoothed spectrum values
float Rad = 500;		//Cloud raduis parameter
float Vel = 0.05;		//Cloud points velocity parameter
int bandRad = 20;		//Band index in spectrum, affecting Rad value
int bandVel = 150;		//Band index in spectrum, affecting Vel value

const int n = 300;		//Number of cloud points

//Offsets for Perlin noise calculation for points
float tx[n], ty[n], tz[n];
ofPoint p[n];			//Cloud's points positions

float time0 = 0;		//Time value, used for dt computing


//--------------------------------------------------------------
void ofApp::setup(){
    //Set up sound sample
    sound.loadSound( "loop01.wav" );
    sound.setLoop( true );
    sound.play();
    
    //Set spectrum values to 0
    for (int i=0; i<N; i++) {
        spectrum[i] = 0.0f;
    }
    //Initialize points offsets by random numbers
    for ( int j=0; j<n; j++ ) {
        tx[j] = ofRandom( 0, 1000 );
        ty[j] = ofRandom( 0, 1000 );
        tz[j] = ofRandom( 0, 1000 );
    }
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(50, 50, 50, 0);
    
    //we need to call this for textures to work on models
    ofDisableArbTex();
    
    //this makes sure that the back of the model doesn't show through the front
    ofEnableDepthTest();
    
    //now we load our model
    model.loadModel("m2.obj");
    model.setPosition(ofGetWidth()*.5, ofGetHeight() * 0.75, 0);
    position1 = model.getPosition();
    light.enable();
}

//--------------------------------------------------------------
void ofApp::update(){
    //Update sound engine
    ofSoundUpdate();
    
    //Get current spectrum with N bands
    float *val = ofSoundGetSpectrum( N );
    //We should not release memory of val,
    //because it is managed by sound engine
    
    //Update our smoothed spectrum,
    //by slowly decreasing its values and getting maximum with val
    //So we will have slowly falling peaks in spectrum
    for ( int i=0; i<N; i++ ) {
        spectrum[i] *= 0.97;	//Slow decreasing
        spectrum[i] = max( spectrum[i], val[i] );
    }
    
    //Update particles using spectrum values
    
    //Computing dt as a time between the last
    //and the current calling of update()
    float time = ofGetElapsedTimef();
    float dt = time - time0;
    dt = ofClamp( dt, 0.0, 0.1 );
    time0 = time; //Store the current time
    
    //Update Rad and Vel from spectrum
    //Note, the parameters in ofMap's were tuned for best result
    //just for current music track
    Rad = ofMap( spectrum[ bandRad ], 1, 3, 400, 800, true );
    Vel = ofMap( spectrum[ bandVel ], 0, 0.1, 0.05, 0.5 );
    

    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    ofSetColor(255, 255, 255, 255);
    
    //first let's just draw the model with the model object
    //drawWithModel();
    easycam.begin();
    
    //then we'll learn how to draw it manually so that we have more control over the data
    drawWithMesh();
    
    easycam.end();
    

    }

//draw the model the built-in way
void ofApp::drawWithModel(){
    
    //get the position of the model
    ofVec3f position = model.getPosition();
    
    //save the current view
    ofPushMatrix();
    
    //center ourselves there
    ofTranslate(position);
    ofRotate(-ofGetMouseX(), 0, 1, 0);
    ofRotate(90,1,0,0);
    ofTranslate(-position);
    
    //draw the model
    model.drawFaces();
    
    //restore the view position
    ofPopMatrix();
}

//draw the model manually
void ofApp::drawWithMesh(){
    
    //get the model attributes we need
    ofVec3f scale = model.getScale();
    ofVec3f position = model.getPosition();
    float normalizedScale = model.getNormalizedScale();
    ofVboMesh mesh = model.getMesh(0);
    ofTexture texture;
    ofxAssimpMeshHelper& meshHelper = model.getMeshHelper( 0 );
    bool bHasTexture = meshHelper.hasTexture();
    if( bHasTexture ) {
        texture = model.getTextureForMesh(0);
    }
    
    ofMaterial material = model.getMaterialForMesh(0);
    
    ofPushMatrix();
    
    ofScale(normalizedScale, normalizedScale, normalizedScale);
    ofScale(scale.x,scale.y,scale.z);
    
    //modify mesh with some noise
    vector<ofVec3f>& verts = mesh.getVertices();
    for(unsigned int i = 0; i < verts.size(); i++){
        
        //if (i < verts.size()/2)
        /*{
         verts[i].x += 3;
         verts[i].y += 3;
         //verts[i].z +=*/
        
        
        int w = 1;
        float s = w/2;
        
        int x_n = 0;
        int y_n = 1;
        int z_n = 5;
        /*
         if ((verts[i].x > x_n * s && verts[i].x < x_n * s + s) && (verts[i].y > y_n * s && verts[i].y < y_n * s + s) && (verts[i].z > z_n * s && verts[i].z < z_n * s + s))
         {
         
         verts[i].x +=s;
         
         }
         
         
         if (verts[i].y > 0-position.y/2) {
         verts[i].y -= 0.04;
         
         }
         */
        
        if ((verts[i].z  < position.z / 10)  && (verts[i].z > position.z / 10 - 0.05)) {
         
            float pos = ofMap( spectrum[ i ], 0, 0.1, position.z-position.z/2, 800);
               ofLog(OF_LOG_NOTICE, "the number is " + ofToString(pos));
             verts[i].z = ofRandomf();
           // verts[i].z += ofMap(spectrum[i],0, 9.36154e+31, position.z-position.z/2, position.z-position.z/2);
            //verts[i].x -= 1;
            
        }
        
        
        /*
         verts[i].x += ofSignedNoise(verts[i].x/liquidness, verts[i].y/liquidness,verts[i].z/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
         verts[i].y += ofSignedNoise(verts[i].z/liquidness, verts[i].x/liquidness,verts[i].y/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;
         verts[i].z += ofSignedNoise(verts[i].y/liquidness, verts[i].z/liquidness,verts[i].x/liquidness, ofGetElapsedTimef()/speedDampen)*amplitude;*/
    }
    
    //draw the model manually
    if(bHasTexture) texture.bind();
    material.begin();
    //mesh.drawWireframe(); //you can draw wireframe too
    mesh.drawFaces();
    material.end();
    if(bHasTexture) texture.unbind();
    
    ofPopMatrix();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 
    
}
