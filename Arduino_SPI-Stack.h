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
			volatile char flush_command = 0xff;
			volatile uint8_t flush_command_threshold = 3;
			volatile uint8_t flush_command_count = 0;

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

			volatile uint8_t *buffer;
			volatile bool debug_mode = false;

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

			void set_flush_command_setting(const char command, const int threshold)
			{
				this->flush_command = command;
				this->flush_command_threshold = threshold;
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
				if (this->debug_mode) {
					Serial.print("-----pos: ");
					Serial.print(this->pos);
					Serial.print(", ");
					Serial.print("received: ");
					const char r = c & 0xff;
					Serial.println(r, BIN);
				}
				if (this->flush_command == c) {
					if (this->debug_mode) {
						Serial.println("-----flush command received");
					}
					this->flush_command_count++;
				}
				if (this->flush_command_count >= this->flush_command_threshold) {
					Serial.println("-----call flush command");
					this->flush();
					return;
				}

				if (this->pos < this->buffer_size) {
					this->buffer[this->pos] = c;
					this->pos++;
				}
				if (this->pos >= this->buffer_size) {
					if (this->debug_mode) {
						Serial.println("-----ready to process");
					}
					this->ready_to_process = true;
				}
			}

			bool available()
			{
				return this->ready_to_process;
			}

			void flush()
			{
				if (this->debug_mode) {
					Serial.println("-----flush");
				}

				this->flush_command_count = 0;
				this->pos = 0;
				this->ready_to_process = false;
			}
		};
	}
}
}
