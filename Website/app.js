import { initializeApp } from "https://www.gstatic.com/firebasejs/10.8.0/firebase-app.js";
import { getDatabase, ref, set, onValue } from "https://www.gstatic.com/firebasejs/10.8.0/firebase-database.js";

const firebaseConfig = {
  apiKey: "AIzaSyDhnKkd0S5LAQzYiko_DBZheOc-tqLiELc",
  authDomain: "meter-air-portable.firebaseapp.com",
  databaseURL: "https://meter-air-portable-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "meter-air-portable",
  storageBucket: "meter-air-portable.appspot.com",
  messagingSenderId: "339728610802",
  appId: "1:339728610802:web:60f6d7957e757797aee76b"
};

const app = initializeApp(firebaseConfig);
const db = getDatabase(app);

// Firebase
const refTarget = ref(db, 'sistem_air/kontrol/target_liter');
const refPerintah = ref(db, 'sistem_air/kontrol/perintah_buka');
const refVolume = ref(db, 'sistem_air/sensor/volume_sekarang');
const refValve = ref(db, 'sistem_air/sensor/status_valve');
const refDebit = ref(db, 'sistem_air/sensor/debit_air');
const refHeartbeat = ref(db, 'sistem_air/status/heartbeat'); 

// HTML
const inputTarget = document.getElementById('inputTarget');
const btnKirim = document.getElementById('btnKirim');
const btnManual = document.getElementById('btnManual');
const valVolume = document.getElementById('valVolume');
const valValve = document.getElementById('valValve');
const valDebit = document.getElementById('valDebit');
const statusKoneksi = document.getElementById('statusKoneksi');

let isValveOpenJS = false;
let timeoutKoneksi;

// Cek Status Off/On
onValue(refHeartbeat, (snapshot) => {
    statusKoneksi.innerText = "ONLINE";
    statusKoneksi.classList.add("online");
  
    clearTimeout(timeoutKoneksi);

    timeoutKoneksi = setTimeout(() => {
        statusKoneksi.innerText = "OFFLINE";
        statusKoneksi.classList.remove("online");
    }, 5000); 
});

// Tombol Alirkan Air 
btnKirim.addEventListener('click', () => {
    const target = parseFloat(inputTarget.value);
    if(target > 0) {
        set(refTarget, target);   
        set(refPerintah, true);   
    } else {
        alert("Masukkan angka lebih dari 0 untuk mode otomatis!");
    }
});

// Tombol Buka Manual
btnManual.addEventListener('click', () => {
    if (!isValveOpenJS) {
        set(refTarget, 0); 
        set(refPerintah, true);
    } else {
        set(refPerintah, false);
    }
});

onValue(refVolume, (snapshot) => {
    const data = snapshot.val();
    if(data !== null) valVolume.innerText = data.toFixed(1) + " L"; 
});

onValue(refDebit, (snapshot) => {
    const data = snapshot.val();
    if(data !== null) valDebit.innerText = data.toFixed(2) + " L/min"; 
});

onValue(refValve, (snapshot) => {
    const data = snapshot.val();
    if(data !== null) {
        valValve.innerText = data;
        
        if(data === "TERBUKA") {
            isValveOpenJS = true;
            valValve.style.color = "red";
            
            btnKirim.disabled = true;
            btnKirim.innerText = "SEDANG MENGALIR...";
            btnManual.innerText = "TUTUP KRAN (STOP)";
            btnManual.className = "btn-stop"; 
        } else {
            isValveOpenJS = false;
            valValve.style.color = "black";
            
            btnKirim.disabled = false;
            btnKirim.innerText = "ALIRKAN AIR";
            btnManual.innerText = "BUKA MANUAL";
            btnManual.className = "btn-manual"; 
        }
    }
});

