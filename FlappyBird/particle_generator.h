#ifndef PARTICLE_GENERATOR_H
#define PARTICLE_GENERATOR_H
#include <vector>

#include <cstddef>
#include <memory>
#include "GL\glew.h"
#include "GL\SOIL.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\type_ptr.hpp"
#include "drawAble.h"
#include "shader.h"
#include "config.h"


// Represents a single particle and its state
struct Particle {
    glm::vec2 Position, Velocity;
    glm::vec4 Color;
    GLfloat Life;

    Particle() : Position(0.0f), Velocity(0.0f), Color(1.0f), Life(0.0f) { }
};


// ParticleGenerator acts as a container for rendering a large number of 
// particles by repeatedly spawning and updating particles and killing 
// them after a given amount of time.
class ParticleGenerator : DrawAble
{
public:
    // Constructor
    ParticleGenerator(GLuint amount = 500) {
        // Set up mesh and attribute properties
        GLfloat particle_quad[] = {
            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,

            0.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f, 0.0f
        };

        if (!this->textureLoaded_) {
            glGenTextures(1, &this->texture_);
            glBindTexture(GL_TEXTURE_2D, this->texture_);

            int textureWidth, textureHeight;
            unsigned char* image = SOIL_load_image("texture//particle.png", &textureWidth, &textureHeight, 0, SOIL_LOAD_RGBA);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
            glGenerateMipmap(GL_TEXTURE_2D);

            SOIL_free_image_data(image);
            glBindTexture(GL_TEXTURE_2D, 0);

            this->textureLoaded_ = true;
        }

        glGenVertexArrays(1, &this->VAO_);
        glBindVertexArray(this->VAO_);

        GLuint VBO;
        glGenBuffers(1, &VBO);
        // Fill mesh buffer
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(particle_quad), particle_quad, GL_STATIC_DRAW);
        // Set mesh attributes
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glBindVertexArray(0);

        // Create this->amount default particle instances
        this->amount_ = amount;
        for (GLuint i = 0; i < this->amount_; ++i)
            this->particles_.push_back(Particle());
    }
    // Update all particles
    void update(GLfloat dt, glm::vec2 pos, glm::vec2 velocity, GLuint newParticles, glm::vec2 offset = glm::vec2(0.0f, 0.0f)) {
        // Add new particles 
        for (GLuint i = 0; i < newParticles; ++i)
        {
            int unusedParticle = this->firstUnusedParticle();
            this->respawnParticle(this->particles_[unusedParticle], pos, velocity, offset);
        }
        // Update all particles
        for (GLuint i = 0; i < this->amount_; ++i)
        {
            Particle& p = this->particles_[i];
            p.Life -= dt * 10; // reduce life
            if (p.Life > 0.0f)
            {	// particle is alive, thus update
                //p.Position -= p.Velocity * dt;
                p.Position.x -= 2500 * dt;
                p.Color.a -= dt * 7.5;
                // cout << p.Color.a << endl;
            }
        }
    }
    // Render all particles
    void draw(Shader& shader) override {
        // Use additive blending to give it a 'glow' effect
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        shader.use();
        for (Particle particle : this->particles_)
        {
            if (particle.Life > 0.0f)
            {
                //cout << "particle: " << "posX: " << particle.Position.x << " posY: " << particle.Position.y << endl;
                glUniformMatrix4fv(glGetUniformLocation(shader.getProgram(), "projection"), 1, GL_FALSE, glm::value_ptr(PROJECTION));
                glUniform2f(glGetUniformLocation(shader.getProgram(), "offset"), particle.Position.x, particle.Position.y);
                glUniform4f(glGetUniformLocation(shader.getProgram(), "color"), particle.Color.r, particle.Color.g, particle.Color.b, particle.Color.a);
                //cout << particle.Color.a << endl;
                glBindTexture(GL_TEXTURE_2D, this->texture_);
                glBindVertexArray(this->VAO_);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
            }
        }
        // Don't forget to reset to default blending mode
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
private:
    // State
    std::vector<Particle> particles_;
    GLuint amount_;
    static GLuint texture_;
    static bool textureLoaded_;
    GLuint VAO_;
    // Stores the index of the last particle used (for quick access to next dead particle)
    GLuint lastUsedParticle_ = 0;

    // Returns the first Particle index that's currently unused e.g. Life <= 0.0f or 0 if no particle is currently inactive
    GLuint firstUnusedParticle() {
        // First search from last used particle, this will usually return almost instantly
        for (GLuint i = lastUsedParticle_; i < this->amount_; ++i) {
            if (this->particles_[i].Life <= 0.0f) {
                lastUsedParticle_ = i;
                return i;
            }
        }
        // Otherwise, do a linear search
        for (GLuint i = 0; i < lastUsedParticle_; ++i) {
            if (this->particles_[i].Life <= 0.0f) {
                lastUsedParticle_ = i;
                return i;
            }
        }
        // All particles are taken, override the first one (note that if it repeatedly hits this case, more particles should be reserved)
        lastUsedParticle_ = 0;
        return 0;
    }
    // Respawns particle
    void respawnParticle(Particle& particle, glm::vec2 pos, glm::vec2 velocity, glm::vec2 offset = glm::vec2(0.0f, 0.0f)) {
        GLfloat random = ((rand() % 100) - 50) / 10.0f;
        GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);
        particle.Position = pos + random; // + offset
        particle.Color = glm::vec4(rColor, rColor, rColor, 1.0f);
        particle.Life = 1.0f;
        particle.Velocity = velocity * 0.1f;
    }
};

GLuint ParticleGenerator::texture_;
bool ParticleGenerator::textureLoaded_ = false;

#endif