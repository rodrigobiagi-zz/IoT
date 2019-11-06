// Digital clock display of the time
String printDigits(int digits){
  String new_digits;
  if (digits < 10) {
    new_digits += "0";
  }
  new_digits += String(digits);
  return new_digits;
}

void clockDisplay(){
  showWeek = DayOfWeek[weekday()-1];
  showMonth = Month[month()-1];

  currentTime = String(printDigits(hour())) + ":" + printDigits(minute());
  currentDate = String(showWeek) + " - " + printDigits(day()) + "/" + showMonth + "/" + year();
  Serial.print("Current time: ");
  Serial.print(showWeek);
  Serial.print(" ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();

  // Send time to the App
  Blynk.virtualWrite(V1, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V2, currentDate);
}
