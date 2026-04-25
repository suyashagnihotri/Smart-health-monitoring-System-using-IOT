
import streamlit as st
import numpy as np
import pickle

st.title("Disease predictor (temperature, heart rate, SpO2)")
st.write("Enter vitals to predict disease.")

model = pickle.load(open("output\disease_model.pkl", "rb"))
le = pickle.load(open("output\label_encoder.pkl", "rb"))

# inputs (use medians from training as defaults if you like)
temp = st.number_input("Body temperature", value=37.6, format="%.2f")
hr = st.number_input("Heart rate (pulse)", value=80, format="%d")
o2 = st.number_input("SpO2 (oxygen saturation)", value=95.0, format="%.1f")

if st.button("Predict"):
    cols = ['pulse', 'SpO2', 'body temperature']
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
    st.success(f"Predicted disease: {label}")
    st.write("Probabilities:")
    for i, cls in enumerate(le.classes_):
        st.write(f"{cls}: {proba[i]:.3f}")
