#ifndef BIRD_H
#define BIRD_H

#include <cstddef>
#include <memory>
#include "GL\glew.h"
#include "GL\SOIL.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "physic.h"
#include "drawAble.h"
#include "shader.h"
#include "collisionWorld.h"
#include "config.h"
#include "displayBoard.h"

std::vector<const char*> origin_tex = {
						 "texture//birdNormal.png", "texture//birdFlutterDownNormal.png", "texture//birdFlutterUpNormal.png",
						 "texture//birdNormalFly.png", "texture//birdFlutterDownFly.png", "texture//birdFlutterUpFly.png",
						 "texture//birdNormalFall.png", "texture//birdFlutterDownFall.png", "texture//birdFlutterUpFall.png"
};

std::vector<const char*> blue_tex = {
						 "texture//blueNormal.png", "texture//blueFlutterDownNormal.png", "texture//blueFlutterUpNormal.png",
						 "texture//blueNormalFly.png", "texture//blueFlutterDownFly.png", "texture//blueFlutterUpFly.png",
						 "texture//blueNormalFall.png", "texture//blueFlutterDownFall.png", "texture//blueFlutterUpFall.png"
};

glm::vec3 normal_scale = { 0.6f, 0.6f, 1.0f };
glm::vec3 hard_scale = { 1.0f, 1.0f, 1.0f };

/*
\  用于定义鸟的类型
*/
class Bird : public DisplayBoard, public utility::Collidable {
public:
	using BoxT = utility::Collidable::BoxType;

	Bird(const glm::vec3& pos, GLint mode = 0, GLint skin = 0, const GLfloat speed = -1000.0f)
		: DisplayBoard(skin == 0 ? origin_tex : blue_tex,
			pos,
			(mode == 0 || mode == 1) ? normal_scale : hard_scale),
		utility::Collidable(BoxT::RETENCGEL, pos, 2.0f * (BoardSp::HALFEDGE * 0.6f - 5.0f), 2.0f * (BoardSp::HALFEDGE * 0.6f - 5.0f)),
		speed_(speed)
	{}

	void fly() {
		this->speed_ = 5500.0f;
		this->setTexture(Fly);
	}

	void fall(const GLfloat deltaTime) {
		this->Board::position_.y += utility::Motion::displacement(this->speed_, deltaTime);
		// this->utility::Collidable::position().y += utility::Motion::displacement(this->speed_, deltaTime);
		this->utility::Collidable::position().y = this->Board::position_.y;
		this->speed_ = utility::Motion::velocity(this->speed_, deltaTime);
		if (this->speed_ < 1000.0f && this->speed_ > -1000.0f) {
			if (this->getIndex() > FlutterUpNormal)
				this->setTexture(Normal);
		}
		else if (this->speed_ < 0.0f) {
			if (this->getIndex() < Fall)
				this->setTexture(Fall);
		}
	}

	bool out() {
		return this->Board::position_.y <= -500;
	}

	glm::vec2 getPosition2f() {
		return glm::vec2{ this->Board::position_.x, this->Board::position_.y };
	}

	GLfloat getVelocityY() {
		return this->speed_;
	}

	GLfloat getHalfEdge() {
		return BoardSp::HALFEDGE * 0.6; //this->Board::scale_.x
	}

	// 用于实现鸟煽动翅膀的动画
	void flutter() {
		switch (this->getIndex())
		{
		case Normal:
			this->setTexture(FlutterUpNormal);
			break;
		
		case FlutterUpNormal:
			this->setTexture(FlutterDownNormal);
			break;
		
		case FlutterDownNormal:
			this->setTexture(Normal);
			break;

		case Fall:
  			this->setTexture(FlutterUpFall);
			break;

		case FlutterUpFall:
			this->setTexture(FlutterDownFall);
			break;

		case FlutterDownFall:
			this->setTexture(Fall);
			break;

		case Fly:
			this->setTexture(FlutterUpFly);
			break;

		case FlutterUpFly:
			this->setTexture(FlutterDownFly);
			break;

		case FlutterDownFly:
			this->setTexture(Fly);
			break;

		default:
			break;
		}
	}

private:
	enum { Normal = 0, FlutterDownNormal, FlutterUpNormal,
		   Fly, FlutterDownFly, FlutterUpFly,
		   Fall, FlutterDownFall, FlutterUpFall};
	GLfloat speed_;
};

#endif // !BIRD_H
