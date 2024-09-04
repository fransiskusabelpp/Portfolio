
import cv2

import sqlite3

from simple_facerec import SimpleFacerec



sfr = SimpleFacerec()
sfr.load_encodings("known_faces_encoding.pkl")

conn_recognition = sqlite3.connect('database/recognition.db')
cursor_recognition = conn_recognition.cursor()


cap2 = cv2.VideoCapture(0)

while True:
    ret2, frame2 = cap2.read()
    if not ret2:
        break


    face_locations, face_names = sfr.detect_known_faces(frame2)
    for face_loc, name in zip(face_locations, face_names):
        yy1, xx2, yy2, xx1 = face_loc[0], face_loc[1], face_loc[2], face_loc[3]

        cv2.putText(frame2, f'{name}',(xx1, yy1 - 10), cv2.FONT_HERSHEY_DUPLEX, 1, (0, 0, 200), 2)
        cv2.rectangle(frame2, (xx1, yy1), (xx2, yy2), (0, 0, 200), 4)

        cursor_recognition.execute("SELECT * FROM data_pengguna WHERE name=?", (name,))

        if cursor_recognition.fetchone() is not None:
            cv2.imwrite('temp/image2.jpg', frame2)


    cv2.imshow("Detected Faces", frame2)


    if cv2.waitKey(1) & 0xFF == ord('q'):
        break


cap2.release()
cv2.destroyAllWindows()
conn_recognition.close()
