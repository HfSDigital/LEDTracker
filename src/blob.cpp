#include "blob.h"
#include "shoe.h"

int blob::ID_Counter = 0;
uint64_t blob::timeout = 3000;


blob::blob(float x, float y)
{
	lastTimeSeenAlive = ofGetSystemTimeMillis();
	id = ID_Counter;
	ID_Counter++;
	position.x = x;
	position.y = y;
	pairedShoe = nullptr;

}

blob::~blob()
{
}

void blob::draw(float minArea, float maxArea, float nConsidered, float threshold_blobRange)
{
	ofNoFill();
	// draw min, max and considered areas
	ofSetColor(255, 0, 0);
	float sqrtMinArea = sqrt(minArea);
	ofDrawRectangle(position.x - sqrtMinArea / 2, position.y - sqrtMinArea / 2, sqrtMinArea, sqrtMinArea);

	float sqrtMaxArea = sqrt(maxArea);
	if (isMarked) {
		ofFill();
		ofSetColor(255, 255, 0, 100);
		ofDrawRectangle(position.x - sqrtMaxArea / 2, position.y - sqrtMaxArea / 2, sqrtMaxArea, sqrtMaxArea);
	}
	if (isMouseOver && !isMarked)
	{
		ofFill();
		ofSetColor(255, 255, 0, 60);
		ofDrawRectangle(position.x - sqrtMaxArea / 2, position.y - sqrtMaxArea / 2, sqrtMaxArea, sqrtMaxArea);
	}

	ofNoFill();
	ofSetColor(255, 255, 0);
	ofDrawRectangle(position.x - sqrtMaxArea / 2, position.y - sqrtMaxArea / 2, sqrtMaxArea, sqrtMaxArea);

	ofNoFill();
	ofSetColor(255, 255, 255);
	float sqrtNConsidered = sqrt(nConsidered);
	ofDrawRectangle(position.x - sqrtNConsidered / 2, position.y - sqrtNConsidered / 2, sqrtNConsidered, sqrtNConsidered);

	ofNoFill();
	ofColor(0, 0, 255);
	ofCircle(position.x, position.y, threshold_blobRange);

    stringstream ss;
	ss << id << endl;
	if(pairedShoe != nullptr)
		ss << pairedShoe->name;

	ofSetColor(0, 255, 0);
	ofDrawBitmapString(ss.str(), position.x-30, position.y);
}

void blob::update(int mouseX, int mouseY, int mousePressed, float threshold_blobRange)
{
	if (ofDist(mouseX, mouseY, position.x, position.y) < threshold_blobRange)
	{
		isMouseOver = true;
	}
	else
	{
		isMouseOver = false;
	}
}

void blob::updatePos(float x, float y)
{
	position.x = x;
	position.y = y;
	lastTimeSeenAlive = ofGetSystemTimeMillis();
}

void blob::mouseClicked()
{
	if (isMouseOver)
		isMarked = !isMarked;
	else
		isMarked = false;
}