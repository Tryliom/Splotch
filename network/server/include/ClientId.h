#pragma once

class ClientId
{
public:
	static constexpr int EMPTY_INDEX = -1;

	int Index = EMPTY_INDEX;

	[[nodiscard]] bool IsEmpty() const
	{
		return Index == EMPTY_INDEX;
	}

	bool operator==(const ClientId& other) const
	{
		return Index == other.Index;
	}
};

static constexpr ClientId EMPTY_CLIENT_ID { ClientId::EMPTY_INDEX };