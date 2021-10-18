#include <iostream>

#include "Clock.h"
#include "ImageManager.h"
#include "SceneLoader.h"
#include "Scene.h"
#include "GP_ONE.h"

const int TEST_ITERATIONS = 1000;

bool compareFrameBuffers(FrameBuffer &fBuf1, FrameBuffer &fBuf2, int* count) {
    auto isEqual = true;

    for (uint16_t i = 0; i < FRAMEBUFFER_BUF_SIZE; ++i) {
        if (fBuf1.color[i] != fBuf2.color[i]) {
            isEqual = false;
            (*count)++;
        }
    }

    return isEqual;
}

double testScene(const std::string &sceneName) {
    Clock clock;
    double time;
    FrameBuffer frameBuffer;
    FrameBuffer reference = ImageManager::loadFrameBuffer("Scenes/Render/"+sceneName+".png");
    Scene scene = SceneLoader::loadScene("Scenes/"+sceneName+".txt");

    clock.restart();
    for (int i = 0; i < TEST_ITERATIONS; ++i) {
        GP_ONE::clearFrameBuffer(scene.bkgColor);
        GP_ONE::loadSprites(scene.sprites.data(), scene.sprites.size());
        GP_ONE::drawSpriteInstances(scene.spriteInstances.data(), scene.spriteInstances.size());
        GP_ONE::saveFrameBuffer(frameBuffer);
    }
    time = clock.getTime();
    int count = 0;
    std::string result = compareFrameBuffers(frameBuffer, reference, &count) ? "PASSED" : "FAILED";
    std::cout <<sceneName<<" : "<<result<<" : "<<time<<" seconds" << " Miss count : " << count << std::endl;

    ImageManager::saveFrameBuffer("Render/"+sceneName+".png", frameBuffer);

    return time;
}

void runTests() {
    double total_time = 0.0;

    for (int i = 0; i < 5; ++i) {
        total_time += testScene("scene_"+std::to_string(i));
    }

    std::cout << "Total time = " << total_time <<" seconds"<< std::endl;
}

int main() {
    runTests();

    return 0;
}