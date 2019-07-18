#pragma once
#include "ofMain.h"
#include "shoe.h"

class blob
{
public:
	static uint64_t timeout;
	static int ID_Counter;

	blob(float x, float y);
	~blob();
	void draw(float minArea, float maxArea, float nConsidered);
	void updatePos(float x, float y);

	//--------------------------------------------------------------

	int id;
	ofVec2f position;
	uint64_t lastTimeSeenAlive;
	shared_ptr<shoe> connectedShoe;
};

