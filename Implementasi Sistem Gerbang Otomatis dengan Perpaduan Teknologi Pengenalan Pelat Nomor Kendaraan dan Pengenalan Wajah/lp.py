from ultralytics import YOLO
import cv2
from paddleocr import PaddleOCR

import numpy as np

license_plate_detector = YOLO('model/license_plate_detector.pt')

p_ocr = PaddleOCR(lang='en', det_db_box_thresh=0.6, det_db_unclip_ratio=3, use_angle_cls=False)

cap = cv2.VideoCapture(0)

while True:
    ret, frame = cap.read()
    if not ret:
        break


    name = "" 
    plate_text = ""

    license_plates = license_plate_detector(frame)[0]

    for license_plate in license_plates.boxes.data.tolist():
        x1, y1, x2, y2, score, class_id = license_plate
        w, h = x2 - x1, y2 - y1

        plate_roi = frame[int(y1): int(y1 + h), int(x1): int(x1 + w)]
        cropped_plate_roi = plate_roi[5:80, 10:300]  # Example cropping dimensions
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


        cv2.rectangle(frame, (int(x1), int(y1)), (int(x2), int(y2)), (0, 255, 0), 2)

    cv2.imshow("Detected Plates", frame)


    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

cap.release()
cv2.destroyAllWindows()

