// Illustrates how to use hardware serial ports with Thunderbolt
#define IEEE754_4BYTE_PRECISION	// comment out if there is double precision support on the device 
#include "Thunderbolt.h"

// ---------------------------
// Devices 
// ---------------------------
Thunderbolt tbolt(Serial2); // Use HardwareSerial #2 

// ---------------------------
// Globals
// ---------------------------
GPSStatus prevStatus;
GPSTime prevTime;

// Display version info
void display_version(GPSVersion ver) {
	char buf[50];
	sprintf(buf, "Thunderbolt ver: app:%d.%02d core:%d.%02d\n", ver.app.major_ver, ver.app.minor_ver, ver.core.major_ver, ver.core.minor_ver);
	Serial.print(buf);
}

// Display status
void display_status(GPSStatus s) {
	Serial.print("Status: ");
	Serial.print(s.rcvr_status);
	Serial.print(" Mode = ");
	Serial.print(s.rcvr_mode);
	Serial.print(" Lat = ");
	Serial.print(s.latitude * RAD_TO_DEG);
	Serial.print(" Lng = ");
	Serial.print(s.longitude * RAD_TO_DEG);
	Serial.print(" Alt = ");
	Serial.print(s.altitude * METERS_TO_FEET);
	Serial.print((s.critical_alarms) ? " (Critical Alarm!)" : "");
	Serial.println((s.minor_alarms) ? " (Minor Alarm!)" : "");
}

// Display UTC time
void displayTime(GPSTime t) {
	char buf[20];
	sprintf(buf, "time: %02d:%02d:%02d\n", t.Hour, t.Minute, t.Second);
	Serial.print(buf);
}

void setup() {
	Serial.begin(115200);
	Serial2.begin(TSIP_BAUD_RATE);

	tbolt.flush();
	if (tbolt.getSoftwareVersionInfo() == false) // this call is synchronous (waits for a response - but will timeout)
	{
		Serial.println("Unable to start Thunderbolt, nothing more to do.");
		while (1);
	}
	display_version(tbolt.getVersion());
	tbolt.setPacketBroadcastMask(PBM_TIMING_ONLY);
	tbolt.setFixMode(RPT_FIX_POS_LLA_64, RPT_FIX_VEL_ENU, ALT_MSL, PPS_FIX, TME_UTCTIME);
}

void loop()
{
	GPSStatus status = tbolt.getStatus();
	if (prevStatus != status) {
		display_status(status);
		prevStatus = status;
	}

	GPSTime time = tbolt.getGPSTime();
	if (prevTime != time) {
		displayTime(time);
		prevTime = time;
	}
}

// Use handy built-in serial event (hardware serial only), NOT interrupt driven, called once each loop when serial data is available
void serialEvent2() {
	tbolt.readSerial(); // Thunderbolt class serial in
}
