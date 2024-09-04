import sqlite3

def delete_data(name_to_delete, plate_number_to_delete):
    try:
        # Connect to the SQLite database (replace 'recognition.db' with your actual database file)
        conn = sqlite3.connect("database/recognition.db")
        cursor = conn.cursor()

        # Delete the record from the 'data_pengguna' table
        cursor.execute("DELETE FROM data_pengguna WHERE name = ? AND plate_number = ?", (name_to_delete, plate_number_to_delete))
        conn.commit()
        print(f"Record for '{name_to_delete}' with plate number '{plate_number_to_delete}' deleted successfully!")

    except sqlite3.Error as e:
        print(f"Error deleting record: {e}")

    finally:
        conn.close()

# Example usage:
if __name__ == "__main__":
    user_name_to_delete = "Messi"
    user_plate_number_to_delete = "B1945RI"
    delete_data(user_name_to_delete, user_plate_number_to_delete)
