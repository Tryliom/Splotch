#pragma once

#include <SFML/Network.hpp>

#include <utility>

enum class PacketType :
	char
{
	Invalid,
	UDPAcknowledge,
	ConfirmUDPConnection,
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

class UDPAcknowledgePacket final :
	public Packet
{
 public:
	UDPAcknowledgePacket() : Packet(static_cast<char>(PacketType::UDPAcknowledge)) {}
	explicit UDPAcknowledgePacket(unsigned short port) : Packet(static_cast<char>(PacketType::UDPAcknowledge)), Port(port) {}

	// Port of the sender in TCP, used to identify the client
	unsigned short Port = 0;

	[[nodiscard]] Packet* Clone() const override
	{
		return new UDPAcknowledgePacket();
	}
	[[nodiscard]] std::string ToString() const override
	{
		return "UDPAcknowledgePacket";
	}
	void Write(sf::Packet& packet) const override
	{
		packet << Port;
	}
	void Read(sf::Packet& packet) override
	{
		packet >> Port;
	}
};

class ConfirmUDPConnectionPacket final :
	public Packet
{
 public:
	ConfirmUDPConnectionPacket() : Packet(static_cast<char>(PacketType::ConfirmUDPConnection)) {}

	[[nodiscard]] Packet* Clone() const override
	{
		return new ConfirmUDPConnectionPacket();
	}
	[[nodiscard]] std::string ToString() const override
	{
		return "ConfirmUDPConnectionPacket";
	}
	void Write(sf::Packet& packet) const override {}
	void Read(sf::Packet& packet) override {}
};

sf::Packet& operator<<(sf::Packet& packet, const Packet& myPacket);
sf::Packet& operator>>(sf::Packet& packet, Packet& myPacket);