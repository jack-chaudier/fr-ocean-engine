//
//  Rigidbody.cpp
//  game_engine
//
//  Created by Jack Gaffney on 4/13/25.
//

#include "Rigidbody.hpp"
#include "RigidbodyWorld.hpp"
#include "glm/glm.hpp"


void Rigidbody::Init(Actor * owner) {
	b2BodyDef bodyDef;

    if (body_type == "dynamic")
        bodyDef.type = b2_dynamicBody;
    else if (body_type == "static")
        bodyDef.type = b2_staticBody;
    else if (body_type == "kinematic")
        bodyDef.type = b2_kinematicBody;
    else
        bodyDef.type = b2_dynamicBody;

    bodyDef.position.Set(x, y);

    bodyDef.bullet = precise;
    
    bodyDef.gravityScale = gravity_scale;
    bodyDef.angularDamping = angular_friction;
    
    bodyDef.angle = rotation * (b2_pi / 180.0f);
    
    RigidbodyWorld::Init();
    
    body = RigidbodyWorld::AddRigidbody(bodyDef);
    if (!body) return;

    if (has_collider) {
        b2FixtureDef colDef;
        if (collider_type == "box") {
            static b2PolygonShape boxShape;
            boxShape.SetAsBox(width * 0.5f, height * 0.5f);
            colDef.shape = &boxShape;
        } else {
            static b2CircleShape circleShape;
            circleShape.m_radius = radius;
            colDef.shape = &circleShape;
        }

        colDef.density    = density;
        colDef.friction   = friction;
        colDef.restitution= bounciness;
        colDef.isSensor   = false;

        colDef.filter.categoryBits = 0x0001;
        colDef.filter.maskBits     = 0x0001;

        body->CreateFixture(&colDef);
    }

    if (has_trigger) {
        b2FixtureDef trigDef;
        trigDef.isSensor = true;
        trigDef.density  = density;

        if (trigger_type == "box") {
            static b2PolygonShape boxShape;
            boxShape.SetAsBox(trigger_width * 0.5f,
                              trigger_height * 0.5f);
            trigDef.shape = &boxShape;
        } else {
            static b2CircleShape circleShape;
            circleShape.m_radius = trigger_radius;
            trigDef.shape = &circleShape;
        }

        trigDef.filter.categoryBits = 0x0002;
        trigDef.filter.maskBits     = 0x0002;

        body->CreateFixture(&trigDef);
    }

    if (!has_collider && !has_trigger) {
        b2PolygonShape phantomShape;
        phantomShape.SetAsBox(width * 0.5f, height * 0.5f);

        b2FixtureDef phantomDef;
        phantomDef.shape     = &phantomShape;
        phantomDef.isSensor  = true;
        phantomDef.density = density;
        phantomDef.filter.categoryBits = 0x0002;
        phantomDef.filter.maskBits     = 0x0000;

        body->CreateFixture(&phantomDef);
    }

    for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext()) {
        f->GetUserData().pointer =
            reinterpret_cast<uintptr_t>(owner);
    }
}

void Rigidbody::OnDestroy() {
    if (body) {
        RigidbodyWorld::GetWorld()->DestroyBody(body);
        body = nullptr;
    }
}

b2Vec2 Rigidbody::GetPosition() const {
    if (body)
        return body->GetPosition();
    return b2Vec2(x, y);
}

float Rigidbody::GetRotation() const {
    if (body)
        return body->GetAngle() * (180.0f / b2_pi);
    return rotation;
}

void Rigidbody::AddForce(const b2Vec2& force) {
    if (body)
        body->ApplyForceToCenter(force, true);
}

void Rigidbody::SetVelocity(const b2Vec2& velocity) {
    if (body)
        body->SetLinearVelocity(velocity);
}

void Rigidbody::SetPosition(const b2Vec2& position) {
    if (body)
        body->SetTransform(position, body->GetAngle());
}

void Rigidbody::SetRotation(float degrees_clockwise) {
    if (body)
        body->SetTransform(body->GetPosition(), degrees_clockwise * (b2_pi / 180.0f));
}

void Rigidbody::SetAngularVelocity(float degrees_clockwise) {
    if (body)
        body->SetAngularVelocity(degrees_clockwise * (b2_pi / 180.0f));
}

void Rigidbody::SetGravityScale(float scale) {
    if (body)
        body->SetGravityScale(scale);
}

void Rigidbody::SetUpDirection(b2Vec2 direction) {
    if (body) {
        direction.Normalize();
        float angle = glm::atan(direction.x, -direction.y);
        body->SetTransform(body->GetPosition(), angle);
    }
}

void Rigidbody::SetRightDirection(b2Vec2 direction) {
    if (body) {
        direction.Normalize();
        float angle = glm::atan(direction.x, -direction.y) - b2_pi / 2.0f;
        body->SetTransform(body->GetPosition(), angle);
    }
}

b2Vec2 Rigidbody::GetVelocity() const {
    if (body)
        return body->GetLinearVelocity();
    return b2Vec2(0.0f, 0.0f);
}

float Rigidbody::GetAngularVelocity() const {
    if (body)
        return body->GetAngularVelocity() * (180.0f / b2_pi);
    return 0.0f;
}

float Rigidbody::GetGravityScale() const {
    if (body)
        return body->GetGravityScale();
    return gravity_scale;
}

b2Vec2 Rigidbody::GetUpDirection() const {
    if (body) {
        float angle = body->GetAngle();
        b2Vec2 result = b2Vec2(glm::sin(angle), -glm::cos(angle));
        result.Normalize();
        return result;
    }
    return b2Vec2(0.0f, -1.0f);
}

b2Vec2 Rigidbody::GetRightDirection() const {
    if (body) {
        float angle = body->GetAngle();
        b2Vec2 result = b2Vec2(glm::cos(angle), glm::sin(angle));
        result.Normalize();
        return result;
    }
    return b2Vec2(1.0f, 0.0f);
}
