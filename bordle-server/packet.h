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
	serialized.emplace_back(packet.message_type());
	serialized.emplace_back(packet.message_content());
	return serialized;
}

class Packet {
public:
	Packet() = delete;
	Packet(unsigned short size, Error error, std::vector<std::byte> error_message) 
		: m_size{size}, m_message_type{static_cast<std::byte>(error)}, m_message_content{error_message}
	{}
	Packet(unsigned short size, Operation operation, std::vector<std::byte> operand)
		: m_size{size}, m_message_type{static_cast<std::byte>(operation)}, m_message_content{operand}
	{}
	unsigned short		   size()			 const { return m_size; }
	std::byte			   message_type()	 const { return m_message_type; }
	std::vector<std::byte> message_content() const { return m_message_content; }
private:
	unsigned short m_size;
	std::byte m_message_type;
	std::vector<std::byte> m_message_content;

};

