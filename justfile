pre:
  arduino-cli core update-index
  arduino-cli core install arduino:avr
  arduino-cli lib install MIDIUSB
  arduino-cli lib install ezButton

compile:
  arduino-cli compile --fqbn arduino:avr:leonardo .

upload:
  arduino-cli upload -p /dev/ttyACM0 --fqbn arduino:avr:leonardo .

all: pre compile upload
