#if ZN_TINYXML_IMPLEMENTATION

#include "tinyxml2.h"
using namespace tinyxml2;


char *copy_to(const char *inp)
{
    char *s = new char[strlen(inp) + 1];
    strcpy(s, inp);

    return s;
}

/**
 * @brief Represents a single SVGB slot object, typically a <symbol> element.
 *
 * Handles parsing XML attributes, serializing content to <svg> elements,
 * and managing related metadata such as width, height, scale, and pixmap data.
 */
class ZN_SvgbSlotObj:public ZN_XmlParser
{
private:
    /**
     * @brief Initializes the SVGB object (override in derived classes).
     */
    virtual ZN_ERROR init()
    {
        return ZN_OK;
    }

public:
    std::string svg_data;                 ///< Serialized SVG content
    int width;                            ///< Width of the slot
    int height;                           ///< Height of the slot
    float scale = 1.0f;                   ///< Scale factor
    const uint8_t *pixmap_data = nullptr; ///< Raw pixmap data
    int pixmap_len = 0;                   ///< Length of pixmap
    ZN_ViewBox viewbox;                   ///< ViewBox info
    XMLElement &data;                     ///< Reference to TinyXML2 element

    /**
     * @brief Constructor with XML element.
     */
    ZN_SvgbSlotObj(XMLElement &data) : data(data) {}

    // virtual ZN_ERROR load_buffer()
    // {
    //     return ZN_OK;
    // }

    /**
     * @brief Retrieves an attribute value from the XML element.
     * @param name Name of the attribute
     * @param out Output string
     * @return ZN_ERROR code
     */
    ZN_ERROR get_attr_value(char *name, std::string &out)
    {
        const char *attr = data.Attribute(name);
        if (!attr)
            return ZN_ERR_SVGB_ATTR_NOT_FOUND;

        out = std::string(copy_to(attr));
        return ZN_OK;
    }

    /**
     * @brief Converts the current <symbol> element into a serialized <svg> element.
     */
    void rename_to_Svg()
    {
        XMLDocument *doc = data.GetDocument();
        XMLElement *svg = doc->NewElement("svg");

        // Copy attributes
        for (const XMLAttribute *attr = data.FirstAttribute(); attr; attr = attr->Next())
            svg->SetAttribute(attr->Name(), attr->Value());

        // Move children
        for (XMLNode *child = data.FirstChild(); child;)
        {
            XMLNode *next = child->NextSibling();
            svg->InsertEndChild(child);
            child = next;
        }

        svg_data = get_text(svg);
    }

    /**
     * @brief Serializes the XML element and its children to string.
     * @param e Pointer to XML element
     * @return Serialized string
     */
    std::string get_text(XMLElement *e)
    {
        XMLPrinter printer;
        e->Accept(&printer);
        return std::string(printer.CStr());
    }

    /**
     * @brief Loads buffer data for the SVGB object.
     */
    virtual ZN_ERROR load_buffer()

    {

        int result = get_svg_buffer_from_buffer(
            reinterpret_cast<const uint8_t *>(svg_data.data()), // const uint8_t* OK
            static_cast<int>(svg_data.size()),                  // int len
            scale,                                              // float scale
            &pixmap_data,                                       // const uint8_t** out_ptr
            &pixmap_len,                                        // int* out_len
            &width,                                             // int* width
            &height                                             // int* height
        );

        return ZN_OK;
    }

    virtual void get_size(int &width, int &height)
    {
        width = this->width * scale;
        height = this->height * scale;
    }
};

#endif
