<h1>LED Clock</h1>

<img align="right" src="hardware/images/Clock5.jpg" alt="Clock" width="400" />

<p>Clock with WS2812 LEDs, controlled by a NodeMCU ESP8266 module.</p>
<p><span style="color: red">DO NOT EXPOSE THE CLOCK TO THE INTERNET<br/>The clock stores your WiFi credentials and supports remote updates</span></p>

<h2>Features</h2>
<ul>
  <li>Setting color for hour, minute, second</li>
  <li>Brighness control</li>
  <li>Use NTP for time synchronization</li>
  <li>Convert from UTC to local time and DST</li>
  <li>Manual and automatic time triggered On/Off</li>
  <li>OTA Update</li>
  <li>Optional secure HTTPS communication</li>
</ul>

<h2>Used Libraries</h2>
<ul>
  <li>Arduino 1.8.0</li>
  <li><a href="https://github.com/esp8266/Arduino">Arduino Core for ESP8266</a> (Version 2.4.2 - I had OTA Update problems with 2.5.0)</li>
  <li><a href="https://github.com/adafruit/Adafruit_NeoPixel">Adafruit NeoPixel Library</a> (Version 1.2.1)</li>
</ul>

<h2>Material</h2>
<ul>
  <li>NodeMCU v2</li>
  <li>Buck converter 12V to 5V</li>
  <li>60x WS2812 LEDs on PCB</li>
  <li>Photo resistor 5539</li>
  <li>3D printed housing</li>
  <li>Acryl glass front plate</li>
</ul>

<h2>Setup</h2>
<ol>
  <li>Open the Arduino IDE</li>
  <li>Install the required libraries</li>
  <li>Check the <a href="source/ArduinoSettings.png">Board Settings</a></li>
  <li>Load the Clock.ino sketch</li>
  <li>Modify the Password in password.h (it is used as PSK in Access Point mode and OTA Update password)</li>
  <li>Flash the ESP8266 using Arduino IDE</li>
  <li>
    Connect the NodeMCU with the buck converter, WS2812 LEDs and the photo resistor:<br/>
    <img src="images/Clock.sch.png" width="600" alt="schematic"/><br/>
  </li>
</ol>

<h2>First Start</h2>
<ul>
  <li>Open the Serial Monitor in Arduino IDE</li>
  <li>Power on / reset the Clock</li>
  <li>Connect to Network with the SSID "Clock-xxxxxx" and the password specified in the password.h file (both are shown in the Serial Monitor)</li>
  <li>Open the web page "http://192.168.4.1"</li>
  <li>On the Settings page enter the credentials (SSID and PSK) of your Wireless LAN and press "save"</li>
  <li>The clock will now connect to your network, fetch the current time and display it</li>
  <li>Check your router for the network address of the Clock and open the page in the browser to check the other settings</li>
  <li>At next power on the clock will remember the settings</li>
</ul>

<h2>Usage</h2>
<ul>
  <li>The LEDs can be switched on/off manually and automatically. If automatic mode is enabled and the LEDs are switched manually, the automatic mode stays enabled and will switch at the next configured time again.</li>
  <li>The OTA Update username is the Name specified on the Settings page and the password is the one specified in password.h file</li>
</ul>

<h2>Security</h2>
<p>By default the Clock does not use https for communication, it's too slow. Https can be enabled in Clock.ino (#define HTTPS ...).</p>
<p>The repository contains a certificate and private key, but you should use your own. To create and upload them:</p>
<ol>
  <li>Run the script "source/make-cert.sh" to create the certificate file "cert.der" and the private key file "key.der". The script requires <a href="https://www.openssl.org/">OpenSSL</a> to be installed.</li>
  <li>Create the directory "source/Clock/data" and copy the generated files into that directory.</li>
  <li>Install the SPIFS tool for Arduino as described in the <a href="https://arduino-esp8266.readthedocs.io/en/latest/filesystem.html#uploading-files-to-file-system">ESP8266 documentation</a>.</li>
  <li>Use the Upload Tool to upload the files to the ESP8266.</li> 
</ol>

<h2>3D Print</h2>

<p>The OpenSCAD file to 3D Print the housing is in the hardware folder. STL files are available at <a href="https://www.thingiverse.com/thing:3130435">Thingiverse</a><br/>
An alternative to 3D Printing is to use a WS2812 LED ring with 60 LEDs</p>

<p>
<a style="vertical-align: top" href="hardware/images/Clock1.jpg"><img src="hardware/images/Clock1.jpg" alt="clock 1" width="250" border="1px"/></a>
<a style="vertical-align: top" href="hardware/images/Clock2.jpg"><img src="hardware/images/Clock2.jpg" alt="clock 1" width="250" border="1px"/></a>
<a style="vertical-align: top" href="hardware/images/Clock3.jpg"><img src="hardware/images/Clock3.jpg" alt="clock 1" width="250" border="1px"/></a>
<a style="vertical-align: top" href="hardware/images/Clock4.jpg"><img src="hardware/images/Clock4.jpg" alt="clock 1" width="250" border="1px"/></a>
</p>

<h2>UI Screenshots</h2>
<p>
<a style="vertical-align: top" href="images/home.png"    ><img src="images/home.png"     alt="home"     width="250" border="1px"/></a>
<a style="vertical-align: top" href="images/settings.png"><img src="images/settings.png" alt="settings" width="250" border="1px"/></a>
<a style="vertical-align: top" href="images/update.png"  ><img src="images/update.png"   alt="update"   width="250" border="1px"/></a>
</p>
