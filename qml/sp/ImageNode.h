
#ifndef SEC_145_QML_IMAGENODE_H
#define SEC_145_QML_IMAGENODE_H

//-------------------------------------------------------------------------------------------------
/*
DESCRITION: class for node of the image
TODO:
FIXME:
DANGER:
NOTE: * this class is based on the next GitHub repository:
		https://github.com/SiberianProgrammers/sp_qt_libs
	  * see a sp.drawio in the sp directory
	  * Define of QQuickView* p_view pointer is necessary in another .cpp
		file. Most possibly in main.cpp; for example:
		...
		QQuickView* p_view;
		...
		int main() {
			...
			QQuickView view;
			p_view = &view;
			...
		}

Sec_145::ImageNode class
+---------------+------------+
| thread safety | reentrance |
+---------------+------------+
|      NO       |    YES     |
+---------------+------------+
*/

//-------------------------------------------------------------------------------------------------
#include <QSGGeometryNode>  // QSGGeometryNode class (for inheritance)
#include <QSGGeometry>      // QSGGeometry class (for inheritance)
#include <QImage>           // QImage class
#include <cstdint>          // integer types

#include <QSGImageNode>

//-------------------------------------------------------------------------------------------------
namespace Sec_145 {

//-------------------------------------------------------------------------------------------------
class ImageNode : public QSGGeometryNode, private QSGGeometry
{
public:
	explicit ImageNode(const uint32_t verticesCount);
	virtual ~ImageNode();

	// Sets an image
	void setImage(const QImage& image);
};

//-------------------------------------------------------------------------------------------------
} // namespace Sec_145

//-------------------------------------------------------------------------------------------------
#endif // SEC_145_QML_IMAGENODE_H
