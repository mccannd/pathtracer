INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
#LIBS += -L$$PWD/lib -ltbb

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/mainwindow.cpp \
    $$PWD/mygl.cpp \
    $$PWD/scene/camera.cpp \
    $$PWD/scene/scene.cpp \
    $$PWD/bmp/EasyBMP.cpp \
    $$PWD/scene/geometry/cube.cpp \
    $$PWD/scene/geometry/mesh.cpp \
    $$PWD/scene/geometry/sphere.cpp \
    $$PWD/openGL/drawable.cpp \
    $$PWD/openGL/glwidget277.cpp \
    $$PWD/openGL/shaderprogram.cpp \
    $$PWD/raytracing/intersection.cpp \
    $$PWD/raytracing/ray.cpp \
    $$PWD/scene/transform.cpp \
    $$PWD/scene/geometry/square.cpp \
    $$PWD/tinyobj/tiny_obj_loader.cc \
    $$PWD/scene/materials/material.cpp \
    $$PWD/raytracing/film.cpp \
    $$PWD/scene/xmlreader.cpp \
    $$PWD/raytracing/integrator.cpp \
    $$PWD/cameracontrolshelp.cpp \
    $$PWD/raytracing/samplers/stratifiedpixelsampler.cpp \
    $$PWD/raytracing/samplers/uniformpixelsampler.cpp \
    $$PWD/scene/geometry/disc.cpp \
    $$PWD/scene/materials/bxdfs/lambertBxDF.cpp \
    $$PWD/scene/materials/bxdfs/bxdf.cpp \
    $$PWD/scene/materials/lightmaterial.cpp \
    $$PWD/renderthread.cpp \
    $$PWD/scene/geometry/geometry.cpp \
    $$PWD/raytracing/directlightingintegrator.cpp \
    $$PWD/scene/materials/bxdfs/blinnmicrofacetbxdf.cpp \
    $$PWD/scene/materials/bxdfs/specularreflectionbxdf.cpp \
    $$PWD/scene/materials/weightedmaterial.cpp \
    $$PWD/scene/materials/bxdfs/speculartransmission.cpp \
    $$PWD/scene/materials/fresnel.cpp \
    $$PWD/scene/materials/transmissivematerial.cpp \
    $$PWD/scene/materials/bxdfs/fresnel_specularreflection.cpp \
    $$PWD/scene/geometry/voxel.cpp \
    $$PWD/scene/geometry/perlin.cpp \
    $$PWD/scene/materials/cloudmaterial.cpp \
    $$PWD/renderrunnable.cpp \
    $$PWD/scene/materials/bxdfs/anisotropic_bxdf.cpp \
    $$PWD/scene/geometry/boundingbox.cpp \
    $$PWD/raytracing/bvhnode.cpp \
    $$PWD/raytracing/intersectionengine.cpp


HEADERS += \
    $$PWD/mainwindow.h \
    $$PWD/mygl.h \
    $$PWD/scene/camera.h \
    $$PWD/la.h \
    $$PWD/drawable.h \
    $$PWD/scene/scene.h \
    $$PWD/bmp/EasyBMP.h \
    $$PWD/bmp/EasyBMP_BMP.h \
    $$PWD/bmp/EasyBMP_DataStructures.h \
    $$PWD/bmp/EasyBMP_VariousBMPutilities.h \
    $$PWD/scene/geometry/cube.h \
    $$PWD/scene/geometry/geometry.h \
    $$PWD/scene/geometry/mesh.h \
    $$PWD/scene/geometry/sphere.h \
    $$PWD/openGL/drawable.h \
    $$PWD/openGL/glwidget277.h \
    $$PWD/openGL/shaderprogram.h \
    $$PWD/raytracing/intersection.h \
    $$PWD/raytracing/ray.h \
    $$PWD/scene/transform.h \
    $$PWD/scene/geometry/square.h \
    $$PWD/tinyobj/tiny_obj_loader.h \
    $$PWD/scene/materials/material.h \
    $$PWD/raytracing/film.h \
    $$PWD/scene/xmlreader.h \
    $$PWD/raytracing/integrator.h \
    $$PWD/cameracontrolshelp.h \
    $$PWD/raytracing/samplers/pixelsampler.h \
    $$PWD/raytracing/samplers/stratifiedpixelsampler.h \
    $$PWD/raytracing/samplers/uniformpixelsampler.h \
    $$PWD/scene/geometry/disc.h \
    $$PWD/scene/materials/bxdfs/lambertBxDF.h \
    $$PWD/scene/materials/bxdfs/bxdf.h \
    $$PWD/scene/materials/lightmaterial.h \
    $$PWD/renderthread.h \
    $$PWD/raytracing/intersectionengine.h \
    $$PWD/raytracing/directlightingintegrator.h \
    $$PWD/scene/materials/bxdfs/blinnmicrofacetbxdf.h \
    $$PWD/scene/materials/bxdfs/specularreflectionbxdf.h \
    $$PWD/scene/materials/weightedmaterial.h \
    $$PWD/scene/materials/bxdfs/speculartransmission.h \
    $$PWD/scene/materials/fresnel.h \
    $$PWD/scene/materials/transmissivematerial.h \
    $$PWD/scene/materials/bxdfs/fresnel_specularreflection.h \
    $$PWD/scene/geometry/voxel.h \
    $$PWD/scene/geometry/perlin.h \
    $$PWD/scene/materials/cloudmaterial.h \
    $$PWD/renderrunnable.h \
    $$PWD/scene/materials/bxdfs/anisotropic_bxdf.h \
    $$PWD/scene/geometry/boundingbox.h \
    $$PWD/raytracing/bvhnode.h
