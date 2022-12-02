//#include "texturemap.h"
//#include <iostream>

//bool TextureMap::loadImageFromFile(const QString &file) {
//    QImage myImage;
////    std::vector<RGBA> textureMapRGBA;
//    if (!myImage.load(file)) {
//        std::cout<<"Failed to load in image"<<std::endl;
//        return false;
//    }
//    myImage = myImage.convertToFormat(QImage::Format_RGBX8888);
//    textureMap_width = myImage.width();
//    textureMap_height = myImage.height();
//    QByteArray arr = QByteArray::fromRawData((const char*) myImage.bits(), myImage.sizeInBytes());

//    textureMapRGBA.clear();
//    textureMapRGBA.reserve(textureMap_width * textureMap_height);
//    for (int i = 0; i < arr.size() / 4.f; i++){
//        textureMapRGBA.push_back(RGBA{(std::uint8_t) arr[4*i], (std::uint8_t) arr[4*i+1], (std::uint8_t) arr[4*i+2], (std::uint8_t) arr[4*i+3]});
//    }
////    displayImage();

//    return true;
//}

//std::vector<RGBA>* TextureMap::getTextureMapsRGBA()
//{
//    return &textureMapRGBA;
//}

//int TextureMap::getTextureMapHeight()
//{
//    return textureMap_height;
//}

//int TextureMap::getTextureMapWidth()
//{
//    return textureMap_width;
//}
