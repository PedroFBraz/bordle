#pragma once
#include <cstddef>
#include <array>
#include <vector>
#include <bit>
#include <cstring>

class Packet;

enum class Error : unsigned char {

};

enum class Operation : unsigned char {

};

std::vector<std::byte> serialize_packet(const Packet& packet) {
	std::vector<std::byte> serialized;
	// sizeof(unsigned short) may better
	const auto size_in_bytes = std::bit_cast<std::array<std::byte, 2>>(packet.size());
	// Endianness might become a problem later on
	serialized.emplace_back(size_in_bytes[0]);
	serialized.emplace_back(size_in_bytes[1]);
	serialized.emplace_back(packet.operation());
	serialized.emplace_back(packet.operand());
	return serialized;
}
/* 
   It might be better to store the errorand the operation into a single byte,
   as an operation cannot success if an error ocurred
*/
class Packet {
public:
	Packet() = delete;
	Packet(unsigned short size, Error error, Operation operation, std::vector<std::byte> operand) 
		: m_size{ size }, m_error{ error }, m_operation{ operation }, m_operand{ operand }
	{}
	unsigned short		   size()	   const { return m_size; }
	Error				   error()	   const { return m_error; }
	Operation			   operation() const { return m_operation; }
	std::vector<std::byte> operand()   const { return m_operand; }
private:
	unsigned short m_size;
	Error m_error;
	Operation m_operation;
	std::vector<std::byte> m_operand;
};

