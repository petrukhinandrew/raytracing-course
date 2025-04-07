#include <iostream>
#include <algorithm>
#include "parser/FileParser.h"
#include "glm.hpp"
#include "scene/ImageScene.h"
#include "image_writer/NetPbmImageWriter.h"

int main(int argc, char* argv[]) {
    std::string scenePath = argv[1];
    std::string renderPath = argv[2];
    
    auto scene = ImageScene();
    auto parser = new TextFileParser(scene);
    parser -> parse(scenePath);
    scene.render();
    auto writer = NetpbmImageWriter();
    writer.write(renderPath, scene);
    return 0;
}