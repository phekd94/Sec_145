
#pragma once

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for rendering an image
TODO: * try to use a QSGTransformNode class for rotating
	  * try to use a QSGSimpleTextureNode class instead sp::ImageNode class
	  * find a way to change a type of arguments "detect" and "angle" in the setImage() and
		setEmptyImage() methods. The problem relate with call this functions in the qml code.
		unsigned long long detect -> size_t detect
		unsigned int angle -> int32_t angle
	  * How to get a node tree?	(see a code from https://www.youtube.com/watch?v=cNE6Jabxxxo)
FIXME:
DANGER:
NOTE: * this class is based on the next GitHub repository:
		https://github.com/SiberianProgrammers/sp_qt_libs
	  * see a sp.drawio in the sp directory
	  * int main(int argc, char *argv[]) {
			...
			QApplication app(argc, argv);
			QLoggingCategory::setFilterRules("qt.scenegraph.general=true");
			// for debug information about Qt Scene Graph and about graphics
			// driver which is used
			...
		}
	  * .pro: DEFINES += QSG_RENDER_LOOP=threaded (or basis, or windows)
	  * texture coordinate in the TexturedPoint2D::set() method are in the next limits: [0, 1]
	  * rotation of rectangle (image) in the updatePaintNode() method:
		"https://ru.wikipedia.org/wiki/%D0%9F%D0%BE%D0%B2%D0%BE%D1%80%D0%BE
		 %D1%82#%D0%9F%D0%BE%D0%B2%D0%BE%D1%80%D0%BE%D1%82_%D0%B2_%D0%B4%D0%B2
		 %D1%83%D0%BC%D0%B5%D1%80%D0%BD%D0%BE%D0%BC_%D0%BF%D1%80%D0%BE%D1%81
		 %D1%82%D1%80%D0%B0%D0%BD%D1%81%D1%82%D0%B2%D0%B5"

Sec_145::Image class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      NO       |    YES     |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <QQuickItem>   // QQuickItem class (for inheritance)
#include "ImageNode.h"  // Sec_145::ImageNode class (for inheritance)
#include <QImage>       // QImage class
#include <cstddef>      // size_t type
#include <cstdint>      // integer types

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
class Image: public QQuickItem, private ImageNode
{
	Q_OBJECT

public:

	Image(QQuickItem* parent = nullptr);
	virtual ~Image();

	// Sets an image
	Q_INVOKABLE void setImage(const QImage& image,
	                          const unsigned long long detect = 0,
	                          const unsigned int angle = 0);

	// Sets an image with black background
	Q_INVOKABLE void setEmptyImage();

private:

	// Reaction on QQuickItem::update() method or on another event
	virtual QSGNode* updatePaintNode(QSGNode* oldNode,
	                                 UpdatePaintNodeData* updatePaintNodeData) override final;

	// Converts degrees into radians
	double degree2radian(const uint32_t angle) const;

	// Image
	QImage m_image;

	// Angle for rotation an image
	double m_angle;
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145
