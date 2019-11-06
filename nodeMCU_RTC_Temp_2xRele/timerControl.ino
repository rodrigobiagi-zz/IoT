BLYNK_WRITE(V20) {
  TimeInputParam t(param);
  // Process start time
  if (t.hasStartTime()){
    Serial.println(String("Start: ") +
                   t.getStartHour() + ":" +
                   t.getStartMinute() + ":" +
                   t.getStartSecond());
    digitalWrite(rele03Pin, LOW);
  }
  else if (t.isStartSunrise()){
    Serial.println("Start at sunrise");
    digitalWrite(rele03Pin, LOW);
  }
  else if (t.isStartSunset()){
    Serial.println("Start at sunset");
    digitalWrite(rele03Pin, LOW);
  }
  else{
    // Do nothing
  }
  // Process stop time
  if (t.hasStopTime()){
    Serial.println(String("Stop: ") +
                   t.getStopHour() + ":" +
                   t.getStopMinute() + ":" +
                   t.getStopSecond());
    //digitalWrite(rele03Pin, HIGH);
  }
  else if (t.isStopSunrise()){
    Serial.println("Stop at sunrise");
    //digitalWrite(rele03Pin, HIGH);
  }
  else if (t.isStopSunset()){
    Serial.println("Stop at sunset");
    //digitalWrite(rele03Pin, HIGH);
  }
  else{
    // Do nothing: no stop time was set
  }

  // Process timezone
  // Timezone is already added to start/stop time

  Serial.println(String("Time zone: ") + t.getTZ());

  // Get timezone offset (in seconds)
  Serial.println(String("Time zone offset: ") + t.getTZ_Offset());

  // Process weekdays (1. Mon, 2. Tue, 3. Wed, ...)

  for (int i = 1; i <= 7; i++) {
    if (t.isWeekdaySelected(i)) {
      Serial.println(String("Day ") + i + " is selected");
    }
  }
  Serial.println();
}
