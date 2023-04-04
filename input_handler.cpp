#include "input_handler.h"
# include <GLFW/glfw3.h>

void KeyboardInput(GLFWwindow* window, Camera camera, float delta_time) {
	float camera_speed = 2.5f * delta_time;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.position += camera_speed * camera.front;
	}
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.position -= camera_speed * camera.front;
	}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.position -= glm::normalize(glm::cross(camera.front, camera.up)) * camera_speed;
	}
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.position += glm::normalize(glm::cross(camera.front, camera.up)) * camera_speed;
	}
}
void MouseInput(GLFWwindow* window, Camera camera) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (last_mouse_pos.x == 0.0f && last_mouse_pos.y == 0.0f) {
            last_mouse_pos.x = xpos;
            last_mouse_pos.y = ypos;
        }

        float xoffset = xpos - last_mouse_pos.x;
        float yoffset = last_mouse_pos.y - ypos; // Inverti l'asse y

        last_mouse_pos.x = xpos;
        last_mouse_pos.y = ypos;

        xoffset *= camera.mouse_sensitivity;
        yoffset *= camera.mouse_sensitivity;

        camera.yaw += xoffset;
        camera.pitch += yoffset;

        // Limita la rotazione in modo che la camera non si capovolga
        if (camera.pitch > 89.0f) {
            camera.pitch = 89.0f;
        }
        if (camera.pitch < -89.0f) {
            camera.pitch = -89.0f;
        }
    }
    else {
        last_mouse_pos.x = 0.0f;
        last_mouse_pos.y = 0.0f;
    }

    float scroll_offset = glfwGetScrollY(window);
    camera.position += camera.front * scroll_offset;
}