const char index_html[] PROGMEM = R"rawliteral(<html>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<head>
 <style>
  big{
    font-size:68px;
  }
  ul{
  }
  li{
    text-align:center;
    line-height:50px;
    font-size:20px;
    margin:2%;
    float:left;
    width:15%;
    height:50px;
    color:white;
    padding:8px 20px 8px 20px;
    background-color:#666666;
    border-radius:8px;
    }
  table{
    table-layout:fixed;
    
  }
  </style>
  <script>
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
    var avaliableMedia = JSON.parse(this.responseText);
    
    for (const [key, value] of Object.entries(avaliableMedia)) {
      console.log(`${key}: ${value}`);
      var newitem=document.createElement("li");
      newitem.setAttribute("id", "media"+`${key}`);
      newitem.appendChild(document.createTextNode(value));
      newitem.onclick = function() {changeMedia(key)};
      document.getElementById("avaliableMedia").appendChild(newitem);
    }
      
    }
  };
    xhttp.open("GET", "/avaliableMedia", true);
    xhttp.send();
  
  function changeMedia(id){
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
    var allButton = document.getElementsByTagName("li");
    for (var i = 0; i<allButton.length; i++) {
      allButton[i].style.backgroundColor = "#666666";
    }
    document.getElementById('media'+id).style.backgroundColor = "#68e884";
    }
  };
    xhttp.open("GET", "/changeMedia?id="+id, true);
    xhttp.send();   
  }
  function changeAutoNext(){
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
    if(this.response == "True") document.getElementById("autoNextStatus").innerHTML = "⏸️"
    else if(this.response == "False") document.getElementById("autoNextStatus").innerHTML = "▶️"
    }
  };
    xhttp.open("GET", "/changeAutoNext", true);
    xhttp.send();
  }
</script>
</head>
<body>
  <div style="width:80%;margin:0 auto;text-align:center">
  <big>bbPOV-P</big>
  <div style="font-size:40px" id="autoNextStatus" onclick="changeAutoNext()">⏸️</div>
  <p>可用图像：</p>
      <ul id="avaliableMedia">
      </ul>
  </div>
</body>
</html>)rawliteral";
