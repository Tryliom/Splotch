#pragma once

#include <SFML/Network.hpp>

#include <utility>

enum class PacketType :
	char
{
	Invalid,
	COUNT // Always last
};

// Packet attributes always need to be initialized to default values
class Packet
{
 public:
	Packet() = default;
	explicit Packet(char type) : Type(type) {}
	virtual ~Packet() = default;

	/**
	 * @brief Type of the packet, used to determine which packet to create from a received packet
	 */
	char Type = static_cast<char>(PacketType::Invalid);

	[[nodiscard]] virtual Packet* Clone() const = 0;
	[[nodiscard]] virtual std::string ToString() const = 0;
	/**
	 * @brief Write the packet to a sf::Packet, type is already written first
	 * @param packet sf::Packet to write to
	 */
	virtual void Write(sf::Packet& packet) const = 0;
	/**
	 * @brief Read the packet from a sf::Packet, type is already read first
	 * @param packet sf::Packet to read from
	 */
	virtual void Read(sf::Packet& packet) = 0;

	template<typename T>
	T* As()
	{
		if (typeid(*this) == typeid(T)) return static_cast<T*>(this);
		else return nullptr;
	}
};

class InvalidPacket final :
	public Packet
{
 public:
	InvalidPacket() = default;

	[[nodiscard]] Packet* Clone() const override
	{
		return new InvalidPacket();
	}
	[[nodiscard]] std::string ToString() const override
	{
		return "InvalidPacket";
	}
	void Write(sf::Packet& packet) const override
	{
	}
	void Read(sf::Packet& packet) override
	{
	}
};

sf::Packet& operator<<(sf::Packet& packet, const Packet& myPacket);
sf::Packet& operator>>(sf::Packet& packet, Packet& myPacket);