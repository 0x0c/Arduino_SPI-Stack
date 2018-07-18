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
			uint8_t reset_counter = 0;
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

			Stack(uint8_t buffer_size)
			{
				this->buffer = new uint8_t[buffer_size];
				memset(this->buffer, 0, sizeof(this->buffer));
			}

			~Stack()
			{
				delete this->buffer;
			}

			void setup(SPIMode mode, BitOrder order)
			{
				pinMode(MISO, OUTPUT);
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
				if (c == 0xff) {
					this->reset_counter++;
				}
				else {
					this->reset_counter = 0;
				}

				if (this->reset_counter >= 3) {
					// reset state
					this->pos = 0;
					return;
				}

				if (this->pos < sizeof(this->buffer)) {
					this->buffer[this->pos++] = c;
				}

				if (this->pos >= sizeof(this->buffer)) {
					this->ready_to_process = true;
				}
			}

			bool available()
			{
				return this->ready_to_process;
			}

			void flush()
			{
				this->pos = 0;
				this->ready_to_process = false;
			}
		};
	}
}
}
