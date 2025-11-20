core := "arduino:avr"
board := "leonardo"

pre:
  arduino-cli core update-index
  arduino-cli core install {{core}}
  arduino-cli lib install MIDIUSB
  arduino-cli lib install ezButton

compile:
  arduino-cli compile --fqbn {{core}}:{{board}} .

upload:
  arduino-cli upload -p /dev/ttyACM0 --fqbn {{core}}:{{board}} .

attach:
  arduino-cli board attach -p /dev/ttyACM0 -b {{core}}:{{board}} .

all: pre compile upload
