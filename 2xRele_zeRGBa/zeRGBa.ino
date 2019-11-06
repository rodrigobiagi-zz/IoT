BLYNK_WRITE(zeRGBa) { // START Blynk Function
  r = param[0].asInt(); // get a RED channel value
  g = param[1].asInt(); // get a GREEN channel value
  b = param[2].asInt(); // get a BLUE channel value
  analogWrite(redPin, r);
  analogWrite(greenPin, g);
  analogWrite(bluePin, b);
  Blynk.virtualWrite(vRed, r);  // Red LED intensity
  Blynk.virtualWrite(vGreen, g);  // Green LED intensity
  Blynk.virtualWrite(vBlue, b);  // Blue LED intensity
  
}  // END Blynk Function

BLYNK_WRITE(vRed)
{
  r = param.asInt();
  analogWrite(redPin, r);
  Blynk.virtualWrite(V0, r, g, b);
}

BLYNK_WRITE(vGreen)
{
  g = param.asInt();
  analogWrite(greenPin, g);
  Blynk.virtualWrite(V0, r, g, b);
}

BLYNK_WRITE(vBlue)
{
  b = param.asInt();
  analogWrite(bluePin, b);
  Blynk.virtualWrite(V0, r, g, b);
}
