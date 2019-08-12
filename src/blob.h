#pragma once
#include "ofMain.h"

class shoe;

class blob
{
public:
	static uint64_t timeout;
	static int ID_Counter;

	blob(float x = -1, float y = -1);
	~blob();
	void draw(float minArea, float maxArea, float nConsidered, float threshold_blobRange);
	void update(int mouseX, int mouseY, int mousePressed, float threshold_blobRange);
	void updatePos(float x, float y);
	void mouseClicked();
	void setDirection(int mouseX, int mouseY);

	//--------------------------------------------------------------

	int id;
	ofVec2f position;
	ofVec2f direction;
	uint64_t lastTimeSeenAlive;
	bool isMouseOver = false;
	bool isMarked = false;
	shared_ptr<shoe> pairedShoe;
};

