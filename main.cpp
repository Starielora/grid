#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>

#include "PerlinNoise.hpp"

#include "camera.hpp"
#include "shader.hpp"

#include <iostream>
#include <array>
#include <vector>
#include <queue>
#include <numeric>
#include <array>
#include <string_view>
#include <memory>
#include <numbers>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);

unsigned int SCR_WIDTH = 2*800;
unsigned int SCR_HEIGHT = 2*600;

camera cam;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

static constexpr auto grid_shader_paths = std::tuple{ std::string_view("D:/dev/snake-cpp/grid.vs"), std::string_view("D:/dev/snake-cpp/grid.fs") };
static constexpr auto sun_shader_paths = std::tuple{ std::string_view("D:/dev/snake-cpp/sun.vs"), std::string_view("D:/dev/snake-cpp/sun.fs") };
static constexpr auto skybox_shader_paths = std::tuple{ std::string_view("D:/dev/snake-cpp/skybox.vs"), std::string_view("D:/dev/snake-cpp/skybox.fs") };
static constexpr auto star_shader_paths = std::tuple{ std::string_view("D:/dev/snake-cpp/star.vs"), std::string_view("D:/dev/snake-cpp/star.fs")};
static constexpr auto mountains_shader_paths = std::tuple{ std::string_view("D:/dev/snake-cpp/mountains.vs"), std::string_view("D:/dev/snake-cpp/mountains.fs") };
std::unique_ptr<shader> grid_shader;
std::unique_ptr<shader> sun_shader;
std::unique_ptr<shader> skybox_shader;
std::unique_ptr<shader> star_shader;
std::unique_ptr<shader> mountains_shader;

namespace stars
{
    constexpr auto instances_count = 469;
    auto init()
    {
        auto rd = std::random_device{};
        auto gen = std::mt19937(rd());
        auto distrib = std::uniform_real_distribution(-1., 1.);

        auto stars_buffer = std::array<glm::vec3, instances_count>{};

        for (auto& star : stars_buffer)
        {
            auto x = distrib(gen);
            auto y = std::abs(distrib(gen)) + 0.15;
            auto z = distrib(gen);

            constexpr auto r = 110.;
            // TODO handle x == y == z == 0;
            const auto magnitude = r / std::sqrt(x * x + y * y + z * z);

            x *= magnitude;
            y *= magnitude;
            z *= magnitude;

            star = glm::vec3(x, y, z);
        }

        const auto VAO = gl::genVertexArray();
        const auto VBO = gl::genBuffer();
        const auto instanceVBO = gl::genBuffer();

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO); gl::checkError();
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * instances_count, stars_buffer.data(), GL_STATIC_DRAW); gl::checkError();

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribDivisor(0, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0); gl::checkError();
        glBindVertexArray(0); gl::checkError();

        return std::tuple{ VAO, VBO, instanceVBO };
    }
}

namespace mountains
{
    constexpr auto mountain_tops = 369;
    constexpr auto vertices_count = 2*mountain_tops + 2;
    auto init()
    {
        auto rd = std::random_device{};
        auto gen = std::mt19937(rd());
        auto distrib = std::uniform_real_distribution(0.75, 1.);

        const siv::PerlinNoise::seed_type seed = 123456u;
        const siv::PerlinNoise perlin{ seed };

        auto vertices = std::array<glm::vec3, vertices_count>();

        constexpr auto y_min = 9.f;
        constexpr auto y_max = 12.f;

        for(int i = 0; i < mountain_tops; i++)
        {
            const auto angle = i * (2 * std::numbers::pi / float(mountain_tops)) - std::numbers::pi / 2;
            auto x = std::cos(angle);
            auto y = perlin.noise1D_01(i);
            y = (y_max - y_min) * y + y_min;
            auto z = std::sin(angle);

            constexpr auto r = 100.;
            // TODO handle x == y == z == 0;
            const auto magnitude = r / std::sqrt(x * x + z * z);

            x *= magnitude;
            z *= magnitude;

            vertices[2*i] = glm::vec3(x, -1., z);
            vertices[2*i + 1] = glm::vec3(x, y, z);
        }

        vertices[vertices.size() - 2] = vertices[0];
        vertices[vertices.size() - 1] = vertices[1];

        constexpr auto angle_start = std::numbers::pi / 6.;
        constexpr auto angle_end = 2 * std::numbers::pi - angle_start;
        constexpr auto angle_step = (angle_end - angle_start) / float(mountain_tops);

        constexpr auto one_direction_samples_count = int(angle_start / angle_step);

        for (int i = 1; i < one_direction_samples_count; i+=2)
        {
            const auto magnitude = .0 + sin((std::numbers::pi * float(i) / one_direction_samples_count) - std::numbers::pi / 2.);
            if (magnitude <= 0.f)
            {
                vertices[i].y = -1.f;
                vertices[vertices.size() - i].y = -1.f;
            }
            else
            {
                vertices[i].y *= magnitude;
                vertices[vertices.size() - i].y *= magnitude;
            }
        }

        const auto VAO = gl::genVertexArray();
        const auto VBO = gl::genBuffer();

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        constexpr auto value_type_size = sizeof(decltype(vertices)::value_type);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * value_type_size, vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, value_type_size, nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0); gl::checkError();
        glBindVertexArray(0); gl::checkError();

        return std::tuple{VAO, VBO};
    }

}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSwapInterval(1);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_MULTISAMPLE);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    {
        grid_shader = std::make_unique<shader>(std::get<0>(grid_shader_paths), std::get<1>(grid_shader_paths));
        sun_shader = std::make_unique<shader>(std::get<0>(sun_shader_paths), std::get<1>(sun_shader_paths));
        skybox_shader = std::make_unique<shader>(std::get<0>(skybox_shader_paths), std::get<1>(skybox_shader_paths));
        star_shader = std::make_unique<shader>(std::get<0>(star_shader_paths), std::get<1>(star_shader_paths));
        mountains_shader = std::make_unique<shader>(std::get<0>(mountains_shader_paths), std::get<1>(mountains_shader_paths));

        const auto [stars_VAO, stars_VBO, stars_instances_VBO] = stars::init();
        const auto [mountains_VAO, mountains_VBO] = mountains::init();
        const auto grid_VAO = gl::genVertexArray();

        auto title = std::array<char, 20>{};
        auto frame_times = std::array<float, 100>{};
        auto frame_times_index = std::size_t{ 0 };

        while (!glfwWindowShouldClose(window))
        {
            static float frame_start_ts = 0.f;
            frame_start_ts = glfwGetTime();
            static float last_frame_end_ts = 0.f;

            float currentFrame = static_cast<float>(glfwGetTime());
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;

            processInput(window);

            glClearColor(0, 0, 0, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glm::mat4 projection = cam.projection(SCR_WIDTH, SCR_HEIGHT);
            glm::mat4 view = cam.view();

            skybox_shader->use();
            skybox_shader->setMat4("view", view);
            skybox_shader->setMat4("projection", projection);
            skybox_shader->setVec3("cameraPos", cam.position());
            glBindVertexArray(grid_VAO);
            glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 36, 1, 0);
            gl::checkError();

            sun_shader->use();
            sun_shader->setMat4("view", view);
            sun_shader->setMat4("projection", projection);
            sun_shader->setVec3("cameraPos", cam.position());
            sun_shader->setFloat("time", currentFrame);
            sun_shader->setFloat("scaleFactor", 12.f);
            sun_shader->setVec3("translation", glm::vec3(0, 5, -100) + cam.position());
            glBindVertexArray(grid_VAO);
            glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
            gl::checkError();

            star_shader->use();
            star_shader->setMat4("view", view);
            star_shader->setMat4("projection", projection);
            star_shader->setVec3("cameraPos", cam.position());
            star_shader->setFloat("time", currentFrame);
            star_shader->setFloat("scaleFactor", 0.1);
            glBindVertexArray(stars_VAO); gl::checkError();
            glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, stars::instances_count, 0);
            gl::checkError();

            mountains_shader->use();
            mountains_shader->setMat4("view", view);
            mountains_shader->setMat4("projection", projection);
            mountains_shader->setVec3("cameraPos", cam.position());
            glBindVertexArray(mountains_VAO); gl::checkError();
            glDrawArrays(GL_TRIANGLE_STRIP, 0, mountains::vertices_count);
            gl::checkError();

            grid_shader->use();
            grid_shader->setMat4("view", view);
            grid_shader->setMat4("projection", projection);
            grid_shader->setVec3("cameraPos", cam.position());
            glBindVertexArray(grid_VAO);
            glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, 1, 0);
            gl::checkError();

            glfwSwapBuffers(window);
            glfwPollEvents();

            const auto frame_total_time = (glfwGetTime() - frame_start_ts);

            frame_times[frame_times_index++ % frame_times.size()] = frame_total_time;
            const auto avg_fps = 1./(std::accumulate(frame_times.begin(), frame_times.end(), 0.f) / frame_times.size());

            // TODO fmt
            constexpr auto title_format = std::string_view("fps: %f");
            const int title_written_chars = std::snprintf(title.data(), title.size(), title_format.data(), avg_fps);
            assert(title_written_chars < title.size());
            glfwSetWindowTitle(window, title.data());
        }
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.moveForward(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.moveBack(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.strafeLeft(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.strafeRight(deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
    {
        try
        {
            {
                grid_shader = std::make_unique<shader>(std::get<0>(grid_shader_paths), std::get<1>(grid_shader_paths));
            }
            {
                sun_shader = std::make_unique<shader>(std::get<0>(sun_shader_paths), std::get<1>(sun_shader_paths));
            }
            {
                skybox_shader = std::make_unique<shader>(std::get<0>(skybox_shader_paths), std::get<1>(skybox_shader_paths));
            }
            {
                star_shader = std::make_unique<shader>(std::get<0>(star_shader_paths), std::get<1>(star_shader_paths));
            }
            {
                mountains_shader = std::make_unique<shader>(std::get<0>(mountains_shader_paths), std::get<1>(mountains_shader_paths));
            }
        }
        catch (const std::runtime_error& e)
        {
            std::cerr << e.what() << "\n";
        }
    }

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}


void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    cam.process_mouse(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    cam.fov(-yoffset);
}

