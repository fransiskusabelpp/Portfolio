import face_recognition
import cv2
import os
import glob
import numpy as np
import pickle
import PySimpleGUI as sg

class FaceEncoder:
    def __init__(self):
        self.known_face_encodings = []
        self.known_face_names = []

    def load_encodings(self, encoding_file):
        if os.path.exists(encoding_file):
            # Load existing encodings from file
            with open(encoding_file, 'rb') as f:
                data = pickle.load(f)
                self.known_face_encodings = data['encodings']
                self.known_face_names = data['names']
        else:
            print("No existing encoding file found. Encoding all faces from images folder.")

    def add_new_face(self, images_folder, encoding_file):
        # List all image files in the specified folder
        image_files = glob.glob(os.path.join(images_folder, "*.*"))

        # Flag to check if any new face is added
        new_face_added = False
        new_face_list = []

        # Iterate through each image file
        for image_path in image_files:
            # Load the image
            img = cv2.imread(image_path)
            rgb_img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)

            # Get encoding for the new face
            new_face_encoding = face_recognition.face_encodings(rgb_img)[0]

            # Extract name from the image file name
            filename = os.path.splitext(os.path.basename(image_path))[0]

            # Check if the face already exists in known encodings
            if not any(np.array_equal(new_face_encoding, known_encoding) for known_encoding in self.known_face_encodings):
                # Add the new face encoding and name to the existing data
                self.known_face_encodings.append(new_face_encoding)
                self.known_face_names.append(filename)
                new_face_added = True
                new_face_list.append(filename)
                print(f"New face data for '{filename}' has been added.")

        # Save updated encodings to file if any new face added
        if new_face_added:
            with open(encoding_file, 'wb') as f:
                data = {'encodings': self.known_face_encodings, 'names': self.known_face_names}
                pickle.dump(data, f)
                print("Encoding file updated.")
            return new_face_list
        else:
            return []

    def list_known_faces(self, encoding_file):
        if os.path.exists(encoding_file):
            with open(encoding_file, 'rb') as f:
                data = pickle.load(f)
                known_names = data.get('names', [])
                return known_names
        else:
            return known_names



import sqlite3

def insert_data(name, plate_number):
    try:
        # Connect to the SQLite database (replace 'recognition.db' with your actual database file)
        conn = sqlite3.connect("database/recognition.db")
        cursor = conn.cursor()

        # Insert data into the 'data_pengguna' table
        cursor.execute("INSERT INTO data_pengguna (name, plate_number) VALUES (?, ?)", (name, plate_number))
        conn.commit()
        print("Data inserted successfully!")
        sg.popup("Data inserted successfully!")

    except sqlite3.Error as e:
        print(f"Error inserting data: {e}")
        sg.popup("Error inserting data")

    finally:
        conn.close()

# Example usage:
# Create an instance of FaceEncoder
face_encoder = FaceEncoder()

layout_main = [
    [sg.Text("Face Encoder", font=("Helvetica", 16))],
    [sg.Text("Encoding File:"), sg.InputText(key="encoding_file"), sg.FileBrowse("Select File")],
    [sg.Button("Load Encodings"), sg.Button("Add New Faces"), sg.Button("List Known Faces"), sg.Button("Exit")]
]

# Create the main window
window_main = sg.Window("Face Encoder", layout_main)

window_known_faces_activate = False

window_add_faces_activate = False

# Event loop for the main window
while True:
    event_main, values_main = window_main.read()

    if event_main == sg.WINDOW_CLOSED or event_main == "Exit":
        break
    elif event_main == "Load Encodings":
        face_encoder.load_encodings(values_main["encoding_file"])
        sg.popup("Encodings loaded successfully!", title="Success")
    elif event_main == "Add New Faces":
        layout_add_faces = [
            [sg.Text("Add New Faces", font=("Helvetica", 16))],
            [sg.Text("Images Folder:"), sg.InputText(key="images_folder"), sg.FolderBrowse("Select Folder")],
            [sg.Table(values=[], headings=["New Added Faces"], auto_size_columns=True, justification="left", num_rows=10, key="table_add")],
            [sg.Button("Check"), sg.Button("Add to Database", disabled=True), sg.Button("Cancel")]
        ]

        window_add_faces = sg.Window("Add New Faces", layout_add_faces, finalize=True)
        
        while True:
            event_add_faces, values_add_faces = window_add_faces.read()

            if event_add_faces == sg.WINDOW_CLOSED or event_add_faces == "Cancel":
                window_add_faces.close()
                window_add_faces_activate = False
                break
            elif event_add_faces == "Check":
                added_faces = face_encoder.add_new_face(values_add_faces["images_folder"], values_main["encoding_file"])
                window_add_faces["table_add"].update(values=added_faces)
                if added_faces is not [0]:
                    window_add_faces["Add to Database"].update(disabled=False)
            elif event_add_faces == "Add to Database":
                name = sg.popup_get_text("Name")
                sg.popup(f'You entered {name}')
                plate_number = sg.popup_get_text("Plate Number")
                sg.popup(f'You entered {plate_number}')
                insert_data(name, plate_number)
                
            
                
                
    elif event_main == "List Known Faces" and not window_known_faces_activate:
        known_faces = face_encoder.list_known_faces(values_main["encoding_file"])
        
        # Define the GUI layout for the known faces window
        layout_known_faces = [
            [sg.Text("Known Faces", font=("Helvetica", 16))],
            [sg.InputText(key="search_input", size=(20, 1), enable_events=True), sg.Button("Search")],
            [sg.Table(values=[], headings=["Known Faces"], auto_size_columns=False, justification="left", num_rows=10, key="table")]
        ]
        # Create the known faces window (initially hidden)
        window_known_faces = sg.Window("Known Faces", layout_known_faces, finalize=True)
        window_known_faces["table"].update(values=known_faces)
        window_known_faces_activate = True
        while True:
            event_known_faces, values_known_faces = window_known_faces.read()

            if event_known_faces == sg.WINDOW_CLOSED:
                window_known_faces.close()
                window_known_faces_activate = False
                break
            elif event_known_faces == "Search":
                search_term = values_known_faces["search_input"].strip().lower()
                filtered_faces = [name for name in known_faces if search_term in name.lower()]
                window_known_faces["table"].update(values=filtered_faces)
                

# Close both windows
window_main.close()
window_add_faces.close()
window_known_faces.close()