#include <Arduino_SPI-Stack.h>

m2d::Arduino::SPI::Stack spi_stack = m2d::Arduino::SPI::Stack(2);

void setup()
{
	Serial.begin(9600);
	spi_stack.setup();
}

void loop()
{
	if (spi_stack.available()) {
		Serial.println("Receive data: ");
		Serial.print(spi_stack.buffer[0], BIN);
		Serial.print(" ");
		Serial.println(spi_stack.buffer[1], BIN);
	}
}

ISR(SPI_STC_vect)
{
	byte c = SPDR; // grab byte from SPI Data Register
	spi_stack.process_data(c);
}
