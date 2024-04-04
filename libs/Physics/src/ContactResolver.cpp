#include "ContactResolver.h"

namespace Physics
{
	ContactResolver::ContactResolver(Body* bodyA, Body* bodyB, Collider* colliderA, Collider* colliderB) noexcept
		: _bodyA(bodyA), _bodyB(bodyB), _colliderA(colliderA), _colliderB(colliderB)
	{}

	void ContactResolver::ResolveContact() noexcept
	{
		if (_colliderA->GetShapeType() == Math::ShapeType::Polygon || _colliderB->GetShapeType() == Math::ShapeType::Polygon) return;

		setupContact();

		if (_bodyA->GetBodyType() == BodyType::Static || _bodyA->GetBodyType() == BodyType::Kinematic)
		{
			std::swap(_bodyA, _bodyB);
			std::swap(_colliderA, _colliderB);

			_normal = -_normal;
		}

		resolveCollision();
		resolvePosition();
	}

	void ContactResolver::setupContact() noexcept
	{
		switch (_colliderA->GetShapeType())
		{
			case Math::ShapeType::Circle:
				switch (_colliderB->GetShapeType())
				{
					case Math::ShapeType::Circle: resolveCircleToCircle(); break;
					case Math::ShapeType::Rectangle: resolveCircleToRectangle(); break;
					case Math::ShapeType::Polygon:
					case Math::ShapeType::None:break;
				}
				break;
			case Math::ShapeType::Rectangle:
				switch (_colliderB->GetShapeType())
				{
					case Math::ShapeType::Circle:
					{
						std::swap(_bodyA, _bodyB);
						std::swap(_colliderA, _colliderB);

						resolveCircleToRectangle();
					}
					break;
					case Math::ShapeType::Rectangle: resolveRectangleToRectangle(); break;
					case Math::ShapeType::Polygon:
					case Math::ShapeType::None:break;
				}
				break;
			case Math::ShapeType::Polygon:
			case Math::ShapeType::None:break;
		}
	}

	void ContactResolver::resolveCollision() noexcept
	{
		const auto& separatingVelocity = (_bodyA->Velocity() - _bodyB->Velocity()).Dot(_normal);
		const auto& inverseMassA = _bodyA->InverseMass();
		const auto& inverseMassB = _bodyB->InverseMass();
		const auto& totalInverseMass = inverseMassA + inverseMassB;

		if (separatingVelocity > 0.0f) return;

		const auto& massA = _bodyA->Mass();
		const auto& massB = _bodyB->Mass();
		const auto& restitutionA = _colliderA->GetRestitution();
		const auto& restitutionB = _colliderB->GetRestitution();

		const auto& combinedRestitution = (massA * restitutionA + massB * restitutionB) / (massA + massB);
		const auto& finalSeparatingVelocity = -separatingVelocity * combinedRestitution;
		const auto& deltaVelocity = finalSeparatingVelocity - separatingVelocity;

		const auto& impulse = deltaVelocity / totalInverseMass;
		const auto& impulsePerMass = impulse * _normal;

		if (_bodyA->GetBodyType() == BodyType::Dynamic)
		{
			_bodyA->AddVelocity(impulsePerMass * inverseMassA);
		}

		if (_bodyB->GetBodyType() == BodyType::Dynamic)
		{
			_bodyB->AddVelocity(impulsePerMass * -inverseMassB);
		}
	}

	void ContactResolver::resolvePosition() noexcept
	{
		const auto& inverseMassA = _bodyA->InverseMass();
		const auto& inverseMassB = _bodyB->InverseMass();
		const auto& totalInverseMass = inverseMassA + inverseMassB;

		if (totalInverseMass <= 0.0f) return;
		if (_penetration <= 0.0f) return;

		const auto& movePerMass = _normal * (_penetration / totalInverseMass);

		if (_bodyA->GetBodyType() == BodyType::Dynamic)
		{
			_bodyA->AddPosition(movePerMass * inverseMassA);
		}

		if (_bodyB->GetBodyType() == BodyType::Dynamic)
		{
			_bodyB->AddPosition(movePerMass * -inverseMassB);
		}
	}

	void ContactResolver::resolveCircleToCircle() noexcept
	{
		const auto& circleA = _colliderA->GetCircle();
		const auto& circleB = _colliderB->GetCircle();
		const auto& radiusA = circleA.Radius();
		const auto& radiusB = circleB.Radius();
		const auto& positionA = _bodyA->Position() + circleA.Center() + _colliderA->GetOffset();
		const auto& positionB = _bodyB->Position() + circleB.Center() + _colliderB->GetOffset();

		const auto& delta = positionA - positionB;

		_normal = delta.Normalized();
		_penetration = radiusA + radiusB - delta.Length();
	}

	void ContactResolver::resolveRectangleToRectangle() noexcept
	{
		const auto& rectangleA = _colliderA->GetRectangle();
		const auto& rectangleB = _colliderB->GetRectangle();
		const auto& positionA = _bodyA->Position() + rectangleA.Center() + _colliderA->GetOffset();
		const auto& positionB = _bodyB->Position() + rectangleB.Center() + _colliderB->GetOffset();

		const auto& delta = positionA - positionB;

		const auto& halfSizeA = rectangleA.HalfSize();
		const auto& halfSizeB = rectangleB.HalfSize();

		const auto& penetrationX = halfSizeA.X + halfSizeB.X - std::abs(delta.X);
		const auto& penetrationY = halfSizeA.Y + halfSizeB.Y - std::abs(delta.Y);

		if (penetrationX < penetrationY)
		{
			_normal = delta.X > 0 ? Math::Vec2F::Right() : Math::Vec2F::Left();
			_penetration = penetrationX;
		}
		else
		{
			_normal = delta.Y > 0 ? Math::Vec2F::Up() : Math::Vec2F::Down();
			_penetration = penetrationY;
		}
	}

	void ContactResolver::resolveCircleToRectangle() noexcept
	{
		const auto& circle = _colliderA->GetCircle();
		const auto& rectangle = _colliderB->GetRectangle();
		const auto& circleCenter = _bodyA->Position() + circle.Center() + _colliderA->GetOffset();
		const auto& rectCenter = _bodyB->Position() + rectangle.Center() + _colliderB->GetOffset();

		const auto& delta = circleCenter - rectCenter;
		const auto& radius = circle.Radius();
		const auto& halfSize = rectangle.HalfSize();
		Math::Vec2F closestPoint
		{
			Math::Clamp(delta.X, -halfSize.X, halfSize.X),
			Math::Clamp(delta.Y, -halfSize.Y, halfSize.Y)
		};

		const auto& distance = (closestPoint - delta).Length();
		const auto& closestPointRect = rectCenter + closestPoint;
		auto circleToRect = circleCenter - closestPointRect;

		if (circleToRect.Length() <= Math::Epsilon)
		{
			circleToRect = Math::Vec2F::Up();
		}

		_normal = circleToRect.Normalized();
		_penetration = radius - distance;
	}
}