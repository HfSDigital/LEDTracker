#include "blob.h"

int blob::ID_Counter;
uint64_t blob::timeout = 3000;

blob::blob(float x, float y)
{
	lastTimeSeenAlive = ofGetSystemTimeMillis();
	id = ID_Counter;
	ID_Counter++;
	position.x = x;
	position.y = y;
}

blob::~blob()
{
}

void blob::draw() 
{
	ofNoFill();
	ofSetColor(0, 255, 0);
	ofCircle(position.x, position.y, 10);
	stringstream ss;
	ss << id;
	ofDrawBitmapString(ss.str(), position.x-30, position.y);
}

void blob::updatePos(float x, float y)
{
	position.x = x;
	position.y = y;
	lastTimeSeenAlive = ofGetSystemTimeMillis();
}