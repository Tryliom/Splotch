#include "Body.h"

namespace Physics
{
	Body::Body(Math::Vec2F position, Math::Vec2F velocity) noexcept
	{
		_position = position;
		_velocity = velocity;
	}

	[[nodiscard]] Math::Vec2F Body::Position() const noexcept
	{
		return _position;
	}

	void Body::SetPosition(Math::Vec2F position) noexcept
	{
		_position = position;
	}

	[[nodiscard]] Math::Vec2F Body::Velocity() const noexcept
	{
		return _velocity;
	}

	void Body::SetVelocity(Math::Vec2F velocity) noexcept
	{
        if (_bodyType == BodyType::Static) return;

		_velocity = velocity;
	}

	[[nodiscard]] Math::Vec2F Body::Force() const noexcept
	{
		return _force;
	}

	void Body::SetForce(Math::Vec2F force) noexcept
	{
		_force = force;
	}

	[[nodiscard]] float Body::Mass() const noexcept
	{
		return _mass;
	}

	[[nodiscard]] float Body::InverseMass() const noexcept
	{
		return _inverseMass;
	}

	void Body::SetMass(float mass) noexcept
	{
		if (mass <= 0.f || _bodyType != BodyType::Dynamic) return;

		_mass = mass;
		_inverseMass = 1.f / mass;
	}

    [[nodiscard]] BodyType Body::GetBodyType() const noexcept
    {
        return _bodyType;
    }

    void Body::SetBodyType(BodyType bodyType) noexcept
    {
        _bodyType = bodyType;

        if (bodyType == BodyType::Dynamic) return;

	    _mass = 0.f;
	    _inverseMass = 0.f;
	    _velocity = Math::Vec2F::Zero();
    }

    [[nodiscard]] bool Body::UseGravity() const noexcept
    {
        return _useGravity;
    }

    void Body::SetUseGravity(bool useGravity) noexcept
    {
        _useGravity = useGravity;
    }

	void Body::AddForce(Math::Vec2F force) noexcept
	{
		_force += force;
	}

    void Body::AddVelocity(Math::Vec2F velocity) noexcept
    {
        _velocity += velocity;
    }

    void Body::AddPosition(Math::Vec2F position) noexcept
    {
        _position += position;
    }

	void Body::Disable() noexcept
	{
		_mass = -1.f;

		_position = Math::Vec2F(0, 0);
		_velocity = Math::Vec2F(0, 0);
		_force = Math::Vec2F(0, 0);
		_inverseMass = 0.f;
        _bodyType = BodyType::Dynamic;
	}

	void Body::Enable() noexcept
	{
		_mass = 1.f;
		_inverseMass = 1.f;
	}

	[[nodiscard]] bool Body::IsEnabled() const noexcept
	{
		return _mass >= 0.f;
	}
}