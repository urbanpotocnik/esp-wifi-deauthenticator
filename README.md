# ESP-WiFi-Deauthenticator
This project is an ESP-IDF based WiFi deauthenticator. It allows you to select a WiFi network and send deauthentication frames to it.

## Theory behind the attack
Theoretical background behind this attack can be found on this link: https://blog.spacehuhn.com/wifi-deauthentication-frame

## Building the Project
To build the project, run the following command in the project directory:

idf.py build
idf.py flash
idf.py monitor

after that select the network you want to attack and press enter.

## Important note
For this project to work it is important to use esp-idf version lower than 4.1.2, because the ability of sending this kind of packets was removed in newer versions of the esp-idf.

