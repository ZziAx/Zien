#ifdef ZN_TINYXML_IMPLEMENTATION
#include "zn_svgb_slot_obj.h"

/**
 * @class ZN_SvgbEmojiSlot
 * @brief Represents an SVG-based emoji slot, derived from ZN_SvgbSlotObj.
 *
 * This class handles loading and initializing an SVG emoji element from an XML node.
 * It extracts attributes such as `data-unicode` and `viewBox`, parses the viewBox into
 * a structured format (`ZN_ViewBox`), and renames the XML element to a proper `<svg>` element.
 *
 * Typical usage involves constructing this class with an XML element reference (`tinyxml2::XMLElement`)
 * and allowing it to automatically parse and initialize its internal data.
 *
 * @note This class depends on TinyXML2 for XML parsing and on `ZN_ViewBox` for geometry parsing.
 */
class ZN_SvgbEmojiSlot : public ZN_SvgbSlotObj
{
private:
    /**
     * @brief Initializes the emoji slot by parsing attributes from the XML element.
     *
     * This method performs the following steps:
     * 1. Retrieves the `data-unicode` attribute and stores it in the `unicode` member.
     * 2. Retrieves and parses the `viewBox` attribute into a `ZN_ViewBox` object.
     * 3. Calls `rename_to_Svg()` to rename and serialize the XML node as `<svg>`.
     *
     * @return `ZN_OK` if initialization succeeded, or a specific `ZN_ERROR` code otherwise.
     */
    ZN_ERROR init() override
    {
        // Extract the Unicode attribute (e.g., "U+1F600" for 😀)
        ZN_ERROR err = get_attr_value("data-unicode", unicode);
        if (err)
            return err;

        // Parse the viewBox attribute (defines the visible coordinate system)
        std::string xml_viewbox;
        err = get_attr_value("viewBox", xml_viewbox);
        if (err)
            return err;

        // Convert string to structured viewbox
        viewbox = ZN_ViewBox::from_string(xml_viewbox);

        // Convert the element into a proper <svg> node
        rename_to_Svg();

        return ZN_OK;
    }

public:
    /// Unicode string representing the emoji (e.g., "1F600").
    std::string unicode;

    /**
     * @brief Default constructor (creates an empty emoji slot).
     */
    ZN_SvgbEmojiSlot() = default;

    /**
     * @brief Constructs the emoji slot from an XML element and initializes it.
     *
     * @param data Reference to a TinyXML2 `XMLElement` representing the SVG emoji definition.
     */
    ZN_SvgbEmojiSlot(XMLElement &data)
        : ZN_SvgbSlotObj(data)
    {
        init();
    }
};

#endif
