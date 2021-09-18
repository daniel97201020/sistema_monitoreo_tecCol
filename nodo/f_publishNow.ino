void publishNow(String payload)
{
  Serial.println("publishNow");
  char attributes[200];
  payload.toCharArray(attributes, 200);
  client.publish(TOPIC_ID, attributes);
  Serial.println(attributes);
}
