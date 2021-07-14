
#define STEP 6
#define DIR 7
#define RUNSWITCH 4
#define DIRSWITCH 2
#define SPEEDPOT 0

void setup() {
    Serial.begin(9600);
    pinMode(DIR, OUTPUT);
    pinMode(STEP, OUTPUT);
    pinMode(RUNSWITCH, INPUT_PULLUP);
    pinMode(DIRSWITCH, INPUT);

}

// tokenize(s) split s into global array cmd 
// return number of tokens
#define MAX_ARGSZ 15
#define MAX_ARGS 4
char cmd[MAX_ARGS][MAX_ARGSZ+1];
int tokenize(char *s) {
    int i = 0;
    char *p;
    if(strlen(s) == 0) {
        return 0;
    }
    strncpy(cmd[i], strtok(s, " \t"), MAX_ARGSZ);
    cmd[i][MAX_ARGSZ] = '\0';
    i++;
    while ((p = strtok(NULL, " \t")) != NULL && i < MAX_ARGS) {
        strncpy(cmd[i], p, MAX_ARGSZ);
        cmd[i][MAX_ARGSZ] = '\0';
        i++;
    }
    return i;
}
// get_command() reads from serial until a line is complete
// do not block
// returns tokenize(line) or 0 if not yet a full line 
int get_command(){
    static char line[MAX_ARGS * (MAX_ARGSZ + 1) + 1];
    static int lineix = 0;
    if (Serial.available())  {
        char c = Serial.read(); 
        if (c == '\r') {  
            Serial.read(); //gets rid of the following \r
            line[lineix] = '\0';
            lineix = 0;
            return tokenize(line);
        } else {     
            line[lineix] = c; 
            lineix++;
        }
    }
    return 0;
}

void loop() {
    int speed = 0, xval, speedpot, new_speed;
    bool dirswitch, runswitch, flipper = false;
    unsigned long stime = 0, ntime = 0;
    float a = 0.04, b = 100;
    stime = millis();

    while (1) {
        int i = get_command();
	speedpot = analogRead(SPEEDPOT);
	runswitch = ! digitalRead(RUNSWITCH);
	dirswitch = digitalRead(DIRSWITCH);

	new_speed = int((a * pow(b,float(speedpot)/1023.0) - a)*1023.0);

	/* if ( speedpot < 512 ) { */
	/*     new_speed = speedpot/4; */
	/* } else { */
	/*     new_speed = speedpot * 4 - (2048 - 128); */
	/* } */

	if ( runswitch == false ) {
	    new_speed = 0;
	}
	if ( abs(new_speed - speed) > 3) {
	    speed = new_speed;
	    if ( speed < 31 ) {
		noTone(STEP);
	    } else {
		tone(STEP, speed);
	    }
	    Serial.print("speed: ");
	    Serial.println(speed);
	}
	if ( speed < 31 ) {
	    ntime = millis();
	    if ( (ntime - stime ) > (500/speed) ) {
		stime = ntime;	
		digitalWrite(STEP, flipper);
		flipper = !flipper;
	    }
	}

        digitalWrite(DIR, dirswitch);
	
        if (i > 0) {
            if (strcmp(cmd[0], "status") == 0) {
                Serial.print("speed: ");
                Serial.println(speed);
                Serial.print("SPEEDPOT: ");
                Serial.println(speedpot);
                Serial.print("DIRSWITCH: ");
                Serial.println(dirswitch);
                Serial.print("RUNSWITCH: ");
                Serial.println(runswitch);
            }
        }
    }
}
