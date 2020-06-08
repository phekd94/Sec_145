
#include "Image.h"

#include <QSGGeometry>  // QSGGeometry class
#include <QVector>      // QVector class
#include <QPoint>       // QPoint class
#include <QPainter>     // QPainter class
#include <cstdint>      // integer types

#ifdef Q_OS_WIN    // for MCVS
#define _USE_MATH_DEFINES
#include <math.h>       // std::cos(), std::sin(), M_PI
#endif

#ifdef Q_OS_LINUX  // for GCC
#include <cmath>        // std::cos(), std::sin(), M_PI
#endif

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
Image::Image(QQuickItem* parent) :  QQuickItem(parent),
                                    ImageNode(4 + 3), // image node with 7 vertices
                                    m_angle(0)
{
	// Set a flag for rendering a visual content
	QQuickItem::setFlag(QQuickItem::ItemHasContents);

	// Set an empty image
	setEmptyImage();
}

//-------------------------------------------------------------------------------------------------
Image::~Image()
{
}

//-------------------------------------------------------------------------------------------------
void Image::setImage(const QImage& image,
                     const unsigned long long detect,
                     const unsigned int angle)
{	
	// Check the incoming parameter
	if (image.isNull()) {
		setEmptyImage();
		return;
	}

	// Set a new image
	m_image = image;

	// Set an angle
	m_angle = degree2radian(angle);

	// Movement is detected
	if (detect != 0) {
		// Get a vector of the points
		const QVector<QPoint>* const dt = reinterpret_cast<QVector<QPoint>*>(detect);

		// Too many detected points
		if (dt->count() > 1000) {
			update();
			return;
		}

		// Convert an image format
		if (m_image.format() != QImage::Format_RGBA8888) {
			m_image = m_image.convertToFormat(QImage::Format_RGBA8888);
		}

		// Draw rectangles
		QPainter p(&m_image);
		for (int32_t i = 0; i < dt->count(); i++) {
			if (dt->at(i).x() > 0) {
				p.fillRect(4*dt->at(i).x(), 4*dt->at(i).y(), 4, 4, Qt::red);
			}
			if (dt->at(i).x() < 0) {
				p.fillRect(-2*dt->at(i).x(), 2*dt->at(i).y(), 2, 2, Qt::red);
			}
		}
	}

	// Update an image node
	update();
}

//-------------------------------------------------------------------------------------------------
void Image::setEmptyImage()
{
	// Create an image which has a black background
	m_image = QImage(width(), height(), QImage::Format_RGB888);
	m_image.fill(Qt::black);

	// Set an angle equals 0
	m_angle = 0;

	// Update an image node
	update();
}

//-------------------------------------------------------------------------------------------------
QSGNode* Image::updatePaintNode(QSGNode*, QQuickItem::UpdatePaintNodeData*)
{
	// Set an image and inform that it has bits for rendering
	ImageNode::setImage(m_image);
	ImageNode::markDirty(QSGNode::DirtyGeometry | QSGNode::DirtyMaterial);

	// Get vertices
	::QSGGeometry* geometry = ImageNode::geometry();
	if (geometry == nullptr) {
		return static_cast<ImageNode*>(this);
	}
	::QSGGeometry::TexturedPoint2D* vertices = geometry->vertexDataAsTexturedPoint2D();

	// Set widget (which will be display an image) vertex coordinates
	// (considering a rotation)
	const double w = width();
	const double h = height();
	const double w_d2 = w / 2;
	const double h_d2 = h / 2;
	const double sin = std::sin(m_angle);
	const double cos = std::cos(m_angle);

	const double ax = (-w_d2) * cos - (-h_d2) * sin + w_d2;
	const double ay = (-w_d2) * sin + (-h_d2) * cos + h_d2;

	const double bx = (-w_d2) * cos - (h + (-h_d2)) * sin + w_d2;
	const double by = (-w_d2) * sin + (h + (-h_d2)) * cos + h_d2;

	const double cx = (w + (-w_d2)) * cos - (h + (-h_d2)) * sin + w_d2;
	const double cy = (w + (-w_d2)) * sin + (h + (-h_d2)) * cos + h_d2;

	const double dx = (w + (-w_d2)) * cos - (-h_d2) * sin + w_d2;
	const double dy = (w + (-w_d2)) * sin + (-h_d2) * cos + h_d2;

	// Set texture coordinates (interval - [0, 1])
	vertices[0].set((cx + ax) / 2.0, (cy + ay) / 2.0, 0.5, 0.5);
	vertices[1].set((dx + ax) / 2.0, (dy + ay) / 2.0, 0.5, 0);
	vertices[2].set(ax, ay, 0, 0);
	vertices[3].set(bx, by, 0, 1);
	vertices[4].set(cx, cy, 1, 1);
	vertices[5].set(dx, dy, 1, 0);
	vertices[6].set((dx + ax) / 2.0, (dy + ay) / 2.0, 0.5, 0);

	// Return an image node
	return static_cast<ImageNode*>(this);
}

//-------------------------------------------------------------------------------------------------
double Image::degree2radian(const uint32_t angle) const
{
	// Return an angle in radians
	return angle * M_PI / 180.0;
}
