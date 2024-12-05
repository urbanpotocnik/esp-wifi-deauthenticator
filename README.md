# ESP-WiFi-Deauthenticator
This project is an ESP-IDF based WiFi deauthenticator. On startup the program scans for available networks and displays them in the terminal. After that users can select which network they want to attack and the deauthentication frame is sent to the AP which disconnects all users.

## Theory behind the attack
Theoretical background behind this attack can be found on this link: https://blog.spacehuhn.com/wifi-deauthentication-frame

## Building the Project
To build the project, run the following command in the project directory:

* `idf.py build`
* `idf.py flash`
* `idf.py monitor`
* `after that select the network you want to attack and press enter.`

## Important note
For this project to work it is important to use esp-idf version lower than 4.1.2, because the ability of sending this kind of packets was removed in newer versions of the esp-idf.

