function getObjKeys(obj, value) {
    return Object.keys(obj).filter(key => obj[key] === value);
  }

  /*const firebaseConfig = {
    apiKey: "AIzaSyAqbi6Cud5CDw7bXJE6nCPNCvMECS08oOE",
    authDomain: "counter-77c7e.firebaseapp.com",
    databaseURL: "https://counter-77c7e-default-rtdb.firebaseio.com",
    projectId: "counter-77c7e",
    storageBucket: "counter-77c7e.appspot.com",
    messagingSenderId: "192592357928",
    appId: "1:192592357928:web:1abe3dbf9425a3f649e31d"
  };*/

const firebaseConfig = {
  apiKey: "AIzaSyDvITAGYqUgOpvFZwJ04RiBCHil1l4p8pc",
  authDomain: "counter-a2b53.firebaseapp.com",
  databaseURL: "https://counter-a2b53-default-rtdb.firebaseio.com",
  projectId: "counter-a2b53",
  storageBucket: "counter-a2b53.appspot.com",
  messagingSenderId: "801224434498",
  appId: "1:801224434498:web:0ccdde4e58d2da0a329edb",
  measurementId: "G-ERK6431DZT"
};
firebase.initializeApp(firebaseConfig);

var countRef = firebase.database().ref("Alert");
count = 4;
countRef.on('child_added', function(snapshot) {
    value = snapshot.val()
    label = value["Label"];
    timestamp = value["Val"]
    count += 1;
    const x = `<div class="item" style = "grid-column: 1; grid-row:${count};">
                <label>${timestamp}</label>
              </div>
              <div class="ritem" style = "grid-column: 4; grid-row: ${count};">
                <h4>Incorrect Handling Alert</h4>
                <p>${label} Detected</p>
              </div>`
    document.getElementById('reception').innerHTML = document.getElementById('reception').innerHTML + x;
});