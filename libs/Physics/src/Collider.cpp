#include "Collider.h"

namespace Physics
{
	BodyRef Collider::GetBodyRef() const noexcept
	{
		return _bodyRef;
	}

	ColliderRef Collider::GetColliderRef() const noexcept
	{
		return _colliderRef;
	}

    Math::Vec2F Collider::GetOffset() const noexcept
    {
        return _offset;
    }

	Math::Vec2F Collider::GetPosition() const noexcept
	{
		return _position;
	}

    float Collider::GetRestitution() const noexcept
	{
		return _bounciness;
	}

	bool Collider::IsTrigger() const noexcept
	{
		return _isTrigger;
	}

	bool Collider::IsFree() const noexcept
	{
		return _shapeType == Math::ShapeType::None;
	}

	bool Collider::IsEnabled() const noexcept
	{
		return !IsFree() && _isEnabled;
	}

	void Collider::SetBodyRef(BodyRef bodyRef) noexcept
	{
		_bodyRef = bodyRef;
	}

	void Collider::SetColliderRef(ColliderRef colliderRef) noexcept
	{
		_colliderRef = colliderRef;
	}

    void Collider::SetOffset(Math::Vec2F offset) noexcept
    {
        _offset = offset;
    }

	void Collider::SetPosition(Math::Vec2F position) noexcept
	{
		_position = position;
	}

	void Collider::SetBounciness(float bounciness) noexcept
	{
		_bounciness = bounciness;
	}

	void Collider::SetIsTrigger(bool isTrigger) noexcept
	{
		_isTrigger = isTrigger;
	}

	void Collider::SetCircle(Math::CircleF circle) noexcept
	{
		_shapeType = Math::ShapeType::Circle;
		_shape = circle;
        _bounds = getBounds();
	}

	void Collider::SetRectangle(Math::RectangleF rectangle) noexcept
	{
		_shapeType = Math::ShapeType::Rectangle;
		_shape = rectangle;

        _bounds = getBounds();
	}

	void Collider::SetPolygon(Math::PolygonF polygon) noexcept
	{
		_shapeType = Math::ShapeType::Polygon;
		_shape = polygon;

        _bounds = getBounds();
	}

	void Collider::Enable() noexcept
	{
		_isEnabled = true;
	}

	void Collider::Disable() noexcept
	{
		_isEnabled = false;
	}

	void Collider::Free() noexcept
	{
		_isEnabled = false;
		_bounciness = 0.f;
		_isTrigger = false;
		_shapeType = Math::ShapeType::None;
	}

	Math::CircleF Collider::GetCircle() const noexcept
	{
	    return std::get<Math::CircleF>(_shape);
	}

	Math::RectangleF Collider::GetRectangle() const noexcept
	{
		return std::get<Math::RectangleF>(_shape);
	}

	Math::PolygonF Collider::GetPolygon() const noexcept
	{
        return std::get<Math::PolygonF>(_shape);
	}

	Math::ShapeType Collider::GetShapeType() const noexcept
	{
		return _shapeType;
	}

	Math::RectangleF Collider::getBounds() const noexcept
	{
		switch (_shapeType)
		{
			case Math::ShapeType::Circle:
			{
				const auto& circle = GetCircle();

				return Math::RectangleF::FromCenter(circle.Center(), {circle.Radius(), circle.Radius()});
			}
			case Math::ShapeType::Rectangle:
			{
				return GetRectangle();
			}
			case Math::ShapeType::Polygon:
			{
				float minX = std::numeric_limits<float>::max();
				float minY = std::numeric_limits<float>::max();
				float maxX = std::numeric_limits<float>::min();
				float maxY = std::numeric_limits<float>::min();
				Math::PolygonF polygon = GetPolygon();

				for (auto& vertex : polygon.Vertices())
				{
					minX = std::min(minX, vertex.X);
					minY = std::min(minY, vertex.Y);
					maxX = std::max(maxX, vertex.X);
					maxY = std::max(maxY, vertex.Y);
				}

				return Math::RectangleF{ Math::Vec2F{minX, minY}, Math::Vec2F{maxX, maxY} };
			}
			case Math::ShapeType::None: break;
		}

		return {Math::Vec2F::Zero(), Math::Vec2F::Zero()};
	}

    Math::RectangleF Collider::GetBounds() const noexcept
    {
        return _bounds + _position;
    }
}