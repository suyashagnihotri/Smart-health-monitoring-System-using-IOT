# train_and_deploy_predictor.py
"""
Train a disease predictor from a CSV that contains:
 - pulse (heart rate)
 - body temperature (temperature)
 - SpO2 (oxygen saturation)
 - Status (disease label)

Outputs:
 - /mnt/data/disease_model.pkl  (sklearn Pipeline: scaler + RandomForest)
 - /mnt/data/label_encoder.pkl
 - /mnt/data/predict_app.py     (Streamlit app)
 - /mnt/data/README_predictor.txt
"""

import streamlit as st
import pickle
import numpy as np

st.title("Disease Prediction App")

model = pickle.load(open("output/disease_model.pkl", "rb"))
le = pickle.load(open("output/label_encoder.pkl", "rb"))

temp = st.number_input("Body Temperature", value=98.6)
hr = st.number_input("Heart Rate (Pulse)", value=72)
o2 = st.number_input("SpO2 (Oxygen Level)", value=98)

if st.button("Predict"):
    x = np.array([[hr, o2, temp]])  # must match training feature order
    pred = model.predict(x)[0]
    label = le.inverse_transform([pred])[0]
    st.success(f"Predicted Disease: {label}")
import pandas as pd
import numpy as np
from pathlib import Path
import pickle
from sklearn.model_selection import train_test_split
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler, LabelEncoder
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import classification_report, accuracy_score, confusion_matrix

DATA_PATH = Path("Health data.csv")   # change path if needed
OUT_DIR = Path("./output")
OUT_DIR.mkdir(exist_ok=True)

# 1) Load
df = pd.read_csv(DATA_PATH)
print("Dataset shape:", df.shape)
print(df.head())

# 2) Columns expected (adjust names here if your CSV uses different column names)
# We'll try to be robust by matching common names:
def find_col(possible_names):
    for n in df.columns:
        if n.lower() in [p.lower() for p in possible_names]:
            return n
    return None

temp_col = find_col(["temp", "temperature", "body temperature", "body_temperature"])
hr_col = find_col(["pulse", "hr", "heart_rate", "heart rate"])
o2_col = find_col(["spo2", "spO2", "o2", "o2_sat"])
target_col = find_col(["disease", "status", "diagnosis", "label", "target", "condition"])

# fallback: use best guesses
if not temp_col:
    # pick numeric column that looks like temperature (contains 'body' or 'temp')
    for c in df.select_dtypes(include=[np.number]).columns:
        if 'temp' in c.lower() or 'body' in c.lower():
            temp_col = c
            break
if not hr_col:
    for c in df.select_dtypes(include=[np.number]).columns:
        if 'pulse' in c.lower() or 'hr' in c.lower() or 'heart' in c.lower():
            hr_col = c
            break
if not o2_col:
    for c in df.select_dtypes(include=[np.number]).columns:
        if 'sp' in c.lower() or 'o2' in c.lower():
            o2_col = c
            break
if not target_col:
    target_col = df.columns[-1]  # last column as fallback

print("Using columns -> temp:", temp_col, "hr:", hr_col, "o2:", o2_col, "target:", target_col)

# 3) Simple cleaning & select features
features = [c for c in (hr_col, o2_col, temp_col) if c is not None]
if len(features) < 3:
    # fill with first numeric columns until we have three
    numerics = [c for c in df.select_dtypes(include=[np.number]).columns if c not in features and c.lower() not in ("id","index")]
    for c in numerics:
        if len(features) < 3:
            features.append(c)

df_clean = df.dropna(subset=features + [target_col]).copy()
X = df_clean[features].astype(float)
y_raw = df_clean[target_col].astype(str)

# 4) Encode target, split
le = LabelEncoder()
y = le.fit_transform(y_raw)
print("Classes:", list(le.classes_))
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42, stratify=y)

# 5) Pipeline & quick training (fast; change hyperparams if you'd like)
pipe = Pipeline([
    ("scaler", StandardScaler()),
    ("rf", RandomForestClassifier(n_estimators=150, max_depth=8, random_state=42))
])
pipe.fit(X_train, y_train)

# 6) Evaluate
y_pred = pipe.predict(X_test)
acc = accuracy_score(y_test, y_pred)
print(f"Test accuracy: {acc:.4f}")
print(classification_report(y_test, y_pred, target_names=le.classes_))

# 7) Save artifacts
model_path = OUT_DIR / "disease_model.pkl"
le_path = OUT_DIR / "label_encoder.pkl"
with open(model_path, "wb") as f:
    pickle.dump(pipe, f)
with open(le_path, "wb") as f:
    pickle.dump(le, f)

print("Saved model to", model_path)
print("Saved encoder to", le_path)

# 8) Create Streamlit app
streamlit_code = f'''
import streamlit as st
import numpy as np
import pickle

st.title("Disease predictor (temperature, heart rate, SpO2)")
st.write("Enter vitals to predict disease.")

model = pickle.load(open("{model_path}", "rb"))
le = pickle.load(open("{le_path}", "rb"))

# inputs (use medians from training as defaults if you like)
temp = st.number_input("Body temperature", value={float(X[features[2]].median()) if len(features)>=3 else 36.6}, format="%.2f")
hr = st.number_input("Heart rate (pulse)", value={int(X[features[0]].median()) if len(features)>=1 else 70}, format="%d")
o2 = st.number_input("SpO2 (oxygen saturation)", value={float(X[features[1]].median()) if len(features)>=2 else 97.0}, format="%.1f")

if st.button("Predict"):
    cols = {features}
    vals = []
    for c in cols:
        lc = c.lower()
        if "temp" in lc or "body" in lc:
            vals.append(float(temp))
        elif "pulse" in lc or "hr" in lc or "heart" in lc:
            vals.append(float(hr))
        elif "sp" in lc or "o2" in lc:
            vals.append(float(o2))
        else:
            vals.append(0.0)
    x = np.array([vals])
    pred = model.predict(x)[0]
    proba = model.predict_proba(x)[0]
    label = le.inverse_transform([pred])[0]
    st.success(f"Predicted disease: {{label}}")
    st.write("Probabilities:")
    for i, cls in enumerate(le.classes_):
        st.write(f"{{cls}}: {{proba[i]:.3f}}")
'''

(app_path := OUT_DIR / "predict_app.py").write_text(streamlit_code)
(OUT_DIR / "README_predictor.txt").write_text(
    "Run:\n"
    "1) pip install -r requirements.txt  (or pip install streamlit scikit-learn pandas)\n"
    "2) python train_and_deploy_predictor.py  # trains and writes model to ./output\n"
    "3) streamlit run ./output/predict_app.py\n"
)
print("Wrote Streamlit app to", app_path)