/**
* @file  cusdr_oglText.cpp
* @brief OpenGL Text generation class for cuSDR
* @author Hermann von Hasseln, DL3HVH
* @version 0.1
* @date 2012-02-18
*/

/*
 *	 adapted from the MIFit project: http://code.google.com/p/mifit
 *
 *   Copyright 2012 adapted for cuSDR by Hermann von Hasseln, DL3HVH
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License version 2 as
 *   published by the Free Software Foundation
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "cusdr_oglText.h"

const int TEXTURE_SIZE = 256;
	
struct CharData {
		
		GLuint textureId;
        uint width;
        uint height;
        GLfloat s[2];
        GLfloat t[2];
};

struct OGLTextPrivate {

    OGLTextPrivate(const QFont &f);
    ~OGLTextPrivate();

    void allocateTexture();
    CharData &createCharacter(QChar c);

    QFont font;
    QFontMetrics fontMetrics;

    QHash<ushort, CharData> characters;
    QList<GLuint> textures;

    GLint xOffset;
    GLint yOffset;
};

OGLTextPrivate::OGLTextPrivate(const QFont &f)
    : font(f), fontMetrics(f), xOffset(0), yOffset(0) {}

OGLTextPrivate::~OGLTextPrivate() {
	
	foreach (GLuint texture, textures)
		glDeleteTextures(1, &texture);
}

void OGLTextPrivate::allocateTexture() {
	
	
	
	QImage image(TEXTURE_SIZE, TEXTURE_SIZE, QImage::Format_ARGB32);
	
	image.fill(Qt::transparent);
	
     QOpenGLTexture *fonttext = new QOpenGLTexture(QOpenGLTexture::Target2D);
     fonttext->setData((QOpenGLTexture::PixelFormat::RGBA));
     fonttext->setData(image);
     fonttext->allocateStorage(QOpenGLTexture::RGB_Integer, QOpenGLTexture::UInt8);
     //fonttext->allocateStorage();

     fonttext->bind();
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    textures += fonttext->textureId();
}

CharData &OGLTextPrivate::createCharacter(QChar c) {
	
	ushort unicodeC = c.unicode();
	if (characters.contains(unicodeC))
		return characters[unicodeC];
	
	if (textures.empty())
		allocateTexture();
	
	GLuint texture = textures.last();
	
	GLsizei width = fontMetrics.width(c);
	GLsizei height = fontMetrics.height();

    QPixmap pixmap(width, height);
    pixmap.fill(Qt::transparent);

	/*QImage image(width, height, QImage::Format_ARGB32_Premultiplied);
	if (!image.isNull()) {

		image.fill(Qt::transparent);

		QPainter p(&image);
		if (&font) p.setFont(this->font);
	}*/
	
    QPainter painter;
	//const QPainter::CompositionMode comp_mode = painter.compositionMode();
	//painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.begin(&pixmap);
    //painter.setRenderHints(QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::HighQualityAntialiasing | QPainter::TextAntialiasing, false);
    painter.setFont(font);
    painter.setPen(Qt::white);

    //painter.drawText(0, fontMetrics.ascent(), c);
	painter.drawText(pixmap.rect(), Qt::TextSingleLine | Qt::TextDontClip | Qt::AlignCenter, c);
    painter.end();
//    fonttexture = new QOpenGLTexture(pixmap.toImage());
   // glBindTexture(GL_TEXTURE_2D, fonttexture);
    fonttext->setData((QOpenGLTexture::PixelType::RGB8I));
    fonttext->setData(pixmap);
    fonttext->allocateStorage(QOpenGLTexture::RGB_Integer, QOpenGLTexture::UInt8);
  //  glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, width, height, GL_RGBA, GL_UNSIGNED_BYTE, fonttexture);

    CharData& character = characters[unicodeC];
    character.textureId = texture;
    character.width = width;
    character.height = height;
    character.s[0] = static_cast<GLfloat>(xOffset) / TEXTURE_SIZE;
    character.t[0] = static_cast<GLfloat>(yOffset) / TEXTURE_SIZE;
    character.s[1] = static_cast<GLfloat>(xOffset + width) / TEXTURE_SIZE;
    character.t[1] = static_cast<GLfloat>(yOffset + height) / TEXTURE_SIZE;

    xOffset += width;
    if (xOffset + fontMetrics.maxWidth() >= TEXTURE_SIZE) {
		
		xOffset = 1;
        yOffset += height;
	}
    if (yOffset + fontMetrics.height() >= TEXTURE_SIZE) {
		
		allocateTexture();
        yOffset = 1;
    }
    return character;
}



OGLText::OGLText(const QFont &f) : d(new OGLTextPrivate(f)) {}


OGLText::~OGLText() {

    delete d;
}

QFont OGLText::font() const
{
    return d->font;
}

QFontMetrics OGLText::fontMetrics() const {

    return d->fontMetrics;
}

//! Renders text at given x, y.
void OGLText::renderText(float x, float y, const QString &text) {

    const bool GL_TEXTURE_2D_wasEnabled = glIsEnabled(GL_TEXTURE_2D);
	GLint prev_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_texture);
	GLint prev_shade_model; glGetIntegerv(GL_SHADE_MODEL, &prev_shade_model);

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (prev_shade_model != GL_FLAT) glShadeModel(GL_FLAT);
	if (!GL_TEXTURE_2D_wasEnabled) glEnable(GL_TEXTURE_2D);

    GLuint texture = 0;
    glTranslatef(x, y, 0);

    for (int i = 0; i < text.length(); ++i) {

    	//if (text.length() > 80)
    	//	qDebug() << "*********************  OK";

        CharData &c = d->createCharacter(text.at(i));
        if (texture != c.textureId) {

            texture = c.textureId;
            glBindTexture(GL_TEXTURE_2D, texture);
        }

        glBegin(GL_QUADS);
			glTexCoord2f(c.s[0], c.t[0]); glVertex2f(0, c.height);
			glTexCoord2f(c.s[1], c.t[0]); glVertex2f(c.width, c.height);
			glTexCoord2f(c.s[1], c.t[1]); glVertex2f(c.width, 0);
			glTexCoord2f(c.s[0], c.t[1]); glVertex2f(0, 0);
        glEnd();

        glTranslatef(c.width, 0, 0);
    }

	glShadeModel(prev_shade_model);
	glBindTexture(GL_TEXTURE_2D, prev_texture);
	if (!GL_TEXTURE_2D_wasEnabled) glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glPopAttrib();
}

void OGLText::renderText(float x, float y, float z, const QString &text) {

    const bool GL_TEXTURE_2D_wasEnabled = glIsEnabled(GL_TEXTURE_2D);
	GLint prev_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &prev_texture);
	GLint prev_shade_model; glGetIntegerv(GL_SHADE_MODEL, &prev_shade_model);

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	if (prev_shade_model != GL_FLAT) glShadeModel(GL_FLAT);
	if (!GL_TEXTURE_2D_wasEnabled) glEnable(GL_TEXTURE_2D);

    GLuint texture = 0;
    glTranslatef(x, y, 0);
    for (int i = 0; i < text.length(); ++i) {

        CharData &c = d->createCharacter(text.at(i));
        if (texture != c.textureId) {

            texture = c.textureId;
            glBindTexture(GL_TEXTURE_2D, texture);
        }

        /*glBegin(GL_QUADS);
			glTexCoord2f(c.s[0], c.t[0]); glVertex2f(0, c.height);
			glTexCoord2f(c.s[1], c.t[0]); glVertex2f(c.width, c.height);
			glTexCoord2f(c.s[1], c.t[1]); glVertex2f(c.width, 0);
			glTexCoord2f(c.s[0], c.t[1]); glVertex2f(0, 0);
        glEnd();*/
		glBegin(GL_QUADS);
			glTexCoord2f(c.s[0], c.t[0]); glVertex3f(0, c.height, z);
			glTexCoord2f(c.s[1], c.t[0]); glVertex3f(c.width, c.height, z);
			glTexCoord2f(c.s[1], c.t[1]); glVertex3f(c.width, 0, z);
			glTexCoord2f(c.s[0], c.t[1]); glVertex3f(0, 0, z);
        glEnd();

        glTranslatef(c.width, 0, 0);
    }

	glShadeModel(prev_shade_model);
	glBindTexture(GL_TEXTURE_2D, prev_texture);
	if (!GL_TEXTURE_2D_wasEnabled) glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    glPopAttrib();
}
