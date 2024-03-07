
#include <ArduinoJson.h>

String out;
int Time = 10;//s

void setup() {
  // Initialize Serial port
  Serial.begin(9600);
  while (!Serial) continue;





}

void loop() {
  StaticJsonDocument<256> doc;

  // StaticJsonObject allocates memory on the stack, it can be
  // replaced by DynamicJsonDocument which allocates in the heap.
  //
  // DynamicJsonDocument  doc(200);

  // Add values in the document
  //
  doc["id"] = "n01";
  doc["to"]   = millis();

  // Add an array.
  //
  JsonObject data = doc.createNestedObject("data");
  data["flux"] = (random(10)+(random(100)/100.0));
  data["temp"]= (random(100)+(random(100)/100.0));

  // Generate the minified JSON and send it to the Serial port.
  //
  serializeJson(doc, out);
  Serial.println(out);
  out = "";
  // The above line prints:
  // {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}

  // Start a new line
  delay(Time*1000);
}
