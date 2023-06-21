float pm25, pm10;
SdsDustSensor sds(Serial2);

void saveReadings_SDS011(float pm25_new, float pm10_new){
  pm25 = pm25_new;
  pm10 = pm10_new;
}

void setup_SDS011() {
  sds.begin();
  Serial.println(sds.queryFirmwareVersion().toString()); // prints firmware version
  Serial.println(sds.setActiveReportingMode().toString()); // ensures sensor is in 'active' reporting mode
  Serial.println(sds.setCustomWorkingPeriod(customWorkingPeriod_SDS011_minutes).toString()); // sensor sends data every 5 minutes
}

void loop_SDS011() {
  PmResult pm = sds.readPm();
  if (pm.isOk()) {
    saveReadings_SDS011(float(pm.pm25), float(pm.pm10));
    if (isnan(pm.pm25) || isnan(pm.pm10) || pm.pm25 == 0.00 || pm.pm10 == 0.00) { //don't send to thingsboard for PM2.5 and 10 if 0 or no value

    }
    else {
      tb.sendTelemetryFloat("N1 PM2.5", pm.pm25);
      tb.sendTelemetryFloat("N1 PM10", pm.pm10);
    }
    // if you want to just print the measured values, you can use toString() method as well
    Serial.println(pm.toString() + " ug/m3");
    //Serial.println(pm.toString());
  } else {
    // notice that loop delay is set to 5s (sensor sends data every 3 minutes) and some reads are not available
    // Serial.print("Could not read values from sensor, reason: ");
    // Serial.println(pm.statusToString());
  }
}