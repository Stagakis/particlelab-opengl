#include "ParticleEmitterInt.h"
#include "iostream"
#include <algorithm>
#include <execution>

ParticleEmitterInt::ParticleEmitterInt(Drawable* _model, int number) {
    model = _model;
    number_of_particles = number;
    emitter_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    p_attributes.resize(number_of_particles, particleAttributes());
    transformations.resize(number_of_particles, glm::mat4(0.0f));


    glGenBuffers(1, &transformations_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, transformations_buffer);

    std::size_t vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
}

void ParticleEmitterInt::renderParticles(float time) {
    if(time < 5)
        bindAndUpdateBuffers();
    glDrawElementsInstanced(GL_TRIANGLES, 3*model->indices.size(), GL_UNSIGNED_INT, 0, number_of_particles);
}


void ParticleEmitterInt::bindAndUpdateBuffers()
{
    //Calculate the model matrix in parallel
    std::transform(std::execution::par_unseq, p_attributes.begin(), p_attributes.end(), transformations.begin(),
        [](particleAttributes p)->glm::mat4 {
            if (p.life == 0) return glm::mat4(0.0f);
            auto r = glm::rotate(glm::mat4(), 0.0f, glm::vec3(0, 1, 0));
            auto s = glm::scale(glm::mat4(), glm::vec3(1, 1, 1));
            auto t = glm::translate(glm::mat4(), p.position);
            return t * r * s;
        
        });

    glBindVertexArray(model->VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, transformations_buffer);
    glBufferData(GL_ARRAY_BUFFER, number_of_particles * sizeof(glm::mat4), NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, number_of_particles * sizeof(glm::mat4), &transformations[0]);

}
