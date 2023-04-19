#Import pymysql module library
import pymysql

def storeData(mesg):
  #Create a connection to MySQL Database 
  conn =pymysql.connect(database="senorData",user="user",password="pass123",host="localhost")

  #Create a MySQL Cursor to that executes the SQLs
  cur=conn.cursor()
  #Create a dictonary containing the fields, name, age and place
  data={'topic':'IOT/test','data':mesg}
  #Execute the SQL to write data to the database
  cur.execute("INSERT INTO MQTTdata(topic , data)VALUES(%(topic)s,%(data)s);",data)
  print("Data added")
  #Close the cursor
  cur.close()
  #Commit the data to the database
  conn.commit()
  #Close the connection to the database
  conn.close()

#Open phpMyAdmin and see how the data is stored to the database
