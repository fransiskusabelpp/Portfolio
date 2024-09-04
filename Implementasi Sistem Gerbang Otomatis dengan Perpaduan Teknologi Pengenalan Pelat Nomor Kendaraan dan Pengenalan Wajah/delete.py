import face_recognition
import cv2
import os
import glob
import numpy as np
import pickle
import sqlite3

class FaceEncoder:
    # ... (other methods and __init__ remain unchanged)
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


    def delete_face(self, name_to_delete, encoding_file):
        # Find the index of the name to delete
        index_to_delete = None
        for i, name in enumerate(self.known_face_names):
            if name == name_to_delete:
                index_to_delete = i
                break

        if index_to_delete is not None:
            # Remove the face encoding and name
            del self.known_face_encodings[index_to_delete]
            del self.known_face_names[index_to_delete]

            # Save updated encodings to file
            with open(encoding_file, 'wb') as f:
                data = {'encodings': self.known_face_encodings, 'names': self.known_face_names}
                pickle.dump(data, f)
                print(f"Face data for '{name_to_delete}' has been deleted.")
            return True
        else:
            print(f"Face data for '{name_to_delete}' not found.")
            return False
        

def delete_data_by_name(name_to_delete):
    try:
        # Connect to the SQLite database (replace 'recognition.db' with your actual database file)
        conn = sqlite3.connect("database/recognition.db")
        cursor = conn.cursor()

        # Check if the name exists in the database
        cursor.execute("SELECT * FROM data_pengguna WHERE name = ?", (name_to_delete,))
        row = cursor.fetchone()

        if row:
            # Delete the row if the name exists
            cursor.execute("DELETE FROM data_pengguna WHERE name = ?", (name_to_delete,))
            conn.commit()
            print(f"Record for '{name_to_delete}' deleted successfully!")
        else:
            print(f"Record for '{name_to_delete}' not found.")

    except sqlite3.Error as e:
        print(f"Error deleting record: {e}")

    finally:
        conn.close()

# Example usage:

if __name__ == "__main__":
    face_encoder = FaceEncoder()
    face_encoder.load_encodings("known_faces_encoding.pkl")

    # Delete a face by name (replace "John Doe" with the actual name you want to delete)
    deleted = face_encoder.delete_face("Ihsan", "known_faces_encoding.pkl")
    if deleted:
        print("Face deleted successfully.")
    else:
        print("Face not found.")
    delete_data_by_name("Ihsan")
