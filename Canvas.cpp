#include "Canvas.h"
#include <stdio.h>    //printf
#include <stdlib.h>   //exit
#include "GL/glew.h"
#ifdef _WIN32
#include "GL/wglew.h"
#endif

static constexpr int   elemSize = 5;
static constexpr float fullWindow[4*elemSize]=
{
    // (u, v), (x, y, z)
    0, 0, -1, -1, 0,
    1, 0, 1, -1, 0,
    1, 1, 1, 1, 0,
    0, 1, -1, 1, 0
};

void Canvas::update()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, fbo_->width(), fbo_->height(),
                    GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,fbo_->pixels());

    glBegin(GL_QUADS);
    for(int i=0;i<elemSize*4;i+=elemSize)
    {
        glTexCoord2f(fullWindow[i],fullWindow[i+1]);
        glVertex3f(fullWindow[i+2],fullWindow[i+3],fullWindow[i+4]);
    }
    glEnd();
}

void Canvas::init()
{
    if ( glewInit() != GLEW_OK )
    {
        printf("glewInit error!\n");
        exit(1);
    }
    //BitmapLoader::OutT ptr = BitmapLoader::load("k.bmp");
    //pixels_ = ptr.release()->data();

    glViewport(0, 0, fbo_->width(), fbo_->height());

    glClearColor(255,255,255, 1);
    GLuint texture_id;
    glGenTextures(1,&texture_id);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fbo_->width(), fbo_->height(),
                 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8,fbo_->pixels());

}


