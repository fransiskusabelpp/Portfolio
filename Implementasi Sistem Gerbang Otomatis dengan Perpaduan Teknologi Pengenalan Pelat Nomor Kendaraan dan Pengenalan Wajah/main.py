from ultralytics import YOLO
import cv2
from paddleocr import PaddleOCR
import sqlite3
import datetime
import time
import numpy as np
from simple_facerec import SimpleFacerec
import os
import random
import string
import serial

license_plate_detector = YOLO('model/license_plate_detector.pt')

# Initialize the PaddleOCR object
p_ocr = PaddleOCR(lang='en', det_db_box_thresh=0.6, det_db_unclip_ratio=3, use_angle_cls=False)

ser = serial.Serial('COM7', baudrate=9600)

sfr = SimpleFacerec()
sfr.load_encodings("known_faces_encoding.pkl")


conn_recognition = sqlite3.connect('database/recognition.db')
cursor_recognition = conn_recognition.cursor()

conn_images = sqlite3.connect('database/images.db')
cursor_images = conn_images.cursor()

today = datetime.date.today().strftime("%d_%m_%Y")
cursor_images.execute(f"CREATE TABLE IF NOT EXISTS '{today}' (name TEXT, plate_number TEXT, time TEXT, image_path TEXT)")


last_saved_time = time.time()
last_recognized_plate = None


cap = cv2.VideoCapture(0)
cap2 = cv2.VideoCapture(1)

frame_count = 0

while True:
    ret, frame = cap.read()
    if not ret:
        break

    ret2, frame2 = cap2.read()
    if not ret2:
        break

    name = "" 
    plate_text = ""

    license_plates = license_plate_detector(frame)[0]

    for license_plate in license_plates.boxes.data.tolist():
        x1, y1, x2, y2, score, class_id = license_plate
        w, h = x2 - x1, y2 - y1

        plate_roi = frame[int(y1): int(y1 + h), int(x1): int(x1 + w)]
        cropped_plate_roi = plate_roi[5:70, 10:300]
        gray_image = cv2.cvtColor(cropped_plate_roi, cv2.COLOR_BGR2GRAY)
        filtered_image = cv2.GaussianBlur(gray_image, (5, 5), 0)
        print(score)

        if plate_roi is not None:
            cv2.imshow("ROI", filtered_image)

        # Perform text detection using PaddleOCR on the license plate ROI
        result = p_ocr.ocr(filtered_image)
        for idx in range(len(result)):
                res = result[idx]
                if res != None:
                    for line in res:
                        print(line)
        
        if result[0] is not None:
            result = result[0]
            plate_text = [line[1][0] for line in result]
            
            plate_text = ''.join(element.replace(' ', '') for element in plate_text)
            if len(plate_text) >= 3:
                last_three_chars = plate_text[-3:]  # Get the last three characters
                if '0' in last_three_chars:
                           plate_text = plate_text[:-3] + last_three_chars.replace('0', 'O')
            
            cv2.putText(frame, plate_text, (int(x1), int(y1) - 5), cv2.FONT_HERSHEY_COMPLEX_SMALL, 1, (255, 0, 255), 2)

            cursor_recognition.execute("SELECT * FROM data_pengguna WHERE plate_number=?", (plate_text,))

            if cursor_recognition.fetchone() is not None:
                if plate_text != last_recognized_plate:
                    last_recognized_plate = plate_text

                    if not os.path.exists('recognized_plates_faces'):
                        os.makedirs('recognized_plates_faces')

                    cv2.imwrite('temp/image1.jpg', frame)

        cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), (0, 255, 0), 2)

    face_locations, face_names = sfr.detect_known_faces(frame2)
    for face_loc, name in zip(face_locations, face_names):
        yy1, xx2, yy2, xx1 = face_loc[0], face_loc[1], face_loc[2], face_loc[3]

        cv2.putText(frame2, f'{name}',(xx1, yy1 - 10), cv2.FONT_HERSHEY_DUPLEX, 1, (0, 0, 200), 2)
        cv2.rectangle(frame2, (xx1, yy1), (xx2, yy2), (0, 0, 200), 4)

        cursor_recognition.execute("SELECT * FROM data_pengguna WHERE name=?", (name,))

        if cursor_recognition.fetchone() is not None:
            cv2.imwrite('temp/image2.jpg', frame2)

    check = 0

    cursor_recognition.execute("SELECT * FROM data_pengguna WHERE name=? AND plate_number=?", (name, plate_text))

    if cursor_recognition.fetchone() is not None:
        check = 1
        img1 = cv2.imread('temp\image1.jpg')
        img2 = cv2.imread('temp\image2.jpg')
        con = np.concatenate((img1, img2), axis=1)           

        image_folder = os.path.join('recognized_plates_faces', ''.join(random.choices(string.ascii_uppercase + string.digits, k=10)))
        os.makedirs(image_folder)
                    
        image_path = os.path.join(image_folder, ''.join(random.choices(string.ascii_uppercase + string.digits, k=10)) + '.jpg')
        cv2.imwrite(image_path, con)

        now = datetime.datetime.now()

        now_str = now.strftime("%H:%M:%S")
        cursor_images.execute(f"INSERT INTO '{today}' (name, plate_number, time, image_path) VALUES (?, ?, ?, ?)", (name, plate_text, now_str, image_path))
        conn_images.commit()     

    if(check == 1):
        print("Nomor plat dan wajah yang diinginkan terdeteksi!")
        data_to_send = "1" + ":" + name + "\n"
        ser.write(data_to_send.encode())  # Mengirim data ke Arduino
        print(data_to_send.encode())

    else:
        print("tidak terdeteksi")
        name = ""
        data_to_send = "0" + ":" + name + "\n"
        ser.write(data_to_send.encode())  # Mengirim data ke Arduino
        print(data_to_send.encode())                                                

    cv2.imshow("Detected Plates", frame)
    cv2.imshow("Detected Faces", frame2)


    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cap2.release()
cv2.destroyAllWindows()

conn_recognition.close()
conn_images.close()