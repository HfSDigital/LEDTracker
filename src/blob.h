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
	void mouseClicked(int mouseX, int mouseY, int button);
	void setDirection(int mouseX, int mouseY);

	//--------------------------------------------------------------

	int id;
	ofVec2f position, 
			old_position,
			direction,
			destinyPosition,
			destinyDirection;

	uint64_t lastTimeSeenAlive;
	
	uint64_t lastTimeUpdated;
	static uint64_t updateInterval;

	bool isMouseOver = false;
	bool isSelected = false;
	bool isDestinySet = false;
	shared_ptr<shoe> pairedShoe;

	
	float m1, m2;
	float angle1, angle2;
	int quadrant;
	int motor1, motor2;
	float speedRatioM1M2;
};

