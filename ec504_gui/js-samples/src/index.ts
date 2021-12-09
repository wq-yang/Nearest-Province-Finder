/*
 * Copyright 2019 Google LLC. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/* eslint-disable no-undef, @typescript-eslint/no-unused-vars, no-unused-vars */
import axios from "axios";
import "./style.css";

const labels = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
let labelIndex = 0;
let markers: google.maps.Marker[] = [];

function initMap() {
  const myLatlng = { lat: 42.363, lng: -100.044 };
  const map = new google.maps.Map(document.getElementById("map")!, {
    zoom: 4,
    center: myLatlng,
  });

  // Create the initial InfoWindow.
  let infoWindow = new google.maps.InfoWindow({
    content: "Click the map to get Lat/Lng!",
    position: myLatlng,
  });
  

  infoWindow.open(map);


  // Configure the click listener.
  map.addListener("click", async (mapsMouseEvent) => {
    // Close the current InfoWindow.
    infoWindow.close();

    markers.forEach(marker => marker.setMap(null));
    markers = [];
    // Create a new InfoWindow.
    infoWindow = new google.maps.InfoWindow({
      position: mapsMouseEvent.latLng,
    });

    infoWindow.setContent(
      JSON.stringify(mapsMouseEvent.latLng.toJSON(), null, 2)
      //JSON.stringify(formStr)
    );
    try {
      const str: any = infoWindow.getContent()
      const {lat, lng}: any = JSON.parse(str)
      const res:any = await axios.get(`http://localhost:3000/api/pos?lat=${lat}&lng=${lng}`);
      console.log(res.data.pos)
      res.data.pos.forEach(({rank, lat, lng }:any) => {
        addPosMarker(map, String(rank), Number(lat), Number(lng));
      });
      
    } catch (e) {console.error(e)}

    console.log(infoWindow.getContent())

    infoWindow.open(map);
  });

  // const k1 = document.createElement("button");
  // k1.textContent = "K=1";

  // const k2 = document.createElement("button");
  // k2.textContent = "K=2";

  // const k3 = document.createElement("button");
  // k3.textContent = "K=3";

  // const k4 = document.createElement("button");
  // k4.textContent = "K=4";

  // const k5 = document.createElement("button");
  // k5.textContent = "K=5";

  // const k6 = document.createElement("button");
  // k6.textContent = "K=6";

  // const k7 = document.createElement("button");
  // k7.textContent = "K=7";

  // const k8 = document.createElement("button");
  // k8.textContent = "K=8";

  // const k9 = document.createElement("button");
  // k9.textContent = "K=9";

  // const k10 = document.createElement("button");
  // k10.textContent = "K=10";

  // k1.addEventListener("click", function(){
  //   addMarker(map,'1');
  // });

  // k2.addEventListener("click", function(){
  //   addMarker(map,'2');
  // });

  // k3.addEventListener("click", function(){
  //   addMarker(map,'3');
  // });

  // k4.addEventListener("click", function(){
  //   addMarker(map,'4');
  // });

  // k5.addEventListener("click", function(){
  //   addMarker(map,'5');
  // });

  // k6.addEventListener("click", function(){
  //   addMarker(map,'6');
  // });

  // k7.addEventListener("click", function(){
  //   addMarker(map,'7');
  // });

  // k8.addEventListener("click", function(){
  //   addMarker(map,'8');
  // });

  // k9.addEventListener("click", function(){
  //   addMarker(map,'9');
  // });

  // k10.addEventListener("click", function(){
  //   addMarker(map,'10');
  // });

// function addMarker(map: google.maps.Map, label) {
//   // Add the marker at the clicked location, and add the next-available label
//   // from the array of alphabetical characters.
//   markers.push(new google.maps.Marker({
//     position: {
//       lat: 47.353734888615854,
//       lng: -135.4177527132034
//     },
//     label: label,
//     map: map,
//   }));

//   console.log(`k = ${label}`)
// }

function addPosMarker(map: google.maps.Map, rank, lat, lng) {
  // Add the marker at the clicked location, and add the next-available label
  // from the array of alphabetical characters.
  markers.push(new google.maps.Marker({
    position: {
      lat: lat,
      lng: lng
    },
    label: rank,
    map: map,
  }));
}


  // map.controls[google.maps.ControlPosition.LEFT_BOTTOM].push(k1);
  // map.controls[google.maps.ControlPosition.LEFT_BOTTOM].push(k2);
  // map.controls[google.maps.ControlPosition.LEFT_BOTTOM].push(k3);
  // map.controls[google.maps.ControlPosition.LEFT_BOTTOM].push(k4);
  // map.controls[google.maps.ControlPosition.LEFT_BOTTOM].push(k5);
  // map.controls[google.maps.ControlPosition.LEFT_BOTTOM].push(k6);
  // map.controls[google.maps.ControlPosition.LEFT_BOTTOM].push(k7);
  // map.controls[google.maps.ControlPosition.LEFT_BOTTOM].push(k8);
  // map.controls[google.maps.ControlPosition.LEFT_BOTTOM].push(k9);
  // map.controls[google.maps.ControlPosition.LEFT_BOTTOM].push(k10);
}
export { initMap };
