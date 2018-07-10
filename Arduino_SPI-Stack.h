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
			char *buffer;

			Stack(uint8_t buffer_size)
			{
				this->buffer = new char[buffer_size];
			}

			~Stack()
			{
				delete this->buffer;
			}

			void setup()
			{
				// have to send on master in, *slave out*
				pinMode(MISO, OUTPUT);
				// turn on SPI in slave mode
				SPCR |= _BV(SPE);
				// turn on interrupts
				SPCR |= bit(SPIE);
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
