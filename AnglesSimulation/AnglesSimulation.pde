import processing.serial.*;
import peasy.*;
PeasyCam cam;

PVector incident;
PVector normal;
PVector reflected;

PVector incidentToDraw;
PVector normalToDraw;
PVector reflectedToDraw;

String [] lines;
int linesPtr;

// Diameter and radius of the circle
float d;
float r;

int FRAMES_N_UPDATE = 30;

boolean runSimulation;


String[] data;
int minTime;

int lastTime;

String reflectedPath = "data/reflected.txt";
float reflectedAlt, reflectedAz;

boolean serialEnabled;
boolean serialReady;
Serial serial;

void setup(){

	size(650,650,P3D);


	runSimulation = false;

	serialEnabled = false;

	// Load alt/az info for today
	loadDataForToday();

	linesPtr = indexForNow();


	String portName = "";

	if(args!=null){
		for(int i=0; i<args.length;i++){
		if(args[i].equals("--serial")){
			serialEnabled = true;
			portName = args[i+1];
		}
		if(args[i].equals("--sim")){
			runSimulation = true;
			linesPtr = 0;
		}
		}
	}

	if(serialEnabled){
		openSerialPort(portName);
	}




	d = 10;
	r = d/2;
	
	// Init camera
	cam = new PeasyCam(this,0,0,0,150);
	cam.setMinimumDistance(1);
	cam.setMaximumDistance(300);

	// Initialize vectors
	reflectedToDraw = new PVector();
	incidentToDraw = new PVector();
	normalToDraw = new PVector();
	incident = new PVector(1,1,1);
	normal = new PVector(1,1,0);
	normal.normalize();
	reflected = new PVector(0,0,0);

	/*
	reflectedAlt = 20;
	reflectedAz = 10;

	updateReflected();
	*/

	loadReflectedFromFile();

//	updateAnglesFromLine(linesPtr);

}

void loadReflectedFromFile(){
	String [] lines = loadStrings(reflectedPath);

	reflectedAlt = float(lines[0]);
	reflectedAz = float(lines[1]);

	println("Loading reflected data...");
	updateReflected();

//	println(String.format("Loaded Reflected data :%s",altAzFromVectorToString(reflected)));
}


void saveReflectedToFile(){
	String [] lines = new String[2];
	lines[0] = String.format("%f",reflectedAlt);
	lines[1] = String.format("%f",reflectedAz);

	saveStrings(reflectedPath,lines);


}

void updateReflected(){
	reflected.set(vectorFromAltAz(reflectedAlt,reflectedAz));
	println(String.format("Updated reflected %f %f",reflectedAlt, reflectedAz));
}

String [] getInfoFor(int h, int m){
	int index = indexForHourMinute(h,m);

	String [] datum = splitTokens(lines[index]);

	println(datum[0]+" "+datum[1]+" "+datum[2]);

	return datum;
}

int indexForNow(){
	return indexForHourMinute(hour(),minute());
}

int indexForHourMinute(int h, int m){
	return minutesFromHourMin(h,m) - minTime;
}

void loadDataForToday(){
	lines = loadStringsForToday();

	String [] tokens;
	tokens = splitTokens(lines[0]);

	String [] minTimeStr = split(tokens[0],':');

	minTime = minutesFromHourMin(int(minTimeStr[0]),int(minTimeStr[1]));

//	println(minTime);
}

int minutesFromHourMin(int h, int m){
	return h*60 + m;
}


String [] loadStringsForToday(){
	return loadStringsForDate(year(),month(),day());
}

String [] loadStringsForDate(int y, int m, int d){
	String url = String.format("http://aa.usno.navy.mil/cgi-bin/aa_altazw.pl?form=1&body=10&year=%d&month=%d&day=%d&intv_mag=1&state=NY&place=Brooklyn",y,m,d);

	println("Loading file from web");
	String [] lines = loadStrings(url);
	if(lines == null){
		println("Loading backup!");
		lines = loadStrings("data/backup.txt");
	}
	int linesToSkipFront = 41;
	int linesToSkipBack = 6;
	String [] data = new String[lines.length - linesToSkipFront -linesToSkipBack];

	for(int i=0; i<data.length; i++){
		data[i] = lines[i+linesToSkipFront];
	}
	return data;


}


void updateIncidentFromLine(int n){
	println("Reading line:");
	println(lines[n]);
	String [] tokens = splitTokens(lines[n]);
	float alt = float(tokens[1]);
	float az = float(tokens[2]);
	incident.set(vectorFromAltAz(alt,az));
}

void updateAnglesFromCurrentLine(){
	updateAnglesFromLine(linesPtr);
}

void updateAnglesFromLine(int n){
	updateIncidentFromLine(n);
	normal.set(normalFromIncidentAndReflected(incident,reflected));

	println("Updated normal to: ");
	println(altAzFromVectorToString(normal));
	altAzFromVectorToSerial(normal);
}



void updateReflectedWithIncidentAndNormal(){
	reflected.set(reflect(incident,normal));
	PVector r = altAzFromVector(reflected);
	reflectedAlt = r.x;
	reflectedAz = r.y;
	println(String.format("Updated reflected with incident and normal %f %f",reflectedAlt, reflectedAz));
}

String stringVector(PVector v){
	return String.format("%f %f %f",v.x,v.y,v.z);
}

String altAzFromVectorToString(PVector v){
	PVector a = altAzFromVector(v);
	float alt = a.x;
	float az = a.y;
	return String.format("%f %f",alt,az);

}

void altAzFromVectorToSerial(PVector v){
	PVector a = altAzFromVector(v);
	int alt = int((90-a.x)*10);
	int az = int((90-a.y)*10);

	alt = constrain(alt,0,900);
	az = constrain(az,0,1800);
//	println(String.format("Pitch and yaw to send %d %d",alt,az));

	String pitch = String.format("P%04d\n",alt);
	String yaw = String.format("Y%04d\n",az);


	if(serialEnabled && serialReady){
		println("Sending...");
		print(pitch);
		print(yaw);
		println("Received:");

		serial.write(pitch);
		delay(100);
		serial.write(yaw);
		delay(100);
	}

}

PVector altAzFromVector(PVector v){
	PVector a = new PVector();

	PVector vec = new PVector();
	vec.set(v);
	vec.normalize();
	float mag = vec.mag();

	float alt = degrees(asin(vec.z/mag));
	// projected magnitude in the plane
	float projmag = mag*cos(radians(alt));
//	float az = degrees(acos(vec.y/projmag));
	float az = degrees(atan2(vec.y,vec.x));

	a.set(alt,az);

	return a;
}



void updateNormalFromAltAz(float alt, float az){
	PVector newNormal = vectorFromAltAz(alt,az);
	newNormal.normalize();

	normal.set(newNormal);

}

// Create vector from altitude and azimuth angles (in deg)
PVector vectorFromAltAz(float alt, float az){
	float x,y,z;

	alt = radians(alt);
	az = radians(az);

	x = cos(az);
	y = sin(az);
	z = tan(alt);

	PVector result = new PVector(x,y,z);
	result.normalize();

	return result;
}

PVector normalFromIncidentAndReflected(PVector i, PVector r){
	// Result vector
	PVector result = new PVector();
	// Copy of i
	PVector ir = new PVector();

	// Magnitude of the vectors
	float magI = r.mag();
	// Cosine of the angle between i and n
	float cosa;


	// R and I have to have the same magnitude
	ir.set(i);
	ir.normalize();
	ir.mult(magI);

	// Calculate angle between i and r
	float air = acos(ir.dot(r) / (ir.mag()*r.mag()));


	// Angle between i and n is half of it
	cosa = cos(air/2);

	// n = (r+I)/(2*mag(I)*cosA)
	result.set(ir);
	result.add(r);
	result.div(2*magI*cosa);
	result.normalize();


	// Check error between original reflected and reflected
	/*
	PVector r1 = new PVector();
	r1.set(r);
	PVector r2 = reflect(i,n);
	r1.normalize();
	r2.normalize();
	println("Reflected");
	println(stringVector(r1));
	println(stringVector(r2));
	println("Error");
	println(stringVector(PVector.sub(r1,r2)));
	*/


//	println("Result");
//	println(stringVector(result));
//	println(altAzFromVectorToString(result));

	return result;

}

// Reflected vector
// From http://graphics.cs.cmu.edu/nsp/course/15-462/Spring04/slides/07-lighting.pdf
// r = 2*(i dot n)*n - i
// r - reflected vector
// n - normal vector
// i - incident vector
PVector reflect(PVector i, PVector n){
	float d = i.dot(n);
	PVector r = new PVector();
	r.set(n);
	r.mult(2*d);
	r.sub(i);

	return r;
}

void draw(){

	if(serialEnabled){
		if(serial.available()>0){
			print(serial.readChar());
			if(!serialReady){
				println("Serial ready!");
				serialReady = true;
				updateAnglesFromCurrentLine();
			}
		}

	}

	reflectedToDraw.set(reflected);
	reflectedToDraw.mult(r);

	incidentToDraw.set(incident);
	incidentToDraw.mult(r);

	normalToDraw.set(normal);
	normalToDraw.mult(2);

	background(20);
	noFill();

	strokeWeight(0.5);
	scale(10);



	beginShape(LINES);
	strokeWeight(0.3);
	// X
	stroke(255,0,0);
	vertex(0,0,0);
	vertex(1,0,0);
	// Y
	stroke(0,255,0);
	vertex(0,0,0);
	vertex(0,-1,0);
	// Z
	stroke(0,0,255);
	vertex(0,0,0);
	vertex(0,0,1);
	strokeWeight(0.5);
	// I
	stroke(255,255,0);
	vertex(0,0,0);
	vertex(incidentToDraw.x,incidentToDraw.y,incidentToDraw.z);
	// R
	stroke(255,128,0);
	vertex(0,0,0);
	vertex(reflectedToDraw.x,reflectedToDraw.y,reflectedToDraw.z);
	// N
	stroke(0,255,255);
	vertex(0,0,0);
	vertex(normalToDraw.x,normalToDraw.y,normalToDraw.z);
	// 4 cardinal points
	stroke(100);
	strokeWeight(0.1);
	vertex(0,-r,0);
	vertex(0,r,0);
	vertex(-r,0,0);
	vertex(r,0,0);
	endShape();

	stroke(100);
	ellipse(0,0,d,d);

	translate(incidentToDraw.x,incidentToDraw.y,incidentToDraw.z);
	fill(255,255,0);
	noStroke();
	sphere(0.5);


	if(runSimulation){
		if(frameCount%FRAMES_N_UPDATE==0){
			linesPtr = (linesPtr+1)%lines.length;
			updateAnglesFromCurrentLine();
			/*
			save(String.format("%04d.png",frameCount/FRAMES_N_UPDATE));
			if(linesPtr == 0){
				exit();
			}
			*/
	//		println(linesPtr);
		}
	}
	else{
		if(indexForNow() != linesPtr){
			println("--------------------");
			println(String.format("Updating for %02d:%02d",hour(),minute()));
			println("--------------------");
			linesPtr = indexForNow();
			updateAnglesFromCurrentLine();
		}
	}


}


void keyPressed(){
	boolean doUpdateReflected = false;
	boolean doUpdateNormal = false;

	PVector normalAltAz = altAzFromVector(normal);
	float normalAlt = normalAltAz.x;
	float normalAz = normalAltAz.y;
//	println("Normal alt,az from Vector "+normalAlt+" "+normalAz);


	switch(key){
		case 'w':
			reflectedAlt += 0.1;
			doUpdateReflected = true;
		break;
		case 's':
			reflectedAlt -= 0.1;
			doUpdateReflected = true;
		break;
		case 'a':
			reflectedAz -= 0.1;
			doUpdateReflected = true;
		break;
		case 'd':
			reflectedAz += 0.1;
			doUpdateReflected = true;
		break;
		case 'W':
			reflectedAlt += 1;
			doUpdateReflected = true;
		break;
		case 'S':
			reflectedAlt -= 1;
			doUpdateReflected = true;
		break;
		case 'A':
			reflectedAz -= 1;
			doUpdateReflected = true;
		break;
		case 'D':
			reflectedAz += 1;
			doUpdateReflected = true;
		break;

		case 'j':
			normalAlt -= 1;
			doUpdateNormal = true;
		break;

		case 'k':
			normalAlt += 1;
			doUpdateNormal = true;
		break;
		case 'h':
			normalAz -= 1;
			doUpdateNormal = true;
		break;

		case 'l':
			normalAz += 1;
			doUpdateNormal = true;
		break;

		/*
		case '0':
			openSerialPort("/dev/ttyACM0");
		break;
		case '1':
			openSerialPort("/dev/ttyACM1");
		break;
		*/
	}



	if(doUpdateNormal){
		updateNormalFromAltAz(normalAlt,normalAz);

		/*
		println("Normal alt,az");
		println(altAzFromVectorToString(normal));
		*/
		updateReflectedWithIncidentAndNormal();
		updateAnglesFromCurrentLine();
		saveReflectedToFile();
	}

	if(doUpdateReflected){
			updateReflected();
			updateAnglesFromCurrentLine();
			saveReflectedToFile();
	}


}


void openSerialPort(String portName){
		println("Opening serial port... "+portName);
		serial = new Serial(this,portName,115200);

		delay(1000);
		serialReady = false;


		println("Opened!");
		println(serial.readChar());
}
