#ifndef SCOREBOARD_H
#define SCOREBOARD_H

#include<array>
#include <vector>
#include "displayBoard.h"
#include "drawAble.h"

// 用于展示分数的展板
class ScoreBoard : public DrawAble {
public:
	ScoreBoard(const glm::vec3 pos = { 0.0f, 0.0f, 0.0f }, 
		const glm::vec3 scale = { 1.0f, 1.0f, 1.0f }, 
		const int val = 0,
		const std::vector<const char*> &texs = {"texture//0.png", "texture//1.png", "texture//2.png", "texture//3.png",
												"texture//4.png", "texture//5.png", "texture//6.png", 
												"texture//7.png", "texture//8.png", "texture//9.png", 
												"texture//empty.png", "texture//pause.png"})
		: boards_{ DisplayBoard(texs, {pos.x + 2 * BoardSp::HALFEDGE * scale.x, pos.y, pos.z}, scale),
				   DisplayBoard(texs, pos, scale),
				   DisplayBoard(texs, {pos.x - 2 * BoardSp::HALFEDGE * scale.x, pos.y, pos.z}, scale) },
		value_(val) 
	{
		this->setRun();
		if (value_ < 10) {
			boards_[Right].setTexture(value_);
			boards_[Middle].setTexture(EmptyIndex);
			boards_[Left].setTexture(EmptyIndex);
		}
		else if (value_ < 100) {
			boards_[Right].setTexture(value_ % 10);
			boards_[Middle].setTexture(value_ / 10);
			boards_[Left].setTexture(EmptyIndex);
		}
		else if (value_ < 1000) {
			boards_[Right].setTexture(value_ % 10);
			boards_[Middle].setTexture(value_ / 10 % 10);
			boards_[Left].setTexture(value_ % 100);
		}
		else
			return;
	}

	void draw(Shader &shader) override {
		boards_[Right].draw(shader);
		boards_[Middle].draw(shader);
		boards_[Left].draw(shader);
	}

	void setValue(const int val) {
		value_ = val;

		if (paused_) {
			boards_[Right].setTexture(EmptyIndex);
			boards_[Middle].setTexture(PauseIndex);
			boards_[Left].setTexture(EmptyIndex);
		}
		else {
			if (value_ < 10) {
				boards_[Right].setTexture(value_);
				boards_[Middle].setTexture(EmptyIndex);
				boards_[Left].setTexture(EmptyIndex);
			}
			else if (value_ < 100) {
				boards_[Right].setTexture(value_ % 10);
				boards_[Middle].setTexture(value_ / 10);
				boards_[Left].setTexture(EmptyIndex);
			}
			else if (value_ < 1000) {
				boards_[Right].setTexture(value_ % 10);
				boards_[Middle].setTexture(value_ / 10 % 10);
				boards_[Left].setTexture(value_ / 100);
			}
			else
				return;
		}
	}

	int getValue() const noexcept { return this->value_; }

	void setPause() { 
		this->paused_ = true;
		this->setValue(this->value_);
	}

	void setRun() { 
		this->paused_ = false;
		this->setValue(this->value_);
	}

private:
	const static size_t EmptyIndex;
	const static size_t PauseIndex;
	enum {Right = 0, Middle = 1, Left = 2};
	int value_;
	bool paused_ = false;
	std::array<DisplayBoard, 3> boards_;
};


const size_t ScoreBoard::EmptyIndex = 10;
const size_t ScoreBoard::PauseIndex = 11;

#endif // !SCOREBOARD_H
