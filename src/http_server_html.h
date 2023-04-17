const char CLIMATE_CONFIG_FORM[] =
    "<!DOCTYPE HTML>"
    "<html>"
    "<head>"
    "<meta content=\"text/html; charset=ISO-8859-1\""
    " http-equiv=\"content-type\">"
    "<meta name = \"viewport\" content = \"width = device-width, initial-scale = 1.0, maximum-scale = 1.0, user-scalable=0\">"
    "<title>Terrarium controller - Config</title>"
    "<style>"
    "\"body { background-color: #808080; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; text-align:center;}\""
    "</style>"
    "</head>"
    "<body>"
    "<h3>Climate configuration</h3>"
    "<form action=\"/api/climate\" method=\"post\">"
    "<p>"
    "<label>Config: JSON</label>"
    "<textarea name=\"json_config\"></textarea><br>"
    "<input type=\"submit\" value=\"Save\">"
    "</p>"
    "</form>"
    "</body>"
    "</html>";

const char SETTINGS_FORM[] =
   "<!DOCTYPE HTML><html><head> <meta content=\"text/html; charset=ISO-8859-1\" http-equiv=\"content-type\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0\"> <title>Terrarium controller - Config</title> <style>body{background-color: #ffffff; font-family: Arial, Helvetica, Sans-Serif; Color: #000000;}</style></head><body> <svg version=\"1.0\" xmlns=\"http://www.w3.org/2000/svg\" width=\"1405.000000pt\" height=\"582.000000pt\" viewBox=\"0 0 1405.000000 582.000000\" preserveAspectRatio=\"xMidYMid meet\"> <metadata> Created by potrace 1.16, written by Peter Selinger 2001-2019 </metadata> <g transform=\"translate(0.000000,582.000000) scale(0.100000,-0.100000)\" fill=\"#455bff\" stroke=\"none\"> <path d=\"M4196 5450 c-52 -26 -74 -47 -100 -95 -20 -37 -21 -58 -26 -307 l-5 -267 -60 45 c-144 109 -371 232 -549 298 -128 48 -331 100 -476 121 -155 23 -486 24 -625 1 -274 -45 -520 -132 -745 -264 -238 -140 -491 -358 -635 -548 -16 -22 -35 -45 -42 -53 -18 -18 -154 -224 -188 -286 -120 -214 -223 -487 -270 -720 -65 -321 -73 -695 -19 -1002 35 -203 113 -466 186 -623 53 -113 73 -155 79 -160 3 -3 16 -25 30 -50 26 -48 149 -238 173 -267 8 -10 40 -49 72 -88 315 -391 769 -667 1269 -771 120 -25 148 -27 380 -28 270 -1 337 8 550 69 490 142 957 516 1262 1011 103 166 234 452 241 525 12 122 -86 228 -210 229 l-48 0 0 299 0 300 199 3 c184 3 202 5 228 24 32 24 49 54 58 107 l7 37 -546 0 -546 0 -36 -33 c-31 -28 -37 -41 -42 -85 l-5 -52 242 0 241 0 -2 -357 -3 -357 -263 -133 c-367 -184 -615 -279 -868 -329 -67 -13 -135 -18 -274 -17 -206 0 -296 16 -418 73 -365 171 -617 620 -537 959 30 124 105 216 229 276 130 63 321 94 464 74 242 -34 492 -209 737 -517 45 -57 99 -82 174 -82 92 1 183 70 196 152 4 21 7 178 7 348 l1 310 841 0 c791 0 847 1 934 19 191 40 369 153 492 311 19 25 39 48 45 52 5 4 6 8 2 8 -5 0 0 12 11 28 65 94 102 225 109 392 4 106 2 139 -16 214 -51 213 -169 387 -342 506 -31 22 -363 192 -738 379 -638 319 -685 341 -734 341 -31 -1 -68 -9 -91 -20z m786 -1036 c41 -21 67 -49 89 -97 24 -54 24 -97 0 -150 -71 -154 -281 -155 -352 -2 -25 54 -23 100 5 162 46 98 161 137 258 87z\"/> <path d=\"M9253 2991 c-42 -26 -53 -49 -53 -110 0 -38 6 -56 25 -80 97 -114 290 -6 232 130 -23 54 -50 72 -115 76 -44 3 -64 0 -89 -16z\"/> <path d=\"M5119 2596 c-120 -33 -230 -129 -269 -233 -20 -53 -21 -72 -18 -298 l3 -240 29 -53 c54 -96 151 -166 268 -192 80 -18 238 -8 308 19 121 46 194 114 207 194 11 71 -37 127 -110 127 -33 0 -35 -3 -46 -44 -28 -113 -155 -174 -301 -146 -119 22 -180 100 -180 227 l0 73 305 0 306 0 25 26 c26 25 26 27 22 152 -4 147 -19 191 -90 268 -86 94 -186 135 -325 133 -48 0 -109 -7 -134 -13z m222 -152 c98 -29 159 -116 159 -228 l0 -46 -245 0 -245 0 0 62 c0 169 150 265 331 212z\"/> <path d=\"M5964 2591 c-67 -41 -64 -16 -64 -544 l0 -480 51 5 c54 6 97 30 116 69 9 17 13 106 13 324 0 340 2 349 78 417 71 65 209 86 296 47 36 -17 48 -3 48 55 0 93 -52 130 -171 124 -88 -4 -164 -35 -223 -91 l-38 -35 0 64 0 64 -37 0 c-21 0 -52 -9 -69 -19z\"/> <path d=\"M6724 2591 c-67 -41 -64 -16 -64 -544 l0 -480 51 5 c54 6 97 30 116 69 9 17 13 106 13 324 0 340 2 349 78 417 71 65 209 86 296 47 36 -17 48 -3 48 55 0 93 -52 130 -171 124 -88 -4 -164 -35 -223 -91 l-38 -35 0 64 0 64 -37 0 c-21 0 -52 -9 -69 -19z\"/> <path d=\"M7605 2591 c-144 -42 -225 -112 -225 -196 0 -29 8 -45 36 -73 l36 -36 20 24 c46 60 87 95 138 119 75 35 213 37 276 4 81 -43 114 -106 114 -216 l0 -58 -26 20 c-71 56 -219 83 -340 62 -138 -24 -224 -82 -275 -186 -30 -61 -31 -69 -27 -162 3 -87 7 -105 34 -153 40 -71 110 -125 197 -151 55 -17 87 -21 170 -17 90 3 110 7 167 35 36 18 75 40 88 49 l22 16 0 -51 0 -51 35 0 c53 0 101 24 120 60 14 27 16 74 13 383 -3 351 -3 352 -28 402 -46 93 -136 160 -248 184 -78 16 -227 13 -297 -8z m296 -527 c125 -68 136 -223 20 -301 -51 -34 -107 -46 -203 -41 -78 4 -93 8 -129 34 -61 42 -81 85 -77 161 5 87 44 135 134 169 64 24 189 13 255 -22z\"/> <path d=\"M8504 2591 c-66 -40 -64 -29 -64 -427 0 -198 -3 -436 -7 -527 l-6 -167 -54 0 c-30 0 -63 -5 -74 -10 -26 -15 -49 -62 -49 -105 l0 -35 90 0 90 0 0 -310 c0 -184 4 -329 11 -360 15 -71 65 -139 125 -170 56 -29 182 -40 245 -21 78 23 122 113 83 169 -15 21 -16 21 -47 5 -21 -11 -56 -17 -99 -18 -61 0 -70 3 -95 28 l-28 27 -3 325 -3 325 63 0 c129 1 185 32 195 108 l6 42 -132 0 -131 0 0 398 c0 448 0 444 78 514 71 65 209 86 296 47 36 -17 48 -3 48 55 0 93 -52 130 -171 124 -88 -4 -164 -35 -223 -91 l-38 -35 0 64 0 64 -37 0 c-21 0 -52 -9 -69 -19z\"/> <path d=\"M9305 2591 c-23 -10 -46 -29 -53 -45 -9 -20 -12 -147 -12 -501 l0 -475 43 0 c52 0 108 27 124 60 10 19 13 141 13 503 l0 477 -37 0 c-21 0 -56 -9 -78 -19z\"/> <path d=\"M9784 2596 c-65 -28 -64 -24 -64 -408 0 -326 1 -351 21 -402 27 -73 96 -148 167 -183 52 -26 66 -28 172 -28 130 0 171 13 267 86 l53 40 0 -67 0 -67 46 6 c51 5 92 31 111 68 10 18 13 141 13 497 l0 472 -35 0 c-53 0 -114 -28 -130 -61 -12 -23 -15 -84 -15 -327 l0 -298 -26 -52 c-61 -123 -236 -177 -362 -113 -44 22 -73 57 -90 110 -8 23 -12 147 -12 387 l0 354 -42 0 c-24 -1 -57 -7 -74 -14z\"/> <path d=\"M10914 2591 c-69 -42 -64 32 -64 -986 0 -1013 -2 -971 61 -1048 48 -58 100 -80 200 -85 105 -5 147 7 189 54 34 39 40 86 15 120 -14 21 -15 21 -67 3 -99 -34 -177 -17 -203 45 -21 50 -22 1569 -2 1619 36 84 113 136 202 137 66 0 111 -28 135 -83 17 -39 19 -95 25 -862 7 -896 6 -875 66 -948 48 -58 100 -80 200 -85 105 -5 147 7 189 54 34 39 40 86 15 120 -14 21 -15 21 -67 3 -65 -22 -108 -24 -154 -4 -64 26 -61 -2 -65 865 l-4 785 25 45 c37 66 96 103 173 108 52 3 67 0 97 -20 19 -13 44 -41 55 -63 19 -38 20 -63 23 -419 l3 -378 50 4 c54 6 97 31 116 69 16 30 18 640 3 720 -22 116 -91 204 -182 235 -68 22 -195 16 -261 -14 -26 -12 -72 -46 -101 -75 l-53 -55 -19 35 c-26 49 -87 98 -139 113 -59 16 -180 9 -224 -14 -19 -10 -56 -39 -83 -63 l-48 -46 0 67 0 66 -37 0 c-21 0 -52 -9 -69 -19z\"/> <path d=\"M12309 1496 c-120 -33 -230 -129 -269 -233 -20 -53 -21 -72 -18 -298 l3 -240 29 -53 c54 -96 151 -166 268 -192 80 -18 238 -8 308 19 121 46 194 114 207 194 11 71 -37 127 -110 127 -33 0 -35 -3 -46 -44 -28 -113 -155 -174 -301 -146 -119 22 -180 100 -180 227 l0 73 305 0 306 0 25 26 c26 25 26 27 22 152 -4 147 -19 191 -90 268 -86 94 -186 135 -325 133 -48 0 -109 -7 -134 -13z m222 -152 c98 -29 159 -116 159 -228 l0 -46 -245 0 -245 0 0 62 c0 169 150 265 331 212z\"/> <path d=\"M13154 1491 c-67 -41 -64 -16 -64 -544 l0 -480 51 5 c54 6 97 30 116 69 9 17 13 106 13 324 0 268 2 304 19 340 43 95 157 154 273 141 33 -3 68 -11 79 -17 36 -19 49 -4 49 54 0 96 -50 132 -172 125 -85 -4 -162 -36 -220 -91 l-38 -35 0 64 0 64 -37 0 c-21 0 -52 -9 -69 -19z\"/> <path d=\"M5450 1475 c-117 -33 -220 -123 -255 -224 -12 -35 -15 -97 -15 -291 l0 -247 30 -61 c68 -134 199 -202 395 -202 232 0 395 108 395 262 0 62 -36 99 -112 113 -36 7 -37 7 -44 -32 -9 -50 -26 -82 -61 -114 -118 -110 -343 -79 -404 54 -16 37 -19 67 -19 238 0 186 1 198 23 240 83 156 355 156 435 -1 12 -24 22 -57 22 -72 0 -23 4 -28 25 -28 40 0 92 26 110 54 13 20 16 41 13 85 -7 97 -68 165 -189 213 -72 28 -267 35 -349 13z\"/> <path d=\"M6417 1479 c-108 -25 -213 -102 -262 -190 l-30 -54 0 -270 0 -270 27 -46 c32 -55 101 -120 157 -148 133 -68 349 -68 483 0 63 32 143 113 169 172 23 50 24 61 24 292 l0 240 -33 67 c-49 101 -139 172 -257 202 -64 17 -215 20 -278 5z m237 -161 c58 -18 131 -91 146 -147 7 -28 10 -110 8 -232 l-3 -187 -30 -43 c-18 -26 -50 -53 -80 -68 -43 -23 -62 -26 -145 -26 -83 0 -102 3 -145 26 -30 15 -62 42 -80 68 l-30 43 -3 187 c-2 118 1 204 8 230 18 68 81 128 160 152 40 12 149 10 194 -3z\"/> <path d=\"M7274 1471 c-67 -41 -64 -16 -64 -544 l0 -480 51 5 c54 6 97 30 116 69 9 17 13 105 13 322 0 273 2 301 19 335 57 109 209 175 331 142 56 -15 114 -72 129 -125 7 -26 11 -166 11 -392 l0 -353 43 0 c52 0 108 27 124 60 10 19 13 109 11 385 l-3 360 -28 57 c-55 112 -148 169 -289 176 -127 6 -224 -24 -319 -99 l-39 -31 0 66 0 66 -37 0 c-21 0 -52 -9 -69 -19z\"/> <path d=\"M9184 1471 c-67 -41 -64 -16 -64 -544 l0 -480 51 5 c54 6 97 30 116 69 9 17 13 106 13 324 0 340 2 349 78 417 71 65 209 86 296 47 36 -17 48 -3 48 55 0 93 -52 130 -171 124 -88 -4 -164 -35 -223 -91 l-38 -35 0 64 0 64 -37 0 c-21 0 -52 -9 -69 -19z\"/> <path d=\"M10047 1479 c-108 -25 -213 -102 -262 -190 l-30 -54 0 -270 0 -270 27 -46 c32 -55 101 -120 157 -148 133 -68 349 -68 483 0 63 32 143 113 169 172 23 50 24 61 24 292 l0 240 -33 67 c-49 101 -139 172 -257 202 -64 17 -215 20 -278 5z m237 -161 c58 -18 131 -91 146 -147 7 -28 10 -110 8 -232 l-3 -187 -30 -43 c-18 -26 -50 -53 -80 -68 -43 -23 -62 -26 -145 -26 -83 0 -102 3 -145 26 -30 15 -62 42 -80 68 l-30 43 -3 187 c-2 118 1 204 8 230 18 68 81 128 160 152 40 12 149 10 194 -3z\"/> </g> </svg> <h3>Configure controller</h3> <form action=\"/api/settings\" method=\"post\" id=\"SettingsForm\"> <p> <label>Wifi SSID:</label> <input maxlength=\"100\" name=\"wifi_ssid\" id=\"SettingsForm_wifiSSID\"><br><label>WiFi Passphrase:</label> <input maxlength=\"100\" name=\"wifi_pass\" type=\"password\" id=\"SettingsForm_wifiPass\"><br><label>Controller ID</label> <input maxlength=\"30\" name=\"id\" id=\"SettingsForm_id\"><br><label>Animal name</label> <input maxlength=\"30\" name=\"animal_name\" id=\"SettingsForm_animalName\"><br><label>Time Zone</label> <select name=\"time_zone\" id=\"TimeZone\"> <option value=\"CET-1CEST\">Madrid</option> </select> <label for=\"NTPEnabled\">Enabled NTP?</label> <input type=\"checkbox\" id=\"NTPEnabled\" checked/> <input type=\"submit\" value=\"Save\"> </p></form> <script>let form=document.getElementById('SettingsForm'); function submitForm(){var data={}; data['timestamp']=Math.floor(Date.now() / 1000); data['id']=document.getElementById('SettingsForm_id').value; data['animal_name']=document.getElementById('SettingsForm_animalName').value; data['wifi_ssid']=document.getElementById('SettingsForm_wifiSSID').value; data['wifi_password']=document.getElementById('SettingsForm_wifiPass').value; data['time_zone']=document.getElementById('TimeZone').value; data['ntp_enabled']=document.getElementById('NTPEnabled').checked; var dataAsJSON=JSON.stringify(data); sendData({'json': dataAsJSON}, '/api/settings');}function sendData(data, url){console.log('Sending data'); const XHR=new XMLHttpRequest(); const urlEncodedDataPairs=[]; for (const [name, value] of Object.entries(data)){urlEncodedDataPairs.push(`${encodeURIComponent(name)}=${encodeURIComponent(value)}`);}const urlEncodedData=urlEncodedDataPairs.join('&').replace(/%20/g, '+'); XHR.addEventListener('load', (event)=>{}); XHR.addEventListener('error', (event)=>{}); XHR.open('POST', url); XHR.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded'); XHR.send(urlEncodedData);}form.addEventListener('submit', (e)=>{e.preventDefault(); submitForm();}); </script></body></html>";