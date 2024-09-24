read_flash:
	avrdude -p attiny13 -Pusb -c usbasp -U flash:r:flash.bin:r

write_flash:
	avrdude -p attiny13 -Pusb -c usbasp -U flash:w:electric_candle.bin:r