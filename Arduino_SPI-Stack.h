#pragma once

#include <SPI.h>
#include <StandardCplusplus.h>
#include <string>

namespace m2d
{
namespace Arduino
{
	namespace SPI
	{
		class Stack
		{
		private:
			volatile uint8_t buffer_size = 0;
			volatile uint8_t pos = 0;
			volatile bool ready_to_process = false;

		public:
			typedef enum
			{
				Mode0,
				Mode1,
				Mode2,
				Mode3,
			} SPIMode;

			typedef enum
			{
				LSBFirst,
				MSBFirst
			} BitOrder;

			uint8_t *buffer;

			Stack(uint8_t size)
			{
				this->buffer_size = size;
				this->buffer = new uint8_t[size];
				memset(this->buffer, 0, sizeof(this->buffer));
			}

			~Stack()
			{
				delete this->buffer;
			}

			void setup(SPIMode mode, BitOrder order, uint8_t clock_divider)
			{
				pinMode(MISO, OUTPUT);
				SPIClass::setClockDivider(clock_divider);
				SPCR |= _BV(SPE);

				switch (mode) {
					case Mode0:
						SPCR |= (0 << CPHA);
						SPCR |= (0 << CPOL);
						break;
					case Mode1:
						SPCR |= (0 << CPHA);
						SPCR |= (1 << CPOL);
						break;
					case Mode2:
						SPCR |= (1 << CPHA);
						SPCR |= (0 << CPOL);
						break;
					case Mode3:
						SPCR |= (1 << CPHA);
						SPCR |= (1 << CPOL);
						break;
				}

				if (order == LSBFirst) {
					SPCR |= (1 << DORD);
				}

				// SPI slave
				SPCR &= ~(1 << MSTR);
				// enable SPI
				SPCR |= (1 << SPE);
				// enable SPI interrupt
				SPCR |= (1 << SPIE);
			}

			void process_data(char c)
			{
				Serial.print("pos: ");
				Serial.print(this->pos);
				Serial.print(", ");
				Serial.print("received: ");
				Serial.println(c, BIN);
				if (this->pos < this->buffer_size) {
					this->buffer[this->pos] = c;
					this->pos++;
				}
				if (this->pos >= this->buffer_size) {
					this->ready_to_process = true;
				}
			}

			bool available()
			{
				return this->ready_to_process;
			}

			void flush()
			{
				Serial.println("flush");
				this->pos = 0;
				this->ready_to_process = false;
			}
		};
	}
}
}
