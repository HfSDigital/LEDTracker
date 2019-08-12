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
	old_position = position;
	pairedShoe = nullptr;
}

blob::~blob()
{
}

void blob::draw(float minArea, float maxArea, float nConsidered, float threshold_blobRange)
{
	// draw Min-Area
	ofNoFill();
	ofSetColor(255, 0, 0);
	float sqrtMinArea = sqrt(minArea);
	ofDrawRectangle(position.x - sqrtMinArea / 2, position.y - sqrtMinArea / 2, sqrtMinArea, sqrtMinArea);

	// draw MouseOver and Selected
	float sqrtMaxArea = sqrt(maxArea);
	if (isSelected) {
		ofFill();
		ofSetColor(255, 255, 0, 100);
		ofDrawRectangle(position.x - sqrtMaxArea / 2, position.y - sqrtMaxArea / 2, sqrtMaxArea, sqrtMaxArea);
	}
	if (isMouseOver && !isSelected)
	{
		ofFill();
		ofSetColor(255, 255, 0, 60);
		ofDrawRectangle(position.x - sqrtMaxArea / 2, position.y - sqrtMaxArea / 2, sqrtMaxArea, sqrtMaxArea);
	}

	// draw Max-Area
	ofNoFill();
	ofSetColor(255, 255, 0);
	ofDrawRectangle(position.x - sqrtMaxArea / 2, position.y - sqrtMaxArea / 2, sqrtMaxArea, sqrtMaxArea);

	// draw nConsidered-Area
	ofNoFill();
	ofSetColor(255, 255, 255);
	float sqrtNConsidered = sqrt(nConsidered);
	ofDrawRectangle(position.x - sqrtNConsidered / 2, position.y - sqrtNConsidered / 2, sqrtNConsidered, sqrtNConsidered);

	// draw blob-Range
	ofNoFill();
	ofSetColor(0, 0, 255);
	ofCircle(position.x, position.y, threshold_blobRange);

	// draw ID and Name
    stringstream ss;
	ss << id << endl;
	if(pairedShoe != nullptr) ss << pairedShoe->name;
	ofSetColor(0, 255, 0);
	ofDrawBitmapString(ss.str(), position.x-30, position.y);

	// draw direction Vector
	ss.str("");
	ss << "Direction: " << setprecision(2) << direction;
	
	ofSetColor(255);
	

	ofSetLineWidth(2);

	ofDrawLine(position, position + direction * 60);

	ofPushMatrix();
	ofTranslate(position + direction * 60);
	ofDrawBitmapString(ss.str(), direction*5);
	ofRotate(direction.angle(ofVec2f(0, 1)) * -1.0 + 45.0 + 180.0);
	ofDrawLine(0, 0, 15, 0);
	ofDrawLine(0, 0, 0, 15);
	ofPopMatrix();
	ofSetLineWidth(1);
}

void blob::update(int mouseX, int mouseY, int mousePressed, float threshold_blobRange)
{
	isMouseOver = ofDist(mouseX, mouseY, position.x, position.y) < threshold_blobRange ? true : false;
	//if (ofDist(mouseX, mouseY, position.x, position.y) < threshold_blobRange)
	//{
	//	isMouseOver = true;
	//}
	//else
	//{
	//	isMouseOver = false;
	//}
	if (ofDist(position.x, position.y, old_position.x, old_position.y) > 15) 
	{
		direction.x = position.x - old_position.x;
		direction.y = position.y - old_position.y;
		cout << "updated direction: " << direction;
		direction.normalize();
		old_position = position;
		cout << "\tpos: " << position << "\told_pos: " << old_position << endl;
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
		isSelected = !isSelected;
	else
		isSelected = false;
}