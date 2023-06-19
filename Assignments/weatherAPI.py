import json
from urllib.request import urlopen
import pymysql
#Create user account and obtain API key from https://www.weatherapi.com

def mysql_push(data):
  conn = pymysql.connect(database="senorData",user="user",password="pass123",host="localhost")
  cur = conn.cursor()
  
  cur.execute("INSERT INTO apiData(name, region, country, temperature_c, temperature_f, is_day, condition, wind_speed_mph, wind_speed_kph, humidity) VALUES (%(name)s, %(region)s, %(country)s, %(temperature_c)s, %(temperature_f)s, %(is_day)s, %(condition)s, %(wind_speed_mph)s, %(wind_speed_kph)s, %(humidity)s);", data)
  conn.commit()
  
  print("Data is stored tot the database")
  print('----------------------------------------------------')
  cur.close()
  conn.close() 

def getdata():
  url = "https://api.weatherapi.com/v1/current.json?key=f4a34d94e01244fe92f32019232905&q=kollam&aqi=no"
  api_page = urlopen(url)
  api=api_page.read()
  json_api=json.loads(api)

  print("----Raw Data----")
  print(json_api)

  print("----Parsed----")
  location = json_api['location']
  current = json_api['current']

  name = location['name']
  region = location['region']
  country = location['country']
  temperature_c = current['temp_c']
  temperature_f = current['temp_f']
  is_day = current['is_day']
  condition = current['condition']['text']
  wind_speed_mph = current['wind_mph']
  wind_speed_kph = current['wind_kph']
  humidity = current['humidity']

  data = {
      'name': name,
      'region': region,
      'country': country,
      'temperature_c': temperature_c,
      'temperature_f': temperature_f,
      'is_day': is_day,
      'Condition': condition,
      'wind_speed_mph': wind_speed_mph,
      'wind_speed_kph': wind_speed_kph,
      'humidity': humidity
      }
  
  return data

print('----------------------------------------------------')
print('Get weather update from API press 1')
print('----------------------------------------------------')

while 1:
  res = int(input())
  if res==1:
    dictionary = getdata()
    mysql_push(dictionary)
  else:
    break
