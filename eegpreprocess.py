import pandas as pd
import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.metrics import classification_report, confusion_matrix
import tensorflow as tf
from tensorflow import keras
import serial
import serial.tools.list_ports
import time

#Load Dataset 
file_path = "museMonitor_with_directions.csv"  # <-- already has Direction column
df = pd.read_csv(file_path)

# Drop unnecessary columns
cols_to_drop = ["TimeStamp", "Elements", "Battery"]
df = df.drop(columns=cols_to_drop, errors="ignore")

# Clean dataset
df = df.drop_duplicates()
df = df.dropna(thresh=len(df.columns) - 5)
df = df.interpolate()

if "Direction" not in df.columns:
    raise ValueError("Dataset must contain a 'Direction' column")

print("Class distribution (before balancing):")
print(df["Direction"].value_counts())

# Balance dataset so all classes have equal rows
min_count = df["Direction"].value_counts().min()
df_balanced = df.groupby("Direction").apply(
    lambda x: x.sample(min_count, random_state=42)
).reset_index(drop=True)

print("\nClass distribution (after balancing):")
print(df_balanced["Direction"].value_counts())

# Features & Labels 
X = df_balanced.drop(columns=["Direction"], errors="ignore")
y = df_balanced["Direction"]

feature_columns = X.columns
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

# Encode labels
le = LabelEncoder()
y_encoded = le.fit_transform(y)
class_names = le.classes_   # ['back','front','left','right','stop']

# Stratified train-test split
X_train, X_test, y_train, y_test = train_test_split(
    X_scaled, y_encoded, test_size=0.2, random_state=42, stratify=y_encoded
)

# Build Model
model = keras.Sequential([
    keras.layers.Input(shape=(X_train.shape[1],)),
    keras.layers.Dense(128, activation="relu"),
    keras.layers.Dense(64, activation="relu"),
    keras.layers.Dense(len(class_names), activation="softmax")
])

model.compile(optimizer="adam", loss="sparse_categorical_crossentropy", metrics=["accuracy"])
history = model.fit(X_train, y_train, validation_data=(X_test, y_test), epochs=25, batch_size=32)

# Evaluate
y_pred = np.argmax(model.predict(X_test), axis=1)
print("\nClassification Report:")
print(classification_report(y_test, y_pred, target_names=class_names))

print("Confusion Matrix:")
print(confusion_matrix(y_test, y_pred))

#  Arduino Connection 
def find_arduino_port():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        desc = port.description.lower()
        if "arduino" in desc or "ch340" in desc:
            return port.device
    return None

arduino_port = find_arduino_port()
if arduino_port is None:
    print("⚠ Arduino not auto-detected. Please select manually.")
    ports = serial.tools.list_ports.comports()
    for i, port in enumerate(ports):
        print(f"{i+1}: {port.device} - {port.description}")
    if len(ports) == 0:
        raise Exception(" No serial ports found.")
    choice = input("Enter the number of the correct COM port: ")
    arduino_port = ports[int(choice) - 1].device

print(f"Arduino will use port: {arduino_port}")
arduino = serial.Serial(arduino_port, 9600)
time.sleep(2)

# Command Map 
command_map = {
    "front": "F",
    "back": "B",
    "left": "L",
    "right": "R",
    "stop": "S"
}

# Streaming
print("Streaming predictions to Arduino...")

try:
    for i in range(len(df_balanced)):
        if i >= 20:  # test limit
            break

        row_features = df_balanced.iloc[i][feature_columns].values.reshape(1, -1)
        sample = scaler.transform(row_features)

        pred = model.predict(sample, verbose=0)
        direction = class_names[np.argmax(pred)]
        command = command_map[direction]

        try:
            arduino.write(command.encode())
            print(f"Row {i}: Predicted {direction} -> Sent {command}")
        except serial.SerialException:
            print("Arduino disconnected.")
            break

        time.sleep(1)

except KeyboardInterrupt:
    print("\nStopped manually.")
finally:
    if arduino.is_open:
        arduino.close()
    print("Serial connection closed.")

