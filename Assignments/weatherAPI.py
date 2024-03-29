import json
import time
from urllib.request import urlopen
import pymysql
#Create user account and obtain API key from https://www.weatherapi.com

timer = lambda seconds: [time.sleep(1) for _ in range(seconds)]

def mysql_push(data):
  conn = pymysql.connect(database="senorData",user="user",password="pass123",host="localhost")
  cur = conn.cursor()
  
  cur.execute("INSERT INTO apiData(name, region, country, temperature_c, temperature_f, is_day, w_Condition, wind_speed_mph, wind_speed_kph, humidity) VALUES (%(name)s, %(region)s, %(country)s, %(temperature_c)s, %(temperature_f)s, %(is_day)s, %(w_Condition)s, %(wind_speed_mph)s, %(wind_speed_kph)s, %(humidity)s);", data)
  conn.commit()
  print('------------------------------------')
  print("Data is stored to the database")
  print('------------------------------------')
  cur.close()
  conn.close() 

def getdata():
  url = "https://api.weatherapi.com/v1/current.json?key=f4a34d94e01244fe92f32019232905&q=kollam&aqi=no"
  api_page = urlopen(url)
  api=api_page.read()
  json_api=json.loads(api)

  print("----Raw Data----")
  print('--------------------------------------------------------------')
  print(json_api)
  print('--------------------------------------------------------------')
  
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
      'w_Condition': condition,
      'wind_speed_mph': wind_speed_mph,
      'wind_speed_kph': wind_speed_kph,
      'humidity': humidity
      }
  json_string = json.dumps(data, indent=4)
  print('--------------------------------------------------------------')
  print(json_string)
  print('--------------------------------------------------------------')

  return data

print('----------------------------------------------------')
print('Get weather update from API press 1')
print('----------------------------------------------------')

while 1:
  timer(5)
  res=1
  if res==1:
    dictionary = getdata()
    mysql_push(dictionary)
  else:
    break
