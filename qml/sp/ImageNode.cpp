
#include "ImageNode.h"

#include <QQuickView>          // QQuickView class
#include <QSGTextureMaterial>  // QSGTextureMaterial class
#include <QSGTexture>          // QSGTexture class

#include "other/printDebug.h"  // PRINT_DBG, PRINT_ERR

//-------------------------------------------------------------------------------------------------
// Application view from main.cpp
extern QQuickView* p_view;

//-------------------------------------------------------------------------------------------------
using namespace Sec_145;

//-------------------------------------------------------------------------------------------------
const char* ImageNode::PREF = "[ImageNode]: ";

//-------------------------------------------------------------------------------------------------
ImageNode::ImageNode(const uint32_t verticesCount) :
    QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), verticesCount)
{
	// Set a polygon type
	setDrawingMode(QSGGeometry::DrawTriangleFan);

	// Set a geometry of this node
	setGeometry(static_cast<QSGGeometry*>(this));

	// Set a flag for this node for delete of material when node is deleted
	setFlag(QSGNode::OwnsMaterial);
}

//-------------------------------------------------------------------------------------------------
ImageNode::~ImageNode()
{
	// Delete a texture
	QSGOpaqueTextureMaterial* material =
	        dynamic_cast<QSGOpaqueTextureMaterial*>(this->material());
	if (material != nullptr) {
		QSGTexture* texture = material->texture();
		if (texture != nullptr) {
			texture->deleteLater();
		}
	}
}

//-------------------------------------------------------------------------------------------------
void ImageNode::setImage(const QImage& image)
{
	// Check for QQuickView class object pointer
	if (p_view == nullptr) {
		PRINT_ERR(true, PREF, "QQuickView class object is nullptr");
		return;
	}

	// Create a texture from image
	QSGTexture* texture = p_view->createTextureFromImage(image);
	if (texture == nullptr) {
		PRINT_ERR(true, PREF, "Can't get an QSGTexture class object from QImage class object");
		return;
	}

	// Get a material
	QSGMaterial* material_base = this->material();
	QSGTextureMaterial* material = dynamic_cast<QSGTextureMaterial*>(material_base);

	// Check the material type
	if (material == nullptr && material_base != nullptr) {
		PRINT_ERR(true, PREF, "Another type of material");
		return;
	}

	// Check the material
	if (material == nullptr) {
		// Create a new material
		material = new (std::nothrow) QSGTextureMaterial;
		if (material == nullptr) {
			PRINT_ERR(true, PREF, "Can't allocate memory for QSGTextureMaterial class object");
			return;
		}

		// Set a texture for the material
		material->setTexture(texture);

		// Set a filter for the material
		material->setFiltering(QSGTexture::Linear);

		// Set the material for this geometry node
		setMaterial(material);
	} else {
		// Delete an old texture
		QSGTexture* oldTexture = material->texture();
		if (oldTexture != nullptr) {
			oldTexture->deleteLater();
		}

		// Set a new texture for the material
		material->setTexture(texture);
	}
}
