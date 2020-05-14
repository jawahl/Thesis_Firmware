from firebase import firebase
import firebase_admin
from firebase_admin import credentials
from firebase_admin import db
import base64

cred = credentials.Certificate("C:\Users\jawah\Desktop\wahl-thesis-firebase-adminsdk-d59r7-66f5eead9e.json")
firebase_admin.initialize_app(cred, {'databaseURL' : 'https://wahl-thesis.firebaseio.com/'})
PATH = 'Gateway-Node/Image'


files = db.reference('Testimage')
getImage = files.get()

imString = getImage[12:] # take "file,base64," out, only image data left
decoded = base64.b64decode(imString)
f = open('image.jpg', 'wb') # needs to write binary data to file to view correctly
f.write(decoded)
f.close()

#files.delete()
