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

void blob::draw(float minArea, float maxArea, float nConsidered) 
{
	ofNoFill();
	// draw min, max and considered areas
	ofSetColor(255, 0, 0);
	float sqrtMinArea = sqrt(minArea);
	ofRect(position.x - sqrtMinArea / 2, position.y - sqrtMinArea / 2, sqrtMinArea, sqrtMinArea);

	ofSetColor(255, 255, 0);
	float sqrtMaxArea = sqrt(maxArea);
	ofRect(position.x - sqrtMaxArea / 2, position.y - sqrtMaxArea / 2, sqrtMaxArea, sqrtMaxArea);

	ofSetColor(255, 255, 255);
	float sqrtNConsidered = sqrt(nConsidered);
	ofRect(position.x - sqrtNConsidered / 2, position.y - sqrtNConsidered / 2, sqrtNConsidered, sqrtNConsidered);

    stringstream ss;
	ss << id;
	ofSetColor(0, 255, 0);
	ofDrawBitmapString(ss.str(), position.x-30, position.y);
}

void blob::updatePos(float x, float y)
{
	position.x = x;
	position.y = y;
	lastTimeSeenAlive = ofGetSystemTimeMillis();
}

