#include "blob.h"
#include "shoe.h"

int blob::ID_Counter = 0;
uint64_t blob::timeout = 3000;
uint64_t blob::updateInterval = 500;

blob::blob(float x, float y)
{
	id = ID_Counter;
	ID_Counter++;
	position.x = x;
	position.y = y;
	old_position = position;
	pairedShoe = nullptr;
	lastTimeSeenAlive = ofGetSystemTimeMillis();
	lastTimeUpdated = ofGetElapsedTimeMillis();
}

blob::~blob()
{
}

void blob::draw(float minArea, float maxArea, float nConsidered, float threshold_blobRange)
{
	stringstream ss;
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
		ofDrawCircle(position.x, position.y, threshold_blobRange);

		// draw ID and Name
		ss << id << endl;
		if (pairedShoe != nullptr) ss << pairedShoe->name;
		ofSetColor(0, 255, 0);
		ofDrawBitmapString(ss.str(), position.x - 30, position.y);

		// draw direction Vector
		ss.str("");
		ss << "Direction: " << setprecision(2) << direction;
		ofSetColor(255);
		ofDrawLine(position, position + direction * 60);
		ofPushMatrix();
		ofTranslate(position + direction * 60);
		ofDrawBitmapString(ss.str(), direction * 5);
		ofRotateDeg(direction.angle(ofVec2f(0, 1)) * -1.0 + 45.0 + 180.0);
		ofDrawLine(0, 0, 15, 0);
		ofDrawLine(0, 0, 0, 15);
		ofPopMatrix();
	}
	
	// draw Destiny
	//if (isDestinySet) {
		ofPushMatrix();
		ofTranslate(destinyPosition);
		ofFill();
		ofSetColor(255, 0, 0);
		ofDrawCircle(0, -20, 10);
		ss.str("");
		ss << "Destiny:\t" << setprecision(0) << destinyPosition << endl;
		ss << "Richtungsvektor:\t" << setprecision(2) << direction << endl;
		ss << "Vektor Position->Ziel:\t" << setprecision(2) << destinyDirection /* ofVec2f((destinyPosition.x - position.x), (destinyPosition.y - position.y)) */<< endl;
		ss << "Steigung des Richtungsvektors:\t" << m1 << endl;
		ss << "Steigung Vektor Position->Ziel:\t" << m2 << endl;
		ss << "angle1: " << setprecision(0) << angle1 << endl;
		ss << "angle2: " << setprecision(0) << angle2 << endl;
		ss << "quadrant: " << quadrant << endl;
		ss << "speedRatioM1M2: " << speedRatioM1M2  <<"\tmotor1: " << motor1 << "\tmotor2: " << motor2 << endl;
		ss << "distance to destiny: " << ofDist(position.x, position.y, destinyPosition.x, destinyPosition.y) << endl;
		ofDrawBitmapString(ss.str(), 10, -20);
		ofNoFill();
		ofSetColor(222);
		ofSetLineWidth(3);
		ofDrawLine(0, -20, 0, 0);
		ofSetLineWidth(1);
		ofPopMatrix();
	//}
}

void blob::update(int mouseX, int mouseY, int mousePressed, float threshold_blobRange)
{
	isMouseOver = ofDist(mouseX, mouseY, position.x, position.y) < threshold_blobRange ? true : false;

	// update Direction Vector
	if (ofDist(position.x, position.y, old_position.x, old_position.y) > 15) 
	{
		direction.x = position.x - old_position.x;
		direction.y = position.y - old_position.y;
		direction.normalize();
		old_position = position;
	}


	// Calc way if a destiny is set
	if (isDestinySet && ((lastTimeUpdated + updateInterval) < ofGetElapsedTimeMillis())) {
		// calculate way 
		// Zunächst berechnen wir den Winkel zwischen beiden Geraden
		m1 = direction.y / direction.x;		// Steigung des Richtungsvektors der aktuellen Beweung
		m2 = (destinyPosition.y - position.y) / (destinyPosition.x - position.x); // Steigung der Geraden zwischen aktueller Position und Ziel

		float angle_ = (m1 - m2) / (1 + m1 * m2);
		angle1 = atan(angle_) * 180 / PI;
		//.. das ↑ hat nicht so gut funktioniert. Vielleicht sollten wir
		// DEN WINKEL ZWISCHEN ZWEI VEKTOREN ↓ berechnen (wobei v1 = direction und v2 = destinty->position) ?
		// schau, wie sie es hier machen https://www.mathebibel.de/winkel-zwischen-zwei-vektoren
		//ofVec2f _dirNormalized = direction.getNormalized();
		destinyDirection = ofVec2f(destinyPosition - position);
		destinyDirection.normalize();
		float dot = direction.dot(destinyDirection);
		float a = acos(dot / (abs(direction.length()) * abs(destinyDirection.length())));
		angle2 = a * 180 / PI;

		// Hey cool, mit der anderen Winkel-Berechnung sehe ich, dass alles unter 90° *vor* mir liegt, alles 
		// über 90° *hinter* mir. morgen früh mache ich die if-schleife dazu!
		// ...eine Woche später:

		/*         direction
		              ↑
		Q4            |	 Q1
		angle1 > 0	  |  angle1 < 0
		angle2 < 90   |  angle2 < 90
		--------------|----------------
		Q3            |	 Q2
		angle1 < 0	  |  angle1 > 0
		angle2 > 90   |  angle2 > 90
		              |
		*/
		if (angle1 > 0) { // Q2 oder Q4
			if (angle2 < 90) { // Q4
				quadrant = 4;
			}
			else { //Q2
				quadrant = 2;
			}
		}
		else // Q1 oder Q3
		{
			if (angle2 < 90) { // Q1
				quadrant = 1;
			}
			else{ //Q3
				quadrant = 3;
			}
		} 

		// angle2 gibt mir an, um wieviel stärker sich das eine Rad als das andere drehen soll.
		// der Quadrant gibt mir an, welches der beiden Räder sich stärker drehen soll
		
		int speed = 10;
		speedRatioM1M2 = ofMap(angle2, 0, 180, 1, 0, true);
		speedRatioM1M2 = ofClamp(speedRatioM1M2, 0.3, 1.0);
		motor1 = speed / speedRatioM1M2;
		motor2 = speed * speedRatioM1M2;
		pairedShoe->drive(motor1, motor2);
		cout << "*";
		lastTimeUpdated = ofGetElapsedTimeMillis();
	}

	if (ofDist(position.x, position.y, destinyPosition.x, destinyPosition.y) < 40 && isDestinySet)
	{
		cout << "Ziel erreicht, Stoppe Motoren." << endl;
		pairedShoe->stop();
		isDestinySet = false;
	}

}

void blob::updatePos(float x, float y)
{
	position.x = x;
	position.y = y;
	lastTimeSeenAlive = ofGetSystemTimeMillis();
}

void blob::mouseClicked(int mouseX, int mouseY, int button)
{
	if (isMouseOver)
	{
		isSelected = !isSelected;
		isDestinySet = false;
	}
	else 
	{
		if (isSelected && direction.length() > 0) { // oha, er war selektiert! dann wollen wir doch mal den Weg kalkulieren!
			destinyPosition = ofVec2f(mouseX, mouseY);
			isDestinySet = true;
		}
		isSelected = false;
	}
}