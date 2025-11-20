core := "arduino:avr"
board := "leonardo"
port := "/dev/ttyACM0"

list:
  arduino-cli board list

pre:
  arduino-cli config init
  arduino-cli core update-index
  arduino-cli core install {{core}}
  arduino-cli lib install MIDIUSB
  arduino-cli lib install ezButton

compile:
  arduino-cli compile --fqbn {{core}}:{{board}} .

upload:
  arduino-cli upload -p {{port}} --fqbn {{core}}:{{board}} .

attach:
  arduino-cli board attach -p {{port}} -b {{core}}:{{board}} .

all: pre compile upload

update: compile upload
